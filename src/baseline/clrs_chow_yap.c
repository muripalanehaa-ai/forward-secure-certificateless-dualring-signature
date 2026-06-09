/*
 * clrs_chow_yap_pbc.c
 *
 * Certificateless Ring Signature — Chow & Yap Construction (Section 4.1)
 * Real cryptographic implementation using PBC (Pairing-Based Cryptography)
 * library for true bilinear pairings ê: G1 × G1 → GT.
 *
 * This replaces the earlier BN_mod_exp simulation with:
 *   - Real G1 elliptic curve points (Type-A pairing, BN254-like)
 *   - Real GT elements in the target group
 *   - True bilinear map via pairing_apply()
 *   - Hash-to-point via element_from_hash() for H0, H1, H2
 *
 * Scheme (Chow & Yap §4.1):
 *   Setup  : KGC picks s ∈ Zr, P = generator ∈ G1,
 *             Ppub = s·P,  g = ê(P,P) ∈ GT
 *   PKGen  : QID = H0(ID) ∈ G1,
 *             DID = 1/(s + H0_scalar(ID)) · P   (partial private key)
 *   UKGen  : xID ∈ Zr random,  RID = xID·QID,
 *             yID = H2(RID) ∈ Zr,
 *             SID = 1/(xID + yID) · DID
 *   Sign   : vi random ∈ Zr for i≠A,  VIDi = vi·P
 *             r random ∈ Zr
 *             u = g^r · Π_{i≠A} ê(VIDi, RIDi + yIDi·QIDi)
 *             h = H1(m, u, L, R) ∈ Zr
 *             VIDA = (h+r)·SIDA
 *   Verify : g^h · u  ==  Π ê(VIDi, RIDi + yIDi·QIDi)
 *
 * Dependencies:
 *   PBC   (https://crypto.stanford.edu/pbc/)   — libpbc
 *   GMP   — libgmp
 *   OpenSSL — libssl, libcrypto  (SHA-256 for domain-separated hash)
 *
 * Build (Linux):
 *   gcc -O2 -o clrs_chow_yap_pbc clrs_chow_yap_pbc.c \
 *       -lpbc -lgmp -lssl -lcrypto -lm
 *
 * Build (macOS Homebrew):
 *   OSSL=$(brew --prefix openssl@3)
 *   PBC=$(brew --prefix pbc)
 *   gcc -O2 clrs_chow_yap_pbc.c -o clrs_chow_yap_pbc \
 *       -I$OSSL/include -I$PBC/include \
 *       -L$OSSL/lib -L$PBC/lib \
 *       -lpbc -lgmp -lssl -lcrypto -lm
 *
 * Install on Ubuntu/Debian:
 *   sudo apt-get install libpbc-dev libgmp-dev libssl-dev
 *
 * Output CSVs:
 *   results_ring_size_clrs_pbc.csv
 *   results_epochs_clrs_pbc.csv
 *   results_bench_iter_clrs_pbc.csv
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <pbc/pbc.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

/* ─── Configuration ─── */
#define MSG      "Hello, Chow-Yap CLRS World!"
#define HASH_LEN 32

/*
 * PBC Type-A pairing parameters.
 * Type-A: symmetric pairing ê: G1 × G1 → GT.
 * r ≈ 2^160 (160-bit group order), q ≈ 2^512.
 * This is a well-studied parameter set used in pairing-based crypto papers.
 */
static const char PBC_PARAM[] =
    "type a\n"
    "q 8780710799663312522437781984754049815806883199414208211028"
    "6533992664756308802229570786251794226622214231558587695823174"
    "5927771336731780149849847514539320811155911149418018073513572"
    "3497893578374861766012427250094254770977857095771\n"
    "h 1201601226489114607938882136674053415806880543579936244538"
    "6993130675991766009656918605801506911198175826767832\n"
    "r 730750818665451621361119245571504901405976559617\n"
    "exp2 159\n"
    "exp1 107\n"
    "sign1 1\n"
    "sign0 1\n";

/* ─── Sweep ranges ─── */
static const int RING_SIZES[]  = {2,4,8,16,32,64,128,256,512,1024};
static const int N_RING        = 10;

static const int EPOCHS[]      = {1,2,3,4,5,6,7,8,9,10,
                                   11,12,13,14,15,16,17,18,19,20};
