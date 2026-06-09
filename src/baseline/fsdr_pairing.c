/*
 * fs_ring_pbc_fsdr_pairing.c
 *
 * Forward-Secure Ring Signature — Bilinear Pairing Construction
 * Faithful C port of the Python/Charm implementation using PBC library.
 *
 * Matches Python exactly:
 *   setup()  : g ∈ G1, h ∈ G2, hvec[0..5] ∈ G2, Ft = hvec[0] * Π hvec[i]^tvec[i]
 *   keygen() : SK = [g^r, h^sk * Ft^r],  pk = g^sk
 *   sign()   : R = pair(Π pk_j^cj, h) / pair(g^rhat, Ft)   (real GT pairing)
 *              σ1 = SK[1]^c * Ft^rhat1
 *              σ2 = g^rhat2 / SK[0]^c
 *   verify() : A = pair(g, σ1), B = pair(σ2, Ft)
 *              C = pair(Π pk_j^cj, h)
 *              R' = C / (A·B)
 *              check H(R'||m||PK) == c - Σcj
 *
 * Dependencies:
 *   PBC  library  (https://crypto.stanford.edu/pbc/)
 *   GMP  library
 *   OpenSSL (SHA-256)
 *
 * Build:
 *   gcc -O2 -o fs_ring_pbc_fsdr_pairing fs_ring_pbc_fsdr_pairing.c \
 *       -lpbc -lgmp -lssl -lcrypto -lm
 *
 * Install PBC on Ubuntu/Debian:
 *   sudo apt-get install libpbc-dev libgmp-dev libssl-dev
 *
 * Output CSVs (new names, won't overwrite existing files):
 *   results_ring_size_fsdr_pairing.csv
 *   results_epochs_fsdr_pairing.csv
 *   results_bench_iter_fsdr_pairing.csv
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
#define MSG         "Hello, FS-Pairing Ring World!"
#define HASH_LEN    32
#define HVEC_LEN    6      /* hvec[0..5]  — matches Python range(0,6) */
#define TVEC_LEN    6      /* tvec[0..5]                               */

/* PBC type-A pairing parameters (160-bit r, 512-bit q) */
static const char PBC_PARAM[] =
    "type a\n"
    "q 87807107996633125224377819847540498158068831994142082"
    "1102865339926647563088022295707862517942266222142315585"
    "8769582317459277713367317801498498475145393208111559111"
    "4941795418901807351357234978935783748617660124272500942"
    "5477097785709577\n"
    "h 12016012264891146079388821366740534204802954401251311"
    "822919615131047207289359704531102844802183906537786776\n"
    "r 730750818665451621361119245571504901405976559617\n"
    "exp2 159\n"
    "exp1 107\n"
    "sign1 1\n"
    "sign0 1\n";

/* Sweep ranges — identical to Python/previous C */
static const int RING_SIZES[]  = {2,4,8,16,32,64,128,256,512,1024};
static const int N_RING        = 10;
static const int EPOCHS[]      = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
static const int N_EPOCHS      = 20;
#define BENCH_START 0
#define BENCH_END   200
#define BENCH_STEP  5

#define N_BENCH (((BENCH_END - BENCH_START)/BENCH_STEP) + 1)

/* ─── Global pairing state ─── */
static pairing_t   G_pairing;
static element_t   G_g;          /* generator  g  ∈ G1  */
static element_t   G_h;          /* generator  h  ∈ G2  */
static element_t   G_hvec[HVEC_LEN];   /* hvec[0..5] ∈ G2 */
static element_t   G_Ft;         /* F(t) = hvec[0] * Π hvec[i]^tvec[i] ∈ G2 */
static int         G_tvec[TVEC_LEN] = {0,1,2,1,2,1};  /* matches Python */



static int G_initialized = 0;

/* Helper: element_pow_zn with long int exponent */
static void element_pow_zn_si(element_t out, element_t base, long exp){
    element_t e;
    element_init_Zr(e, G_pairing);
    element_set_si(e, exp);
    element_pow_zn(out, base, e);
    element_clear(e);
}