static const int N_EPOCHS      = 20;

/*
 * BENCH_ITERS: every 5 from 5 to 200 (40 data points).
 * Changed from the original 100-step-100 range to give fine-grained
 * visibility in the 0–200 iteration range.
 */
static const int BENCH_ITERS[] = {
      5,  10,  15,  20,  25,  30,  35,  40,  45,  50,
     55,  60,  65,  70,  75,  80,  85,  90,  95, 100,
    105, 110, 115, 120, 125, 130, 135, 140, 145, 150,
    155, 160, 165, 170, 175, 180, 185, 190, 195, 200
};
static const int N_BENCH = 40;

/* ─── Global pairing state ─── */
static pairing_t  G_pairing;
static element_t  G_P;      /* generator P ∈ G1          */
static element_t  G_Ppub;   /* Ppub = s·P ∈ G1           */
static element_t  G_g;      /* g = ê(P,P) ∈ GT           */
static element_t  G_s;      /* KGC master secret s ∈ Zr  */
static int        G_init = 0;

/* ─── Timing ─── */
static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
}

/* ─── Hash helpers ─── */

/*
 * sha256_tagged: domain-separated SHA-256.
 * Returns a 32-byte digest = SHA256(tag || data).
 */
static void sha256_tagged(uint8_t tag, const uint8_t *data, size_t dlen,
                           uint8_t out[32]) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, &tag, 1);
    EVP_DigestUpdate(ctx, data, dlen);
    unsigned int olen = 32;
    EVP_DigestFinal_ex(ctx, out, &olen);
    EVP_MD_CTX_free(ctx);
}

/*
 * H0: identity string → G1 point  QID = H0(ID)
 * Uses element_from_hash which maps arbitrary bytes to a group element.
 */
static void H0_to_G1(const char *id, element_t out) {
    size_t  len = strlen(id) + 1;
    uint8_t *buf = (uint8_t*)malloc(len);
    buf[0] = 0x00;
    memcpy(buf + 1, id, len - 1);
    element_from_hash(out, buf, (int)len);
    free(buf);
}

/*
 * H0_scalar: identity string → Zr scalar  (used for DID computation)
 * = SHA256(0x00 || ID) interpreted as Zr element.
 */
static void H0_scalar(const char *id, element_t out_zr) {
    uint8_t digest[32];
    sha256_tagged(0x00, (const uint8_t*)id, strlen(id), digest);
    element_from_hash(out_zr, digest, 32);
}

/*
 * H1: (message, u ∈ GT, identities[], R_points[]) → Zr scalar
 * Serialises all inputs, SHA-256s with tag 0x01, maps to Zr.
 */
static void H1_to_Zr(const char *m, element_t u_gt,
                      const char **ids, int n,
                      element_t *R_pts,
                      element_t out_zr) {
    int gt_len = element_length_in_bytes(u_gt);
    int g1_len = element_length_in_bytes(R_pts[0]);
    size_t mlen  = strlen(m);
    size_t total = 1 + mlen + (size_t)gt_len;
    for (int i = 0; i < n; i++) total += strlen(ids[i]) + 1 + (size_t)g1_len;

    uint8_t *buf = (uint8_t*)malloc(total);
    size_t off = 0;

    buf[off++] = 0x01;
    memcpy(buf + off, m, mlen); off += mlen;
    element_to_bytes(buf + off, u_gt); off += gt_len;

    for (int i = 0; i < n; i++) {
        size_t idlen = strlen(ids[i]);
        memcpy(buf + off, ids[i], idlen); off += idlen;
        buf[off++] = 0x00;
        element_to_bytes(buf + off, R_pts[i]); off += g1_len;
    }

    element_from_hash(out_zr, buf, (int)off);
    free(buf);
}

/*
 * H2: G1 point RID → Zr scalar  yID = H2(RID)
 */
static void H2_to_Zr(element_t R_pt, element_t out_zr) {
    int len = element_length_in_bytes(R_pt);
    uint8_t *buf = (uint8_t*)malloc(len + 1);
    buf[0] = 0x02;
    element_to_bytes(buf + 1, R_pt);
    element_from_hash(out_zr, buf, len + 1);
    free(buf);
}