/* ─── Timing ─── */
static double now_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
}

/* ─── Hash helpers ─── */
/*
 * Serialise a GT element to bytes, SHA-256 it, return hex string.
 * Matches Python:  sha256.update(group.serialize(R)); c_ = sha256.hexdigest()
 */
static void gt_to_hex(element_t e, char out_hex[65]){
    int len = element_length_in_bytes(e);
    uint8_t *buf = (uint8_t*)malloc(len);
    element_to_bytes(buf, e);
    uint8_t digest[32];
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, buf, len);
    unsigned int dlen = 32;
    EVP_DigestFinal_ex(ctx, digest, &dlen);
    EVP_MD_CTX_free(ctx);
    free(buf);
    for(int i=0;i<32;i++) sprintf(out_hex+2*i, "%02x", digest[i]);
    out_hex[64] = '\0';
}

/*
 * Serialise a G1 product-of-pks to bytes, SHA-256 it, return hex string.
 * Matches Python:  sha256.update(group.serialize(Π pk[i])); c_ = sha256.hexdigest()
 */
static void g1_to_hex(element_t e, char out_hex[65]){
    int len = element_length_in_bytes(e);
    uint8_t *buf = (uint8_t*)malloc(len);
    element_to_bytes(buf, e);
    uint8_t digest[32];
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, buf, len);
    unsigned int dlen = 32;
    EVP_DigestFinal_ex(ctx, digest, &dlen);
    EVP_MD_CTX_free(ctx);
    free(buf);
    for(int i=0;i<32;i++) sprintf(out_hex+2*i, "%02x", digest[i]);
    out_hex[64] = '\0';
}

/*
 * Derive Zr element from seed string.
 * Matches Python:  c = group.hash(seed, ZR)
 */
static void hash_seed_to_zr(const char *seed, element_t out){
    unsigned char digest[SHA256_DIGEST_LENGTH];

    SHA256(
        (const unsigned char*)seed,
        strlen(seed),
        digest
    );

    element_from_hash(
        out,
        digest,
        SHA256_DIGEST_LENGTH
    );
}

/* ─── Setup ─── */
/*
 * Python setup():
 *   g  = random G1
 *   h  = random G2
 *   hvec[i] = random G2  for i in 0..5
 *   Ft = hvec[0] * Π_{i=1}^{5} hvec[i]^tvec[i]
 */
static void Setup(void){
    if(G_initialized){
        element_clear(G_g); element_clear(G_h); element_clear(G_Ft);
        for(int i=0;i<HVEC_LEN;i++) element_clear(G_hvec[i]);
        pairing_clear(G_pairing);
    }

    pairing_init_set_str(G_pairing, PBC_PARAM);

    element_init_G1(G_g, G_pairing);
    element_init_G2(G_h, G_pairing);
    element_init_G2(G_Ft, G_pairing);
    for(int i=0;i<HVEC_LEN;i++)
        element_init_G2(G_hvec[i], G_pairing);

    /* g = random G1, h = random G2 */
    element_random(G_g);
    element_random(G_h);

    /* hvec[i] = random G2 */
    for(int i=0;i<HVEC_LEN;i++)
        element_random(G_hvec[i]);

    /* Ft = hvec[0] * Π hvec[i]^tvec[i]  for i=1..5 */
    element_set(G_Ft, G_hvec[0]);
    element_t tmp;
    element_init_G2(tmp, G_pairing);
    for(int i=1;i<HVEC_LEN;i++){
        if(G_tvec[i] == 0) continue;
        element_pow_zn_si(tmp, G_hvec[i], G_tvec[i]);   /* hvec[i]^tvec[i] */
        element_mul(G_Ft, G_Ft, tmp);
    }
    element_clear(tmp);
    G_initialized = 1;
}

/* ─── Data structures ─── */

/*
 * Secret key: SK[0] = g^r  (G1),  SK[1] = h^sk * Ft^r  (G2)
 * Public key: pk = g^sk    (G1)
 */
typedef struct {
    element_t sk0;   /* g^r    ∈ G1 */
    element_t sk1;   /* h^sk * Ft^r ∈ G2 */
    element_t sk_zr; /* sk ∈ Zr  (kept for sign) */
    element_t r_zr;  /* r  ∈ Zr  (kept for KeyUp) */
} UserSK;

typedef struct {
    element_t pk;    /* g^sk ∈ G1 */
} UserPK;

/*
 * Ring signature: (σ1 ∈ G2, σ2 ∈ G1, cs[0..n-1] ∈ Zr)
 */
typedef struct {
    element_t sigma1;    /* G2 */
    element_t sigma2;    /* G1 */
    element_t *cs;       /* Zr array, length n */
    int        n;
    int        epoch;    /* for F(t) selection */
} RingSig;

/* ─── KeyGen ─── */
/*
 * Python keygen():
 *   sk = random Zr
 *   r  = random Zr
 *   SK[0] = g^r
 *   SK[1] = h^sk * Ft^r
 *   pk    = g^sk
 */
static void KeyGen(UserSK *usk, UserPK *upk){
    element_init_Zr(usk->sk_zr, G_pairing);
    element_init_Zr(usk->r_zr,  G_pairing);
    element_init_G1(usk->sk0,   G_pairing);
    element_init_G2(usk->sk1,   G_pairing);
    element_init_G1(upk->pk,    G_pairing);

    element_random(usk->sk_zr);
    element_random(usk->r_zr);

    /* SK[0] = g^r */
    element_pow_zn(usk->sk0, G_g, usk->r_zr);

    /* SK[1] = h^sk * Ft^r */
    element_t hsk, Ftr;
    element_init_G2(hsk, G_pairing);
    element_init_G2(Ftr, G_pairing);
    element_pow_zn(hsk, G_h,  usk->sk_zr);
    element_pow_zn(Ftr, G_Ft, usk->r_zr);
    element_mul(usk->sk1, hsk, Ftr);
    element_clear(hsk); element_clear(Ftr);

    /* pk = g^sk */
    element_pow_zn(upk->pk, G_g, usk->sk_zr);
}

/* ─── KeyUp ─── */
/*
 * Forward-secure key update: re-randomise with fresh z ∈ Zr.
 * (Python does not implement KeyUp; this is the paper's Section 4 addition.)
 *   r'    = r + z
 *   SK[0] = g^r'
 *   SK[1] = h^sk * Ft^r'
 * Old r and z are deleted after update (forward security).
 */
static void KeyUp(UserSK *usk){
    element_t z;
    element_init_Zr(z, G_pairing);
    element_random(z);

    /* r' = r + z */
    element_add(usk->r_zr, usk->r_zr, z);

    /* SK[0] = g^r' */
    element_pow_zn(usk->sk0, G_g, usk->r_zr);

    /* SK[1] = h^sk * Ft^r' */
    element_t hsk, Ftr;
    element_init_G2(hsk, G_pairing);
    element_init_G2(Ftr, G_pairing);
    element_pow_zn(hsk, G_h,  usk->sk_zr);
    element_pow_zn(Ftr, G_Ft, usk->r_zr);
    element_mul(usk->sk1, hsk, Ftr);
    element_clear(hsk); element_clear(Ftr);

    /* Securely erase z (forward security) */
    element_set0(z);
    element_clear(z);
}

/* ─── Sign ─── */
/*
 * Python sign(pp, sk, pk, m):
 *   cs[0] = placeholder (signer's slot)
 *   cs[1..n-1] = random Zr
 *
 *   rhat1, rhat2 = random Zr;  rhat = rhat1 + rhat2
 *   tmp = Π_{j=1}^{n-1} pk[j]^cs[j]
 *   R   = pair(tmp, h) / pair(g^rhat, Ft)
 *
 *   seed = hex(SHA256(R)) + m + hex(SHA256(Π pk))
 *   c    = group.hash(seed, ZR)
 *   cs[0] = c + Σ_{j=1}^{n-1} cs[j]     ← note: Python adds, not subtracts
 *
 *   σ1 = SK[1]^c * Ft^rhat1
 *   σ2 = g^rhat2 / SK[0]^c
 *
 * Signer is always index 0 (matches Python: cs = [1, random, ...])
 */