/* ─── Setup ─── */
/*
 * Setup(1^λ):
 *   Initialise pairing (Type-A).
 *   Pick generator P ∈ G1.
 *   Pick KGC master secret s ∈ Zr.
 *   Ppub = s·P.
 *   g = ê(P, P) ∈ GT.
 */
static void Setup(void) {
    if (G_init) {
        element_clear(G_P);
        element_clear(G_Ppub);
        element_clear(G_g);
        element_clear(G_s);
        pairing_clear(G_pairing);
    }

    pairing_init_set_str(G_pairing, PBC_PARAM);

    element_init_G1(G_P,    G_pairing);
    element_init_G1(G_Ppub, G_pairing);
    element_init_GT(G_g,    G_pairing);
    element_init_Zr(G_s,    G_pairing);

    element_random(G_P);
    element_random(G_s);
    element_mul_zn(G_Ppub, G_P, G_s);

    /* g = ê(P, P) ∈ GT — real bilinear pairing */
    pairing_apply(G_g, G_P, G_P, G_pairing);

    G_init = 1;
}

/* ─── Data structures ─── */
typedef struct {
    char id[64];

    element_t Q_pt;   /* QID = H0(ID)          ∈ G1  */
    element_t D_pt;   /* DID = 1/(s+h0) · P    ∈ G1  */

    element_t x_id;   /* secret xID             ∈ Zr  */
    element_t R_pt;   /* RID = xID · QID        ∈ G1  */
    element_t y_id;   /* yID = H2(RID)          ∈ Zr  */
    element_t S_pt;   /* SID = 1/(xID+yID)·DID  ∈ G1  */

    int ukgen_done;
} UserKey;

typedef struct {
    element_t  u_gt;
    element_t *V;
    int        n;
} RingSig;

/* ─── PKGen ─── */
/*
 * PKGen (KGC side):
 *   QID = H0(ID) ∈ G1
 *   h0  = H0_scalar(ID) ∈ Zr
 *   DID = 1/(s + h0) · P
 */
static void PKGen(UserKey *uk, const char *id) {
    strncpy(uk->id, id, 63);
    uk->id[63] = '\0';

    element_init_G1(uk->Q_pt, G_pairing);
    element_init_G1(uk->D_pt, G_pairing);

    H0_to_G1(id, uk->Q_pt);

    element_t h0;
    element_init_Zr(h0, G_pairing);
    H0_scalar(id, h0);

    element_t denom;
    element_init_Zr(denom, G_pairing);
    element_add(denom, G_s, h0);

    element_t inv_denom;
    element_init_Zr(inv_denom, G_pairing);
    element_invert(inv_denom, denom);

    element_mul_zn(uk->D_pt, G_P, inv_denom);

    element_clear(h0);
    element_clear(denom);
    element_clear(inv_denom);

    uk->ukgen_done = 0;
}

/* ─── UKGen ─── */
/*
 * UKGen (user side):
 *   xID = random ∈ Zr
 *   RID = xID · QID
 *   yID = H2(RID) ∈ Zr
 *   SID = 1/(xID + yID) · DID
 */
static void UKGen(UserKey *uk) {
    if (uk->ukgen_done) {
        element_clear(uk->x_id);
        element_clear(uk->R_pt);
        element_clear(uk->y_id);
        element_clear(uk->S_pt);
    }

    element_init_Zr(uk->x_id, G_pairing);
    element_init_G1(uk->R_pt, G_pairing);
    element_init_Zr(uk->y_id, G_pairing);
    element_init_G1(uk->S_pt, G_pairing);

    element_random(uk->x_id);
    element_mul_zn(uk->R_pt, uk->Q_pt, uk->x_id);
    H2_to_Zr(uk->R_pt, uk->y_id);

    element_t x_plus_y, inv_xy;
    element_init_Zr(x_plus_y, G_pairing);
    element_init_Zr(inv_xy,   G_pairing);
    element_add(x_plus_y, uk->x_id, uk->y_id);
    element_invert(inv_xy, x_plus_y);

    element_mul_zn(uk->S_pt, uk->D_pt, inv_xy);

    element_clear(x_plus_y);
    element_clear(inv_xy);

    uk->ukgen_done = 1;
}

/* ─── Sign ─── */
static RingSig *Sign(const char *m, UserKey *ukeys, int signer, int n) {
    RingSig *sig = (RingSig*)malloc(sizeof(RingSig));
    sig->n = n;
    sig->V = (element_t*)malloc(n * sizeof(element_t));
    for (int i = 0; i < n; i++)
        element_init_G1(sig->V[i], G_pairing);

    element_t *v_scalars = (element_t*)malloc(n * sizeof(element_t));
    for (int i = 0; i < n; i++)
        element_init_Zr(v_scalars[i], G_pairing);

    /* Step 1: for i ≠ signer */
    for (int i = 0; i < n; i++) {
        if (i == signer) continue;
        element_random(v_scalars[i]);
        element_mul_zn(sig->V[i], G_P, v_scalars[i]);
    }

    /* Step 2 */
    element_t r;
    element_init_Zr(r, G_pairing);
    element_random(r);

    /* Step 3: u = g^r */
    element_init_GT(sig->u_gt, G_pairing);
    element_pow_zn(sig->u_gt, G_g, r);

    for (int i = 0; i < n; i++) {
        if (i == signer) continue;

        element_t y_fresh;
        element_init_Zr(y_fresh, G_pairing);
        H2_to_Zr(ukeys[i].R_pt, y_fresh);

        element_t yQ, tmp_G1;
        element_init_G1(yQ,     G_pairing);
        element_init_G1(tmp_G1, G_pairing);
        element_mul_zn(yQ, ukeys[i].Q_pt, y_fresh);
        element_add(tmp_G1, ukeys[i].R_pt, yQ);

        element_t pair_i;
        element_init_GT(pair_i, G_pairing);
        pairing_apply(pair_i, sig->V[i], tmp_G1, G_pairing);

        element_mul(sig->u_gt, sig->u_gt, pair_i);

        element_clear(y_fresh);
        element_clear(yQ);
        element_clear(tmp_G1);
        element_clear(pair_i);
    }

    /* Step 4: h = H1(m, u, L, R) */
    const char **id_arr = (const char**)malloc(n * sizeof(char*));
    for (int i = 0; i < n; i++) id_arr[i] = ukeys[i].id;

    element_t *R_arr = (element_t*)malloc(n * sizeof(element_t));
    for (int i = 0; i < n; i++) {
        element_init_G1(R_arr[i], G_pairing);
        element_set(R_arr[i], ukeys[i].R_pt);
    }

    element_t h;
    element_init_Zr(h, G_pairing);
    H1_to_Zr(m, sig->u_gt, id_arr, n, R_arr, h);
    for (int i = 0; i < n; i++) element_clear(R_arr[i]);
    free(R_arr);
    free(id_arr);

    /* Step 5: VIDA = (h + r)·SIDA */
    element_t h_plus_r;
    element_init_Zr(h_plus_r, G_pairing);
    element_add(h_plus_r, h, r);
    element_mul_zn(sig->V[signer], ukeys[signer].S_pt, h_plus_r);

    element_clear(r);
    element_clear(h);
    element_clear(h_plus_r);
    for (int i = 0; i < n; i++) element_clear(v_scalars[i]);
    free(v_scalars);

    return sig;
}

/* ─── Verify ─── */
static int Verify(const char *m, const RingSig *sig,
                  UserKey *ukeys, int n) {
    const char **id_arr = (const char**)malloc(n * sizeof(char*));
    element_t   *R_arr  = (element_t*)  malloc(n * sizeof(element_t));
    for (int i = 0; i < n; i++) {
        id_arr[i] = ukeys[i].id;
        element_init_G1(R_arr[i], G_pairing);
        element_set(R_arr[i], ukeys[i].R_pt);
    }

    element_t h;
    element_init_Zr(h, G_pairing);
    H1_to_Zr(m, sig->u_gt, id_arr, n, R_arr, h);
    free(id_arr);
    for (int i = 0; i < n; i++) element_clear(R_arr[i]);
    free(R_arr);

    /* LHS = g^h · u */
    element_t LHS, gh;
    element_init_GT(LHS, G_pairing);
    element_init_GT(gh,  G_pairing);
    element_pow_zn(gh, G_g, h);
    element_mul(LHS, gh, sig->u_gt);
    element_clear(gh);
    element_clear(h);

    /* RHS = Π ê(VIDi, RIDi + yIDi·QIDi) */
    element_t RHS;
    element_init_GT(RHS, G_pairing);
    element_set1(RHS);

    for (int i = 0; i < n; i++) {
        element_t y_fresh;
        element_init_Zr(y_fresh, G_pairing);
        H2_to_Zr(ukeys[i].R_pt, y_fresh);

        element_t yQ, tmp_G1;
        element_init_G1(yQ,     G_pairing);
        element_init_G1(tmp_G1, G_pairing);
        element_mul_zn(yQ, ukeys[i].Q_pt, y_fresh);
        element_add(tmp_G1, ukeys[i].R_pt, yQ);

        element_t pair_i;
        element_init_GT(pair_i, G_pairing);
        pairing_apply(pair_i, sig->V[i], tmp_G1, G_pairing);

        element_mul(RHS, RHS, pair_i);

        element_clear(y_fresh);
        element_clear(yQ);
        element_clear(tmp_G1);
        element_clear(pair_i);
    }

    int ok = (element_cmp(LHS, RHS) == 0);
    element_clear(LHS);
    element_clear(RHS);
    return ok;
}