static RingSig *Sign(const char *m, int epoch,
                     UserSK *sks, UserPK *pks, int s_idx, int n){
    RingSig *sig = (RingSig*)malloc(sizeof(RingSig));
    sig->n     = n;
    sig->epoch = epoch;
    sig->cs    = (element_t*)malloc(n * sizeof(element_t));

    /* Initialise all cs in Zr */
    for(int i=0;i<n;i++)
        element_init_Zr(sig->cs[i], G_pairing);

    /* cs[s_idx] = 1 (placeholder), cs[j≠s_idx] = random */
    element_set1(sig->cs[s_idx]);
    for(int j=0;j<n;j++){
        if(j == s_idx) continue;
        element_random(sig->cs[j]);
    }

    /* rhat1, rhat2 ∈ Zr; rhat = rhat1 + rhat2 */
    element_t rhat1, rhat2, rhat;
    element_init_Zr(rhat1, G_pairing);
    element_init_Zr(rhat2, G_pairing);
    element_init_Zr(rhat,  G_pairing);
    element_random(rhat1);
    element_random(rhat2);
    element_add(rhat, rhat1, rhat2);

    /* tmp = Π_{j≠s_idx} pk[j]^cs[j]  ∈ G1 */
    element_t tmp;
    element_init_G1(tmp, G_pairing);
    element_set1(tmp);
    for(int j=0;j<n;j++){
        if(j == s_idx) continue;
        element_t pkc;
        element_init_G1(pkc, G_pairing);
        element_pow_zn(pkc, pks[j].pk, sig->cs[j]);
        element_mul(tmp, tmp, pkc);
        element_clear(pkc);
    }

    /* R = pair(tmp, h) / pair(g^rhat, Ft)  ∈ GT */
    element_t R, pair1, pair2, grhat;
    element_init_GT(R,     G_pairing);
    element_init_GT(pair1, G_pairing);
    element_init_GT(pair2, G_pairing);
    element_init_G1(grhat, G_pairing);

    element_pow_zn(grhat, G_g, rhat);
    pairing_apply(pair1, tmp,   G_h,    G_pairing);  /* pair(tmp,   h)  */
    pairing_apply(pair2, grhat, G_Ft,   G_pairing);  /* pair(g^rhat,Ft) */
    element_div(R, pair1, pair2);                     /* pair1 / pair2   */

    /* seed = hex(sha256(R)) + m + hex(sha256(Π pk)) */
    char hex_R[65], hex_pk[65];
    gt_to_hex(R, hex_R);

    element_t all_pk;
    element_init_G1(all_pk, G_pairing);
    element_set1(all_pk);
    for(int i=0;i<n;i++) element_mul(all_pk, all_pk, pks[i].pk);
    g1_to_hex(all_pk, hex_pk);
    element_clear(all_pk);

    char *seed = (char*)malloc(64 + strlen(m) + 64 + 4);
    sprintf(seed, "%s%s%s", hex_R, m, hex_pk);

    /* c = hash(seed, ZR) */
    element_t c;
    element_init_Zr(c, G_pairing);
    hash_seed_to_zr(seed, c);
    free(seed);

    /* cs[s_idx] = c + Σ_{j≠s_idx} cs[j]  (Python: c = c + cs[j]) */
    element_set(sig->cs[s_idx], c);
    for(int j=0;j<n;j++){
        if(j == s_idx) continue;
        element_add(sig->cs[s_idx], sig->cs[s_idx], sig->cs[j]);
    }

    /* σ1 = SK[1]^c * Ft^rhat1  ∈ G2 */
    element_init_G2(sig->sigma1, G_pairing);
    {
        element_t sk1c, Ftr1;
        element_init_G2(sk1c, G_pairing);
        element_init_G2(Ftr1, G_pairing);
        element_pow_zn(sk1c, sks[s_idx].sk1, c);
        element_pow_zn(Ftr1, G_Ft, rhat1);
        element_mul(sig->sigma1, sk1c, Ftr1);
        element_clear(sk1c); element_clear(Ftr1);
    }

    /* σ2 = g^rhat2 / SK[0]^c  ∈ G1 */
    element_init_G1(sig->sigma2, G_pairing);
    {
        element_t gr2, sk0c;
        element_init_G1(gr2,  G_pairing);
        element_init_G1(sk0c, G_pairing);
        element_pow_zn(gr2,  G_g,            rhat2);
        element_pow_zn(sk0c, sks[s_idx].sk0, c);
        element_div(sig->sigma2, gr2, sk0c);
        element_clear(gr2); element_clear(sk0c);
    }

    element_clear(rhat1); element_clear(rhat2); element_clear(rhat);
    element_clear(tmp); element_clear(R);
    element_clear(pair1); element_clear(pair2); element_clear(grhat);
    element_clear(c);
    return sig;
}