/* ─── Memory helpers ─── */
static void free_sig(RingSig *s) {
    if (!s) return;
    element_clear(s->u_gt);
    for (int i = 0; i < s->n; i++) element_clear(s->V[i]);
    free(s->V);
    free(s);
}

static void free_ukey(UserKey *uk) {
    element_clear(uk->Q_pt);
    element_clear(uk->D_pt);
    if (uk->ukgen_done) {
        element_clear(uk->x_id);
        element_clear(uk->R_pt);
        element_clear(uk->y_id);
        element_clear(uk->S_pt);
    }
}

static UserKey *build_ring(int n) {
    UserKey *ukeys = (UserKey*)calloc(n, sizeof(UserKey));
    char id_buf[32];
    for (int i = 0; i < n; i++) {
        snprintf(id_buf, sizeof(id_buf), "user_%d", i);
        PKGen(&ukeys[i], id_buf);
        UKGen(&ukeys[i]);
    }
    return ukeys;
}

static void free_ring(UserKey *ukeys, int n) {
    for (int i = 0; i < n; i++) free_ukey(&ukeys[i]);
    free(ukeys);
}

static size_t compute_sig_bytes(int n) {
    size_t gt_bytes = (size_t)pairing_length_in_bytes_GT(G_pairing);
    size_t g1_bytes = (size_t)pairing_length_in_bytes_compressed_G1(G_pairing);
    return gt_bytes + (size_t)n * g1_bytes;
}

/* ─── MAIN ─── */
int main(void) {
    printf("Certificateless Ring Signature — Chow & Yap (Real PBC Pairings)\n");
    printf("Initialising Type-A pairing (160-bit r)...\n");

    double t0 = now_ms();
    Setup();
    printf("Setup done in %.3f ms\n\n", now_ms() - t0);

    /* ── Correctness check ── */
    printf("--- Correctness Check ---\n");
    {
        int n = 4;
        UserKey *ukeys = build_ring(n);
        RingSig *sig   = Sign(MSG, ukeys, 0, n);
        int v1 = Verify(MSG, sig, ukeys, n);
        printf("  Sign+Verify (n=4)    : %s\n",
               v1 ? "ACCEPT [OK]" : "REJECT [FAIL]");
        int v2 = Verify("TAMPERED!!!", sig, ukeys, n);
        printf("  Tampered msg verify  : %s  [expected REJECT]\n",
               v2 ? "ACCEPT [FAIL]" : "REJECT [OK]");
        free_sig(sig);
        free_ring(ukeys, n);
    }
    printf("\n");

    FILE *f_ring  = fopen("results_ring_size_clrs_pbc.csv",  "w");
    FILE *f_epoch = fopen("results_epochs_clrs_pbc.csv",     "w");
    FILE *f_bench = fopen("results_bench_iter_clrs_pbc.csv", "w");

    if (!f_ring || !f_epoch || !f_bench) {
        fprintf(stderr, "ERROR: Could not open output CSV files.\n");
        return 1;
    }

    /* ─── SWEEP 1: Ring Size ─── */
    fprintf(f_ring, "ring_size,sign_ms,verify_ms,sig_bytes\n");
    printf("=== SWEEP 1: Ring Size ===\n");
    printf("%-12s %-12s %-12s %-12s\n",
           "ring_size","sign_ms","verify_ms","sig_bytes");

    for (int ri = 0; ri < N_RING; ri++) {
        int n     = RING_SIZES[ri];
        int iters = (n >= 256 ? 10 : n >= 64 ? 30 : 100);

        UserKey *ukeys = build_ring(n);
        double s_tot = 0, v_tot = 0;

        for (int it = 0; it < iters; it++) {
            double ts = now_ms();
            RingSig *s = Sign(MSG, ukeys, 0, n);
            s_tot += now_ms() - ts;
            double tv = now_ms();
            Verify(MSG, s, ukeys, n);
            v_tot += now_ms() - tv;
            free_sig(s);
        }
        double avg_s = s_tot / iters, avg_v = v_tot / iters;
        size_t sb = compute_sig_bytes(n);
        fprintf(f_ring, "%d,%.6f,%.6f,%zu\n", n, avg_s, avg_v, sb);
        printf("%-12d %-12.4f %-12.4f %-12zu\n", n, avg_s, avg_v, sb);
        free_ring(ukeys, n);
    }
    fclose(f_ring);

    /* ─── SWEEP 2: Epoch / Key-Update Rounds ─── */
    fprintf(f_epoch, "epoch,sign_ms,verify_ms\n");
    printf("\n=== SWEEP 2: Epoch / Key-Update Rounds (n=4, 50 iters) ===\n");
    printf("%-12s %-12s %-12s\n", "epoch","sign_ms","verify_ms");

    for (int ei = 0; ei < N_EPOCHS; ei++) {
        int ep    = EPOCHS[ei];
        int n     = 4;
        int iters = 50;
        UserKey *ukeys = build_ring(n);
        for (int e = 1; e < ep; e++)
            for (int i = 0; i < n; i++) UKGen(&ukeys[i]);

        double s_tot = 0, v_tot = 0;
        for (int it = 0; it < iters; it++) {
            double ts = now_ms();
            RingSig *s = Sign(MSG, ukeys, 0, n);
            s_tot += now_ms() - ts;
            double tv = now_ms();
            Verify(MSG, s, ukeys, n);
            v_tot += now_ms() - tv;
            free_sig(s);
        }
        fprintf(f_epoch, "%d,%.6f,%.6f\n", ep, s_tot/iters, v_tot/iters);
        printf("%-12d %-12.4f %-12.4f\n", ep, s_tot/iters, v_tot/iters);
        free_ring(ukeys, n);
    }
    fclose(f_epoch);

    /* ─── SWEEP 3: Benchmark Iterations (n=2, step-5 from 5→200) ─── */
    fprintf(f_bench, "bench_iters,sign_ms,verify_ms\n");
    printf("\n=== SWEEP 3: Benchmark Iterations (n=2, step 5, range 5-200) ===\n");
    printf("%-14s %-12s %-12s\n", "bench_iters","sign_ms","verify_ms");

    {
        int n = 2;
        UserKey *ukeys = build_ring(n);
        for (int bi = 0; bi < N_BENCH; bi++) {
            int    iters = BENCH_ITERS[bi];
            double s_tot = 0, v_tot = 0;
            for (int it = 0; it < iters; it++) {
                double ts = now_ms();
                RingSig *s = Sign(MSG, ukeys, 0, n);
                s_tot += now_ms() - ts;
                double tv = now_ms();
                Verify(MSG, s, ukeys, n);
                v_tot += now_ms() - tv;
                free_sig(s);
            }
            fprintf(f_bench, "%d,%.6f,%.6f\n", iters, s_tot/iters, v_tot/iters);
            printf("%-14d %-12.4f %-12.4f\n", iters, s_tot/iters, v_tot/iters);
        }
        free_ring(ukeys, n);
    }
    fclose(f_bench);

    printf("\nDone! CSV files written:\n");
    printf("  results_ring_size_clrs_pbc.csv\n");
    printf("  results_epochs_clrs_pbc.csv\n");
    printf("  results_bench_iter_clrs_pbc.csv\n");

    element_clear(G_P);
    element_clear(G_Ppub);
    element_clear(G_g);
    element_clear(G_s);
    pairing_clear(G_pairing);
    return 0;
}