/* ─── Verify ─── */
/*
 * Python verify():
 *   A     = pair(g, σ1)
 *   B     = pair(σ2, Ft)
 *   tmp   = Π_{j=0}^{n-1} pk[j]^cs[j]
 *   C     = pair(tmp, h)
 *   R'    = C / (A * B)
 *
 *   seed  = hex(sha256(R')) + m + hex(sha256(Π pk))
 *   c'    = group.hash(seed, ZR)
 *   sum_cj = Σ_{j=1}^{n-1} cs[j]
 *   return (c' == cs[0] - sum_cj)   ← Python returns 0 on success
 */
static int Verify(const char *m, const RingSig *sig, UserPK *pks, int n){
    /* A = pair(g, σ1) ∈ GT */
    element_t A, B, C, AB, Rprime;
    element_init_GT(A,      G_pairing);
    element_init_GT(B,      G_pairing);
    element_init_GT(C,      G_pairing);
    element_init_GT(AB,     G_pairing);
    element_init_GT(Rprime, G_pairing);

    pairing_apply(A, G_g,       sig->sigma1, G_pairing);  /* pair(g,  σ1) */
    pairing_apply(B, sig->sigma2, G_Ft,      G_pairing);  /* pair(σ2, Ft) */

    /* tmp = Π pk[j]^cs[j] ∈ G1 */
    element_t tmp;
    element_init_G1(tmp, G_pairing);
    element_set1(tmp);
    for(int j=0;j<n;j++){
        element_t pkc;
        element_init_G1(pkc, G_pairing);
        element_pow_zn(pkc, pks[j].pk, sig->cs[j]);
        element_mul(tmp, tmp, pkc);
        element_clear(pkc);
    }
    pairing_apply(C, tmp, G_h, G_pairing);  /* pair(tmp, h) */
    element_clear(tmp);

    /* AB = A * B;  R' = C / AB */
    element_mul(AB, A, B);
    element_div(Rprime, C, AB);

    /* seed = hex(sha256(R')) + m + hex(sha256(Π pk)) */
    char hex_R[65], hex_pk[65];
    gt_to_hex(Rprime, hex_R);

    element_t all_pk;
    element_init_G1(all_pk, G_pairing);
    element_set1(all_pk);
    for(int i=0;i<n;i++) element_mul(all_pk, all_pk, pks[i].pk);
    g1_to_hex(all_pk, hex_pk);
    element_clear(all_pk);

    char *seed = (char*)malloc(64 + strlen(m) + 64 + 4);
    sprintf(seed, "%s%s%s", hex_R, m, hex_pk);

    element_t cprime;
    element_init_Zr(cprime, G_pairing);
    hash_seed_to_zr(seed, cprime);
    free(seed);

    /* sum_cj = Σ_{j=1}^{n-1} cs[j]  (all except index 0, matching Python) */
    element_t sum_cj;
    element_init_Zr(sum_cj, G_pairing);
    element_set0(sum_cj);
    for(int j=1;j<n;j++)
        element_add(sum_cj, sum_cj, sig->cs[j]);

    /* check: c' == cs[0] - sum_cj  →  c' + sum_cj == cs[0] */
    element_t lhs;
    element_init_Zr(lhs, G_pairing);
    element_add(lhs, cprime, sum_cj);

    int ok = (element_cmp(lhs, sig->cs[0]) == 0);

    element_clear(A); element_clear(B); element_clear(C);
    element_clear(AB); element_clear(Rprime);
    element_clear(cprime); element_clear(sum_cj); element_clear(lhs);
    return ok;
}

/* ─── Memory helpers ─── */
static void free_sig(RingSig *s){
    if(!s) return;
    element_clear(s->sigma1);
    element_clear(s->sigma2);
    for(int i=0;i<s->n;i++) element_clear(s->cs[i]);
    free(s->cs);
    free(s);
}

static void free_keys(UserSK *sks, UserPK *pks, int n){
    for(int i=0;i<n;i++){
        element_clear(sks[i].sk0);
        element_clear(sks[i].sk1);
        element_clear(sks[i].sk_zr);
        element_clear(sks[i].r_zr);
        element_clear(pks[i].pk);
    }
}

// /* Helper: element_pow_zn with long int exponent */
// static void element_pow_zn_si(element_t out, element_t base, long exp){
//     element_t e;
//     element_init_Zr(e, G_pairing);
//     element_set_si(e, exp);
//     element_pow_zn(out, base, e);
//     element_clear(e);
// }

/* ─── MAIN ─── */
int main(void){
    printf("Forward-Secure PBC Ring Signature — Dynamic Benchmark\n");
    printf("Initialising pairing (Type-A, 160-bit r)...\n");
    double t0 = now_ms();
    Setup();
    printf("Setup done in %.3f ms\n\n", now_ms()-t0);

    FILE *f_ring  = fopen("results_ring_size_fsdr_pairing.csv",  "w");
    FILE *f_epoch = fopen("results_epochs_fsdr_pairing.csv",     "w");
    FILE *f_bench = fopen("results_bench_iter_fsdr_pairing.csv", "w");

    /* ─── 1. Ring Size Sweep (epoch=1, iters=100) ─── */
    fprintf(f_ring, "ring_size,sign_ms,verify_ms,sig_bytes\n");
    printf("=== SWEEP 1: Ring Size ===\n");
    printf("%-12s %-12s %-12s %-12s\n","ring_size","sign_ms","verify_ms","sig_bytes");

    for(int ri=0;ri<N_RING;ri++){
        int n     = RING_SIZES[ri];
        int iters = 100;

        UserSK *sks = (UserSK*)calloc(n, sizeof(UserSK));
        UserPK *pks = (UserPK*)calloc(n, sizeof(UserPK));
        for(int i=0;i<n;i++) KeyGen(&sks[i], &pks[i]);
        /* one KeyUp to enter epoch 1 */
        for(int i=0;i<n;i++) KeyUp(&sks[i]);

        double s_tot=0, v_tot=0;
        for(int it=0;it<iters;it++){
            double ts = now_ms();
            RingSig *s = Sign(MSG, 1, sks, pks, 0, n);
            s_tot += now_ms()-ts;
            double tv = now_ms();
            Verify(MSG, s, pks, n);
            v_tot += now_ms()-tv;
            free_sig(s);
        }
        double avg_s = s_tot/iters, avg_v = v_tot/iters;

        /* sig size: σ1 (G2) + σ2 (G1) + n scalars (Zr) */
        size_t g1_bytes = (size_t)pairing_length_in_bytes_compressed_G1(G_pairing);
        size_t g2_bytes = (size_t)pairing_length_in_bytes_compressed_G2(G_pairing);
        size_t zr_bytes = (size_t)pairing_length_in_bytes_Zr(G_pairing);
        size_t sig_bytes = g2_bytes + g1_bytes + (size_t)n * zr_bytes;

        fprintf(f_ring,"%d,%.6f,%.6f,%zu\n",n,avg_s,avg_v,sig_bytes);
        printf("%-12d %-12.4f %-12.4f %-12zu\n",n,avg_s,avg_v,sig_bytes);
        free_keys(sks,pks,n); free(sks); free(pks);
    }
    fclose(f_ring);

    /* ─── 2. Epoch Sweep (n=4, iters=50) ─── */
    fprintf(f_epoch,"epoch,sign_ms,verify_ms\n");
    printf("\n=== SWEEP 2: Epoch Count ===\n");
    printf("%-12s %-12s %-12s\n","epoch","sign_ms","verify_ms");
    {
        int n=4, iters=50;
        for(int ei=0;ei<N_EPOCHS;ei++){
            int ep = EPOCHS[ei];
            UserSK *sks=(UserSK*)calloc(n,sizeof(UserSK));
            UserPK *pks=(UserPK*)calloc(n,sizeof(UserPK));
            for(int i=0;i<n;i++) KeyGen(&sks[i],&pks[i]);
            /* evolve to epoch ep */
            for(int e=1;e<=ep;e++)
                for(int i=0;i<n;i++) KeyUp(&sks[i]);

            double s_tot=0, v_tot=0;
            for(int it=0;it<iters;it++){
                double ts=now_ms();
                RingSig *s=Sign(MSG,ep,sks,pks,0,n);
                s_tot+=now_ms()-ts;
                double tv=now_ms();
                Verify(MSG,s,pks,n);
                v_tot+=now_ms()-tv;
                free_sig(s);
            }
            fprintf(f_epoch,"%d,%.6f,%.6f\n",ep,s_tot/iters,v_tot/iters);
            printf("%-12d %-12.4f %-12.4f\n",ep,s_tot/iters,v_tot/iters);
            free_keys(sks,pks,n); free(sks); free(pks);
        }
    }
    fclose(f_epoch);

    /* ─── 3. Benchmark Iterations Sweep (n=2, epoch=1) ─── */
    fprintf(f_bench,"bench_iters,sign_ms,verify_ms\n");
    printf("\n=== SWEEP 3: Benchmark Iterations ===\n");
    printf("%-14s %-12s %-12s\n","bench_iters","sign_ms","verify_ms");
    {
        int n=2;
        UserSK *sks=(UserSK*)calloc(n,sizeof(UserSK));
        UserPK *pks=(UserPK*)calloc(n,sizeof(UserPK));
        for(int i=0;i<n;i++) KeyGen(&sks[i],&pks[i]);
        for(int i=0;i<n;i++) KeyUp(&sks[i]);
        for(int bi = 0; bi < N_BENCH; bi++){
            int iters = BENCH_START + bi * BENCH_STEP;
            if(iters == 0){
            fprintf(f_bench,"0,0,0\n");
            printf("%-14d %-12.4f %-12.4f\n",
                   0,
                   0.0,
                   0.0);
            continue;
        }

        double s_tot = 0;
        double v_tot = 0;

        for(int it = 0; it < iters; it++){

            double ts = now_ms();
            RingSig *s = Sign(MSG,1,sks,pks,0,n);
            s_tot += now_ms() - ts;

            double tv = now_ms();
            Verify(MSG,s,pks,n);
            v_tot += now_ms() - tv;

            free_sig(s);
        }

        double avg_sign   = s_tot / iters;
        double avg_verify = v_tot / iters;

        fprintf(
            f_bench,
            "%d,%.6f,%.6f\n",
            iters,
            avg_sign,
            avg_verify
        );

        printf(
            "%-14d %-12.4f %-12.4f\n",
            iters,
            avg_sign,
            avg_verify
        );
    }
        free_keys(sks,pks,n); free(sks); free(pks);
    }
    fclose(f_bench);

    printf("\nDone! CSV files written:\n");
    printf("  results_ring_size_fsdr_pairing.csv\n");
    printf("  results_epochs_fsdr_pairing.csv\n");
    printf("  results_bench_iter_fsdr_pairing.csv\n");

    element_clear(G_g); element_clear(G_h); element_clear(G_Ft);
    for(int i=0;i<HVEC_LEN;i++) element_clear(G_hvec[i]);
    pairing_clear(G_pairing);
    return 0;
}
