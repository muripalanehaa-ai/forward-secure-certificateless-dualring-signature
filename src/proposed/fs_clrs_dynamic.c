// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdint.h>
// #include <time.h>
// #include <openssl/bn.h>
// #include <openssl/evp.h>
// #include <openssl/rand.h>

// /* ─── Configuration ──────────────────────────────────────────────────────── */
// #define MSG        "Hello, Forward-Secure Ring World!"
// #define HASH_LEN   32
// #define PRIME_BITS 256

// /* Dynamic sweep ranges */
// static const int RING_SIZES[]   = {2,4,8,16,32,64,128,256,512,1024};
// static const int N_RING         = 10;
// static const int EPOCHS[]       = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
// static const int N_EPOCHS       = 20;
// static const int BENCH_ITERS[]  = {100,200,300,400,500,600,700,800,900,1000,
//                                     1100,1200,1300,1400,1500,1600,1700,1800,1900,2000};
// static const int N_BENCH        = 20;

// /* ─── Globals ────────────────────────────────────────────────────────────── */
// static BIGNUM *G_p=NULL, *G_q=NULL, *G_g=NULL, *G_x=NULL, *G_y=NULL;
// static BN_CTX *G_ctx=NULL;

// /* ─── Timing ─────────────────────────────────────────────────────────────── */
// static double now_ms(void){
//     struct timespec ts;
//     clock_gettime(CLOCK_MONOTONIC,&ts);
//     return ts.tv_sec*1e3+ts.tv_nsec/1e6;
// }

// /* ─── Hash helpers ───────────────────────────────────────────────────────── */
// static BIGNUM *H_bytes(uint8_t tag,const uint8_t *data,size_t dlen){
//     uint8_t digest[HASH_LEN];
//     EVP_MD_CTX *mdctx=EVP_MD_CTX_new();
//     EVP_DigestInit_ex(mdctx,EVP_sha256(),NULL);
//     EVP_DigestUpdate(mdctx,&tag,1);
//     EVP_DigestUpdate(mdctx,data,dlen);
//     unsigned int outlen=HASH_LEN;
//     EVP_DigestFinal_ex(mdctx,digest,&outlen);
//     EVP_MD_CTX_free(mdctx);
//     BIGNUM *r=BN_bin2bn(digest,HASH_LEN,NULL);
//     BN_CTX *tmp=BN_CTX_new();
//     BN_mod(r,r,G_q,tmp);
//     BN_CTX_free(tmp);
//     return r;
// }
// static void bn_pack(const BIGNUM *a,uint8_t out[64]){
//     memset(out,0,64);
//     int nb=BN_num_bytes(a);
//     if(nb>64)nb=64;
//     BN_bn2bin(a,out+64-nb);
// }
// static BIGNUM *H1(int i,const BIGNUM *P0){
//     uint8_t buf[68];uint32_t idx=(uint32_t)i;
//     memcpy(buf,&idx,4);bn_pack(P0,buf+4);
//     return H_bytes(0x01,buf,68);
// }
// static BIGNUM *H2(int i,const BIGNUM *P0,const BIGNUM *P1){
//     uint8_t buf[132];uint32_t idx=(uint32_t)i;
//     memcpy(buf,&idx,4);bn_pack(P0,buf+4);bn_pack(P1,buf+68);
//     return H_bytes(0x02,buf,132);
// }
// static BIGNUM *H3(const char *m,int t,const BIGNUM **Phi,int n,const BIGNUM *Ut){
//     size_t mlen=strlen(m);
//     size_t blen=mlen+4+(size_t)n*64+64;
//     uint8_t *buf=(uint8_t*)malloc(blen);
//     size_t off=0;
//     memcpy(buf+off,m,mlen);off+=mlen;
//     uint32_t ep=(uint32_t)t;
//     memcpy(buf+off,&ep,4);off+=4;
//     for(int i=0;i<n;i++){bn_pack(Phi[i],buf+off);off+=64;}
//     bn_pack(Ut,buf+off);off+=64;
//     BIGNUM *r=H_bytes(0x03,buf,off);
//     free(buf);return r;
// }
// static BIGNUM *Hevo(const BIGNUM *zprev,const BIGNUM *rho){
//     uint8_t buf[128];bn_pack(zprev,buf);bn_pack(rho,buf+64);
//     return H_bytes(0x04,buf,128);
// }
// static BIGNUM *Hpi(int i,const BIGNUM *R,const BIGNUM *w){
//     uint8_t buf[132];uint32_t idx=(uint32_t)i;
//     memcpy(buf,&idx,4);bn_pack(R,buf+4);bn_pack(w,buf+68);
//     return H_bytes(0x05,buf,132);
// }

// /* ─── Group ops ──────────────────────────────────────────────────────────── */
// static BIGNUM *gexp(const BIGNUM *base,const BIGNUM *exp){
//     BIGNUM *r=BN_new();
//     BN_mod_exp(r,base,exp,G_p,G_ctx);
//     return r;
// }
// static BIGNUM *gmul(const BIGNUM *a,const BIGNUM *b){
//     BIGNUM *r=BN_new();
//     BN_mod_mul(r,a,b,G_p,G_ctx);
//     return r;
// }

// /* ─── Data structures ────────────────────────────────────────────────────── */
// typedef struct{BIGNUM *d0,*z,*rho;}           UserSK;
// typedef struct{BIGNUM *P0,*P1,*d1,*mu,*R,*epi,*alpha;int epoch;} UserPK;
// typedef struct{BIGNUM *Phi;}                   CombPK;
// typedef struct{BIGNUM **c;BIGNUM *zout;int epoch,n;} Sig;

// /* ─── Setup ──────────────────────────────────────────────────────────────── */
// static void Setup(void){
//     if(G_ctx){BN_CTX_free(G_ctx);}
//     if(G_p){BN_free(G_p);}if(G_q){BN_free(G_q);}
//     if(G_g){BN_free(G_g);}if(G_x){BN_free(G_x);}if(G_y){BN_free(G_y);}
//     G_ctx=BN_CTX_new();
//     G_p=BN_new();G_q=BN_new();G_g=BN_new();G_x=BN_new();G_y=BN_new();
//     BN_generate_prime_ex(G_p,PRIME_BITS,1,NULL,NULL,NULL);
//     BIGNUM *pm1=BN_dup(G_p);BN_sub_word(pm1,1);BN_rshift1(G_q,pm1);BN_free(pm1);
//     BIGNUM *h=BN_new(),*two=BN_new(),*one=BN_new(),*check=BN_new();
//     BN_set_word(two,2);BN_one(one);
//     do{
//         BN_rand_range(h,G_p);
//         BN_mod_exp(G_g,h,two,G_p,G_ctx);
//         BN_mod_exp(check,G_g,G_q,G_p,G_ctx);
//     }while(BN_is_one(G_g)||BN_cmp(check,one)!=0);
//     BN_free(h);BN_free(two);BN_free(one);BN_free(check);
//     BN_rand_range(G_x,G_q);
//     BN_mod_exp(G_y,G_g,G_x,G_p,G_ctx);
// }

// /* ─── PKG ────────────────────────────────────────────────────────────────── */
// static void PKG(int i,UserSK *sk,UserPK *pk){
//     BIGNUM *S0=BN_new(),*S1=BN_new();
//     BN_rand_range(S0,G_q);BN_rand_range(S1,G_q);
//     pk->P0=gexp(G_g,S0);pk->P1=gexp(G_g,S1);
//     BIGNUM *h1=H1(i,pk->P0),*h2=H2(i,pk->P0,pk->P1);
//     sk->d0=BN_new();
//     BIGNUM *tmp=BN_new();
//     BN_mod_mul(tmp,G_x,h1,G_q,G_ctx);BN_mod_add(sk->d0,S0,tmp,G_q,G_ctx);
//     pk->d1=BN_new();
//     BN_mod_mul(tmp,G_x,h2,G_q,G_ctx);BN_mod_add(pk->d1,S1,tmp,G_q,G_ctx);
//     sk->z=sk->rho=NULL;pk->mu=pk->R=pk->epi=pk->alpha=NULL;
//     BN_free(S0);BN_free(S1);BN_free(h1);BN_free(h2);BN_free(tmp);
// }

// /* ─── UKG ────────────────────────────────────────────────────────────────── */
// static void UKG(int i,UserSK *sk,UserPK *pk,CombPK *cpk,int t){
//     BIGNUM *h2=H2(i,pk->P0,pk->P1);
//     BIGNUM *lhs=gexp(G_g,pk->d1),*yh2=gexp(G_y,h2),*rhs=gmul(pk->P1,yh2);
//     if(BN_cmp(lhs,rhs)!=0){fprintf(stderr,"KGC honesty FAIL\n");exit(1);}
//     BN_free(h2);BN_free(lhs);BN_free(yh2);BN_free(rhs);
//     if(t==1){
//         if(sk->z)BN_free(sk->z);
//         sk->z=BN_new();BN_rand_range(sk->z,G_q);
//     }else{
//         BIGNUM *rho_new=BN_new();BN_rand_range(rho_new,G_q);
//         BIGNUM *z_new=Hevo(sk->z,rho_new);
//         BN_free(sk->z);if(sk->rho)BN_free(sk->rho);
//         sk->z=z_new;sk->rho=rho_new;
//     }
//     if(sk->rho&&t==1){BN_free(sk->rho);sk->rho=NULL;}
//     if(!sk->rho){sk->rho=BN_new();BN_rand_range(sk->rho,G_q);}
//     if(pk->mu)BN_free(pk->mu);if(pk->R)BN_free(pk->R);
//     pk->mu=gexp(G_g,sk->z);pk->R=gexp(G_g,sk->rho);
//     BIGNUM *k=BN_new();BN_rand_range(k,G_q);
//     BIGNUM *w=gexp(G_g,k);
//     BIGNUM *ep=Hpi(i,pk->R,w);
//     BIGNUM *al=BN_new(),*erho=BN_new();
//     BN_mod_mul(erho,ep,sk->rho,G_q,G_ctx);
//     BN_mod_sub(al,k,erho,G_q,G_ctx);
//     if(pk->epi)BN_free(pk->epi);if(pk->alpha)BN_free(pk->alpha);
//     pk->epi=ep;pk->alpha=al;pk->epoch=t;
//     BN_free(k);BN_free(w);BN_free(erho);
//     BIGNUM *h1=H1(i,pk->P0),*yh1=gexp(G_y,h1),*t1=gmul(pk->P0,yh1);
//     if(cpk->Phi)BN_free(cpk->Phi);
//     cpk->Phi=gmul(t1,pk->mu);
//     BN_free(h1);BN_free(yh1);BN_free(t1);
//     BIGNUM *tau=BN_new();BN_mod_add(tau,sk->d0,sk->z,G_q,G_ctx);
//     BIGNUM *gtau=gexp(G_g,tau);
//     if(BN_cmp(gtau,cpk->Phi)!=0){fprintf(stderr,"Phi FAIL\n");exit(1);}
//     BN_free(tau);BN_free(gtau);
// }

// /* ─── Sign ───────────────────────────────────────────────────────────────── */
// static Sig *Sign(const char *m,int t,CombPK *cpks,UserPK *pks,UserSK *sks,int j,int n){
//     Sig *sig=(Sig*)malloc(sizeof(Sig));
//     sig->n=n;sig->epoch=t;
//     sig->c=(BIGNUM**)malloc(n*sizeof(BIGNUM*));
//     BIGNUM *tau=BN_new();
//     BN_mod_add(tau,sks[j].d0,sks[j].z,G_q,G_ctx);
//     BIGNUM *rt=BN_new();BN_rand_range(rt,G_q);
//     BIGNUM *Uj=gexp(G_g,rt);
//     BIGNUM *Ut=BN_dup(Uj);
//     for(int i=0;i<n;i++){
//         if(i==j){sig->c[i]=NULL;continue;}
//         sig->c[i]=BN_new();BN_rand_range(sig->c[i],G_q);
//         BIGNUM *Ui=gexp(cpks[i].Phi,sig->c[i]);
//         BIGNUM *tmp=gmul(Ut,Ui);BN_free(Ut);Ut=tmp;BN_free(Ui);
//     }
//     const BIGNUM **Phis=(const BIGNUM**)malloc(n*sizeof(BIGNUM*));
//     for(int i=0;i<n;i++)Phis[i]=cpks[i].Phi;
//     BIGNUM *Ct=H3(m,t,Phis,n,Ut);free(Phis);
//     BIGNUM *Cj=BN_dup(Ct);
//     for(int i=0;i<n;i++){if(i==j)continue;BN_mod_sub(Cj,Cj,sig->c[i],G_q,G_ctx);}
//     sig->c[j]=Cj;
//     BIGNUM *Cjtau=BN_new();BN_mod_mul(Cjtau,Cj,tau,G_q,G_ctx);
//     sig->zout=BN_new();BN_mod_sub(sig->zout,rt,Cjtau,G_q,G_ctx);
//     BN_free(tau);BN_free(rt);BN_free(Uj);BN_free(Ut);BN_free(Ct);BN_free(Cjtau);
//     return sig;
// }

// /* ─── Verify ─────────────────────────────────────────────────────────────── */
// static int Verify(const char *m,const Sig *sig,CombPK *cpks,UserPK *pks,int n){
//     BIGNUM *Utp=gexp(G_g,sig->zout);
//     for(int i=0;i<n;i++){
//         BIGNUM *phi_ci=gexp(cpks[i].Phi,sig->c[i]);
//         BIGNUM *tmp=gmul(Utp,phi_ci);BN_free(Utp);Utp=tmp;BN_free(phi_ci);
//     }
//     const BIGNUM **Phis=(const BIGNUM**)malloc(n*sizeof(BIGNUM*));
//     for(int i=0;i<n;i++)Phis[i]=cpks[i].Phi;
//     BIGNUM *Cstar=H3(m,sig->epoch,Phis,n,Utp);
//     free(Phis);BN_free(Utp);
//     BIGNUM *sumc=BN_new();BN_zero(sumc);
//     for(int i=0;i<n;i++)BN_mod_add(sumc,sumc,sig->c[i],G_q,G_ctx);
//     int ok=(BN_cmp(Cstar,sumc)==0);
//     BN_free(Cstar);BN_free(sumc);
//     if(!ok)return 0;
//     for(int i=0;i<n;i++){
//         BIGNUM *ga=gexp(G_g,pks[i].alpha);
//         BIGNUM *Re=gexp(pks[i].R,pks[i].epi);
//         BIGNUM *wp=gmul(ga,Re);
//         BIGNUM *ep2=Hpi(i,pks[i].R,wp);
//         int pok=(BN_cmp(ep2,pks[i].epi)==0);
//         BN_free(ga);BN_free(Re);BN_free(wp);BN_free(ep2);
//         if(!pok)return 0;
//     }
//     return 1;
// }

// static void free_sig(Sig *s){
//     for(int i=0;i<s->n;i++)BN_free(s->c[i]);
//     free(s->c);BN_free(s->zout);free(s);
// }
// static void free_user(UserSK *sk,UserPK *pk,int n){
//     for(int i=0;i<n;i++){
//         BN_free(sk[i].d0);if(sk[i].z)BN_free(sk[i].z);if(sk[i].rho)BN_free(sk[i].rho);
//         BN_free(pk[i].P0);BN_free(pk[i].P1);BN_free(pk[i].d1);
//         if(pk[i].mu)BN_free(pk[i].mu);if(pk[i].R)BN_free(pk[i].R);
//         if(pk[i].epi)BN_free(pk[i].epi);if(pk[i].alpha)BN_free(pk[i].alpha);
//     }
// }

// /* ══════════════════════════════════════════════════════════════════════════
//    MAIN
//    ══════════════════════════════════════════════════════════════════════════ */
// int main(void){
//     printf("Forward-Secure Certificateless Ring Signature — Dynamic Benchmark\n");
//     printf("Generating %d-bit safe prime once...\n",PRIME_BITS);
//     double t0=now_ms();
//     Setup();
//     double t_setup=now_ms()-t0;
//     printf("Setup done in %.3f ms\n\n",t_setup);

//     FILE *f_ring   = fopen("results_ring_size.csv","w");
//     FILE *f_epoch  = fopen("results_epochs.csv","w");
//     FILE *f_bench  = fopen("results_bench_iter.csv","w");

//     /* ── 1. Ring size sweep (epoch=1, bench=100) ── */
//     fprintf(f_ring,"ring_size,sign_ms,verify_ms,sig_bytes\n");
//     printf("=== SWEEP 1: Ring Size ===\n");
//     printf("%-12s %-12s %-12s %-12s\n","ring_size","sign_ms","verify_ms","sig_bytes");

//     for(int ri=0;ri<N_RING;ri++){
//         int n=RING_SIZES[ri];
//         UserSK *sks=(UserSK*)calloc(n,sizeof(UserSK));
//         UserPK *pks=(UserPK*)calloc(n,sizeof(UserPK));
//         CombPK *cpks=(CombPK*)calloc(n,sizeof(CombPK));
//         for(int i=0;i<n;i++){PKG(i,&sks[i],&pks[i]);cpks[i].Phi=NULL;}
//         for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],1);

//         double s_tot=0,v_tot=0;
//         int iters=100;
//         for(int it=0;it<iters;it++){
//             for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],1);
//             double ts=now_ms();
//             Sig *s=Sign(MSG,1,cpks,pks,sks,0,n);
//             s_tot+=now_ms()-ts;
//             double tv=now_ms();
//             Verify(MSG,s,cpks,pks,n);
//             v_tot+=now_ms()-tv;
//             free_sig(s);
//         }
//         double avg_s=s_tot/iters, avg_v=v_tot/iters;
//         int qbytes=(BN_num_bits(G_q)+7)/8;
//         size_t sig_bytes=(size_t)(n+1)*qbytes+4;
//         fprintf(f_ring,"%d,%.6f,%.6f,%zu\n",n,avg_s,avg_v,sig_bytes);
//         printf("%-12d %-12.4f %-12.4f %-12zu\n",n,avg_s,avg_v,sig_bytes);
//         free_user(sks,pks,n);
//         for(int i=0;i<n;i++)if(cpks[i].Phi)BN_free(cpks[i].Phi);
//         free(sks);free(pks);free(cpks);
//     }
//     fclose(f_ring);

//     /* ── 2. Epoch sweep (ring=4, bench=50) ── */
//     fprintf(f_epoch,"epoch,sign_ms,verify_ms\n");
//     printf("\n=== SWEEP 2: Epoch Count ===\n");
//     printf("%-12s %-12s %-12s\n","epoch","sign_ms","verify_ms");
//     {
//         int n=4;
//         for(int ei=0;ei<N_EPOCHS;ei++){
//             int ep=EPOCHS[ei];
//             UserSK *sks=(UserSK*)calloc(n,sizeof(UserSK));
//             UserPK *pks=(UserPK*)calloc(n,sizeof(UserPK));
//             CombPK *cpks=(CombPK*)calloc(n,sizeof(CombPK));
//             for(int i=0;i<n;i++){PKG(i,&sks[i],&pks[i]);cpks[i].Phi=NULL;}
//             /* Evolve to epoch ep */
//             for(int e=1;e<=ep;e++)
//                 for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],e);

//             double s_tot=0,v_tot=0;
//             int iters=50;
//             for(int it=0;it<iters;it++){
//                 double ts=now_ms();
//                 Sig *s=Sign(MSG,ep,cpks,pks,sks,0,n);
//                 s_tot+=now_ms()-ts;
//                 double tv=now_ms();
//                 Verify(MSG,s,cpks,pks,n);
//                 v_tot+=now_ms()-tv;
//                 free_sig(s);
//             }
//             double avg_s=s_tot/iters,avg_v=v_tot/iters;
//             fprintf(f_epoch,"%d,%.6f,%.6f\n",ep,avg_s,avg_v);
//             printf("%-12d %-12.4f %-12.4f\n",ep,avg_s,avg_v);
//             free_user(sks,pks,n);
//             for(int i=0;i<n;i++)if(cpks[i].Phi)BN_free(cpks[i].Phi);
//             free(sks);free(pks);free(cpks);
//         }
//     }
//     fclose(f_epoch);

//     /* ── 3. Benchmark iterations sweep (ring=2, epoch=1) ── */
//     fprintf(f_bench,"bench_iters,sign_ms,verify_ms\n");
//     printf("\n=== SWEEP 3: Benchmark Iterations ===\n");
//     printf("%-14s %-12s %-12s\n","bench_iters","sign_ms","verify_ms");
//     {
//         int n=2;
//         UserSK *sks=(UserSK*)calloc(n,sizeof(UserSK));
//         UserPK *pks=(UserPK*)calloc(n,sizeof(UserPK));
//         CombPK *cpks=(CombPK*)calloc(n,sizeof(CombPK));
//         for(int i=0;i<n;i++){PKG(i,&sks[i],&pks[i]);cpks[i].Phi=NULL;}
//         for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],1);

//         for(int bi=0;bi<N_BENCH;bi++){
//             int iters=BENCH_ITERS[bi];
//             double s_tot=0,v_tot=0;
//             for(int it=0;it<iters;it++){
//                 for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],1);
//                 double ts=now_ms();
//                 Sig *s=Sign(MSG,1,cpks,pks,sks,0,n);
//                 s_tot+=now_ms()-ts;
//                 double tv=now_ms();
//                 Verify(MSG,s,cpks,pks,n);
//                 v_tot+=now_ms()-tv;
//                 free_sig(s);
//             }
//             double avg_s=s_tot/iters,avg_v=v_tot/iters;
//             fprintf(f_bench,"%d,%.6f,%.6f\n",iters,avg_s,avg_v);
//             printf("%-14d %-12.4f %-12.4f\n",iters,avg_s,avg_v);
//         }
//         free_user(sks,pks,n);
//         for(int i=0;i<n;i++)if(cpks[i].Phi)BN_free(cpks[i].Phi);
//         free(sks);free(pks);free(cpks);
//     }
//     fclose(f_bench);

//     printf("\nDone! CSV files written:\n");
//     printf("  results_ring_size.csv\n");
//     printf("  results_epochs.csv\n");
//     printf("  results_bench_iter.csv\n");

//     BN_free(G_p);BN_free(G_q);BN_free(G_g);BN_free(G_x);BN_free(G_y);
//     BN_CTX_free(G_ctx);
//     return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

/* ─── Configuration ──────────────────────────────────────────────────────── */
#define MSG        "Hello, Forward-Secure Ring World!"
#define HASH_LEN   32
#define PRIME_BITS 256

/* Dynamic sweep ranges */
static const int RING_SIZES[]   = {2,4,8,16,32,64,128,256,512,1024};
static const int N_RING         = 10;
static const int EPOCHS[]       = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
static const int N_EPOCHS       = 20;

/*
 * BENCH_ITERS: 5, 10, 15, ..., 200  (step=5, starts at 5 to avoid /0)
 */
static const int BENCH_ITERS[]  = {
      5, 10, 15, 20, 25, 30, 35, 40,
     45, 50, 55, 60, 65, 70, 75, 80,
     85, 90, 95,100,105,110,115,120,
    125,130,135,140,145,150,155,160,
    165,170,175,180,185,190,195,200
};
static const int N_BENCH        = 40;   /* 40 points: 5..200 step 5 */

/* ─── Globals ────────────────────────────────────────────────────────────── */
static BIGNUM *G_p=NULL, *G_q=NULL, *G_g=NULL, *G_x=NULL, *G_y=NULL;
static BN_CTX *G_ctx=NULL;

/* ─── Timing ─────────────────────────────────────────────────────────────── */
static double now_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC,&ts);
    return ts.tv_sec*1e3+ts.tv_nsec/1e6;
}

/* ─── Hash helpers ───────────────────────────────────────────────────────── */
static BIGNUM *H_bytes(uint8_t tag,const uint8_t *data,size_t dlen){
    uint8_t digest[HASH_LEN];
    EVP_MD_CTX *mdctx=EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx,EVP_sha256(),NULL);
    EVP_DigestUpdate(mdctx,&tag,1);
    EVP_DigestUpdate(mdctx,data,dlen);
    unsigned int outlen=HASH_LEN;
    EVP_DigestFinal_ex(mdctx,digest,&outlen);
    EVP_MD_CTX_free(mdctx);
    BIGNUM *r=BN_bin2bn(digest,HASH_LEN,NULL);
    BN_CTX *tmp=BN_CTX_new();
    BN_mod(r,r,G_q,tmp);
    BN_CTX_free(tmp);
    return r;
}
static void bn_pack(const BIGNUM *a,uint8_t out[64]){
    memset(out,0,64);
    int nb=BN_num_bytes(a);
    if(nb>64)nb=64;
    BN_bn2bin(a,out+64-nb);
}
static BIGNUM *H1(int i,const BIGNUM *P0){
    uint8_t buf[68];uint32_t idx=(uint32_t)i;
    memcpy(buf,&idx,4);bn_pack(P0,buf+4);
    return H_bytes(0x01,buf,68);
}
static BIGNUM *H2(int i,const BIGNUM *P0,const BIGNUM *P1){
    uint8_t buf[132];uint32_t idx=(uint32_t)i;
    memcpy(buf,&idx,4);bn_pack(P0,buf+4);bn_pack(P1,buf+68);
    return H_bytes(0x02,buf,132);
}
static BIGNUM *H3(const char *m,int t,const BIGNUM **Phi,int n,const BIGNUM *Ut){
    size_t mlen=strlen(m);
    size_t blen=mlen+4+(size_t)n*64+64;
    uint8_t *buf=(uint8_t*)malloc(blen);
    size_t off=0;
    memcpy(buf+off,m,mlen);off+=mlen;
    uint32_t ep=(uint32_t)t;
    memcpy(buf+off,&ep,4);off+=4;
    for(int i=0;i<n;i++){bn_pack(Phi[i],buf+off);off+=64;}
    bn_pack(Ut,buf+off);off+=64;
    BIGNUM *r=H_bytes(0x03,buf,off);
    free(buf);return r;
}
static BIGNUM *Hevo(const BIGNUM *zprev,const BIGNUM *rho){
    uint8_t buf[128];bn_pack(zprev,buf);bn_pack(rho,buf+64);
    return H_bytes(0x04,buf,128);
}
static BIGNUM *Hpi(int i,const BIGNUM *R,const BIGNUM *w){
    uint8_t buf[132];uint32_t idx=(uint32_t)i;
    memcpy(buf,&idx,4);bn_pack(R,buf+4);bn_pack(w,buf+68);
    return H_bytes(0x05,buf,132);
}

/* ─── Group ops ──────────────────────────────────────────────────────────── */
static BIGNUM *gexp(const BIGNUM *base,const BIGNUM *exp){
    BIGNUM *r=BN_new();
    BN_mod_exp(r,base,exp,G_p,G_ctx);
    return r;
}
static BIGNUM *gmul(const BIGNUM *a,const BIGNUM *b){
    BIGNUM *r=BN_new();
    BN_mod_mul(r,a,b,G_p,G_ctx);
    return r;
}

/* ─── Data structures ────────────────────────────────────────────────────── */
typedef struct{BIGNUM *d0,*z,*rho;}           UserSK;
typedef struct{BIGNUM *P0,*P1,*d1,*mu,*R,*epi,*alpha;int epoch;} UserPK;
typedef struct{BIGNUM *Phi;}                   CombPK;
typedef struct{BIGNUM **c;BIGNUM *zout;int epoch,n;} Sig;

/* ─── Setup ──────────────────────────────────────────────────────────────── */
static void Setup(void){
    if(G_ctx){BN_CTX_free(G_ctx);}
    if(G_p){BN_free(G_p);}if(G_q){BN_free(G_q);}
    if(G_g){BN_free(G_g);}if(G_x){BN_free(G_x);}if(G_y){BN_free(G_y);}
    G_ctx=BN_CTX_new();
    G_p=BN_new();G_q=BN_new();G_g=BN_new();G_x=BN_new();G_y=BN_new();
    BN_generate_prime_ex(G_p,PRIME_BITS,1,NULL,NULL,NULL);
    BIGNUM *pm1=BN_dup(G_p);BN_sub_word(pm1,1);BN_rshift1(G_q,pm1);BN_free(pm1);
    BIGNUM *h=BN_new(),*two=BN_new(),*one=BN_new(),*check=BN_new();
    BN_set_word(two,2);BN_one(one);
    do{
        BN_rand_range(h,G_p);
        BN_mod_exp(G_g,h,two,G_p,G_ctx);
        BN_mod_exp(check,G_g,G_q,G_p,G_ctx);
    }while(BN_is_one(G_g)||BN_cmp(check,one)!=0);
    BN_free(h);BN_free(two);BN_free(one);BN_free(check);
    BN_rand_range(G_x,G_q);
    BN_mod_exp(G_y,G_g,G_x,G_p,G_ctx);
}

/* ─── PKG ────────────────────────────────────────────────────────────────── */
static void PKG(int i,UserSK *sk,UserPK *pk){
    BIGNUM *S0=BN_new(),*S1=BN_new();
    BN_rand_range(S0,G_q);BN_rand_range(S1,G_q);
    pk->P0=gexp(G_g,S0);pk->P1=gexp(G_g,S1);
    BIGNUM *h1=H1(i,pk->P0),*h2=H2(i,pk->P0,pk->P1);
    sk->d0=BN_new();
    BIGNUM *tmp=BN_new();
    BN_mod_mul(tmp,G_x,h1,G_q,G_ctx);BN_mod_add(sk->d0,S0,tmp,G_q,G_ctx);
    pk->d1=BN_new();
    BN_mod_mul(tmp,G_x,h2,G_q,G_ctx);BN_mod_add(pk->d1,S1,tmp,G_q,G_ctx);
    sk->z=sk->rho=NULL;pk->mu=pk->R=pk->epi=pk->alpha=NULL;
    BN_free(S0);BN_free(S1);BN_free(h1);BN_free(h2);BN_free(tmp);
}

/* ─── UKG ────────────────────────────────────────────────────────────────── */
static void UKG(int i,UserSK *sk,UserPK *pk,CombPK *cpk,int t){
    BIGNUM *h2=H2(i,pk->P0,pk->P1);
    BIGNUM *lhs=gexp(G_g,pk->d1),*yh2=gexp(G_y,h2),*rhs=gmul(pk->P1,yh2);
    if(BN_cmp(lhs,rhs)!=0){fprintf(stderr,"KGC honesty FAIL\n");exit(1);}
    BN_free(h2);BN_free(lhs);BN_free(yh2);BN_free(rhs);
    if(t==1){
        if(sk->z)BN_free(sk->z);
        sk->z=BN_new();BN_rand_range(sk->z,G_q);
    }else{
        BIGNUM *rho_new=BN_new();BN_rand_range(rho_new,G_q);
        BIGNUM *z_new=Hevo(sk->z,rho_new);
        BN_free(sk->z);if(sk->rho)BN_free(sk->rho);
        sk->z=z_new;sk->rho=rho_new;
    }
    if(sk->rho&&t==1){BN_free(sk->rho);sk->rho=NULL;}
    if(!sk->rho){sk->rho=BN_new();BN_rand_range(sk->rho,G_q);}
    if(pk->mu)BN_free(pk->mu);if(pk->R)BN_free(pk->R);
    pk->mu=gexp(G_g,sk->z);pk->R=gexp(G_g,sk->rho);
    BIGNUM *k=BN_new();BN_rand_range(k,G_q);
    BIGNUM *w=gexp(G_g,k);
    BIGNUM *ep=Hpi(i,pk->R,w);
    BIGNUM *al=BN_new(),*erho=BN_new();
    BN_mod_mul(erho,ep,sk->rho,G_q,G_ctx);
    BN_mod_sub(al,k,erho,G_q,G_ctx);
    if(pk->epi)BN_free(pk->epi);if(pk->alpha)BN_free(pk->alpha);
    pk->epi=ep;pk->alpha=al;pk->epoch=t;
    BN_free(k);BN_free(w);BN_free(erho);
    BIGNUM *h1=H1(i,pk->P0),*yh1=gexp(G_y,h1),*t1=gmul(pk->P0,yh1);
    if(cpk->Phi)BN_free(cpk->Phi);
    cpk->Phi=gmul(t1,pk->mu);
    BN_free(h1);BN_free(yh1);BN_free(t1);
    BIGNUM *tau=BN_new();BN_mod_add(tau,sk->d0,sk->z,G_q,G_ctx);
    BIGNUM *gtau=gexp(G_g,tau);
    if(BN_cmp(gtau,cpk->Phi)!=0){fprintf(stderr,"Phi FAIL\n");exit(1);}
    BN_free(tau);BN_free(gtau);
}

/* ─── Sign ───────────────────────────────────────────────────────────────── */
static Sig *Sign(const char *m,int t,CombPK *cpks,UserPK *pks,UserSK *sks,int j,int n){
    Sig *sig=(Sig*)malloc(sizeof(Sig));
    sig->n=n;sig->epoch=t;
    sig->c=(BIGNUM**)malloc(n*sizeof(BIGNUM*));
    BIGNUM *tau=BN_new();
    BN_mod_add(tau,sks[j].d0,sks[j].z,G_q,G_ctx);
    BIGNUM *rt=BN_new();BN_rand_range(rt,G_q);
    BIGNUM *Uj=gexp(G_g,rt);
    BIGNUM *Ut=BN_dup(Uj);
    for(int i=0;i<n;i++){
        if(i==j){sig->c[i]=NULL;continue;}
        sig->c[i]=BN_new();BN_rand_range(sig->c[i],G_q);
        BIGNUM *Ui=gexp(cpks[i].Phi,sig->c[i]);
        BIGNUM *tmp=gmul(Ut,Ui);BN_free(Ut);Ut=tmp;BN_free(Ui);
    }
    const BIGNUM **Phis=(const BIGNUM**)malloc(n*sizeof(BIGNUM*));
    for(int i=0;i<n;i++)Phis[i]=cpks[i].Phi;
    BIGNUM *Ct=H3(m,t,Phis,n,Ut);free(Phis);
    BIGNUM *Cj=BN_dup(Ct);
    for(int i=0;i<n;i++){if(i==j)continue;BN_mod_sub(Cj,Cj,sig->c[i],G_q,G_ctx);}
    sig->c[j]=Cj;
    BIGNUM *Cjtau=BN_new();BN_mod_mul(Cjtau,Cj,tau,G_q,G_ctx);
    sig->zout=BN_new();BN_mod_sub(sig->zout,rt,Cjtau,G_q,G_ctx);
    BN_free(tau);BN_free(rt);BN_free(Uj);BN_free(Ut);BN_free(Ct);BN_free(Cjtau);
    return sig;
}

/* ─── Verify ─────────────────────────────────────────────────────────────── */
static int Verify(const char *m,const Sig *sig,CombPK *cpks,UserPK *pks,int n){
    BIGNUM *Utp=gexp(G_g,sig->zout);
    for(int i=0;i<n;i++){
        BIGNUM *phi_ci=gexp(cpks[i].Phi,sig->c[i]);
        BIGNUM *tmp=gmul(Utp,phi_ci);BN_free(Utp);Utp=tmp;BN_free(phi_ci);
    }
    const BIGNUM **Phis=(const BIGNUM**)malloc(n*sizeof(BIGNUM*));
    for(int i=0;i<n;i++)Phis[i]=cpks[i].Phi;
    BIGNUM *Cstar=H3(m,sig->epoch,Phis,n,Utp);
    free(Phis);BN_free(Utp);
    BIGNUM *sumc=BN_new();BN_zero(sumc);
    for(int i=0;i<n;i++)BN_mod_add(sumc,sumc,sig->c[i],G_q,G_ctx);
    int ok=(BN_cmp(Cstar,sumc)==0);
    BN_free(Cstar);BN_free(sumc);
    if(!ok)return 0;
    for(int i=0;i<n;i++){
        BIGNUM *ga=gexp(G_g,pks[i].alpha);
        BIGNUM *Re=gexp(pks[i].R,pks[i].epi);
        BIGNUM *wp=gmul(ga,Re);
        BIGNUM *ep2=Hpi(i,pks[i].R,wp);
        int pok=(BN_cmp(ep2,pks[i].epi)==0);
        BN_free(ga);BN_free(Re);BN_free(wp);BN_free(ep2);
        if(!pok)return 0;
    }
    return 1;
}

static void free_sig(Sig *s){
    for(int i=0;i<s->n;i++)BN_free(s->c[i]);
    free(s->c);BN_free(s->zout);free(s);
}
static void free_user(UserSK *sk,UserPK *pk,int n){
    for(int i=0;i<n;i++){
        BN_free(sk[i].d0);if(sk[i].z)BN_free(sk[i].z);if(sk[i].rho)BN_free(sk[i].rho);
        BN_free(pk[i].P0);BN_free(pk[i].P1);BN_free(pk[i].d1);
        if(pk[i].mu)BN_free(pk[i].mu);if(pk[i].R)BN_free(pk[i].R);
        if(pk[i].epi)BN_free(pk[i].epi);if(pk[i].alpha)BN_free(pk[i].alpha);
    }
}

/* ══════════════════════════════════════════════════════════════════════════
   MAIN
   ══════════════════════════════════════════════════════════════════════════ */
int main(void){
    printf("Forward-Secure Certificateless Ring Signature — Dynamic Benchmark\n");
    printf("Generating %d-bit safe prime once...\n",PRIME_BITS);
    double t0=now_ms();
    Setup();
    printf("Setup done in %.3f ms\n\n",now_ms()-t0);

    FILE *f_ring   = fopen("results_ring_size_dynamic.csv","w");
    FILE *f_epoch  = fopen("results_epochs_dynamic.csv","w");
    FILE *f_bench  = fopen("results_bench_iter_dynamic.csv","w");

    /* ── 1. Ring size sweep ── */
    fprintf(f_ring,"ring_size,sign_ms,verify_ms,sig_bytes\n");
    printf("=== SWEEP 1: Ring Size ===\n");
    printf("%-12s %-12s %-12s %-12s\n","ring_size","sign_ms","verify_ms","sig_bytes");
    for(int ri=0;ri<N_RING;ri++){
        int n=RING_SIZES[ri];
        UserSK *sks=(UserSK*)calloc(n,sizeof(UserSK));
        UserPK *pks=(UserPK*)calloc(n,sizeof(UserPK));
        CombPK *cpks=(CombPK*)calloc(n,sizeof(CombPK));
        for(int i=0;i<n;i++){PKG(i,&sks[i],&pks[i]);cpks[i].Phi=NULL;}
        for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],1);
        double s_tot=0,v_tot=0;
        int iters=100;
        for(int it=0;it<iters;it++){
            for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],1);
            double ts=now_ms();
            Sig *s=Sign(MSG,1,cpks,pks,sks,0,n);
            s_tot+=now_ms()-ts;
            double tv=now_ms();
            Verify(MSG,s,cpks,pks,n);
            v_tot+=now_ms()-tv;
            free_sig(s);
        }
        double avg_s=s_tot/iters,avg_v=v_tot/iters;
        int qbytes=(BN_num_bits(G_q)+7)/8;
        size_t sig_bytes=(size_t)(n+1)*qbytes+4;
        fprintf(f_ring,"%d,%.6f,%.6f,%zu\n",n,avg_s,avg_v,sig_bytes);
        printf("%-12d %-12.4f %-12.4f %-12zu\n",n,avg_s,avg_v,sig_bytes);
        free_user(sks,pks,n);
        for(int i=0;i<n;i++)if(cpks[i].Phi)BN_free(cpks[i].Phi);
        free(sks);free(pks);free(cpks);
    }
    fclose(f_ring);

    /* ── 2. Epoch sweep ── */
    fprintf(f_epoch,"epoch,sign_ms,verify_ms\n");
    printf("\n=== SWEEP 2: Epoch Count ===\n");
    printf("%-12s %-12s %-12s\n","epoch","sign_ms","verify_ms");
    {
        int n=4;
        for(int ei=0;ei<N_EPOCHS;ei++){
            int ep=EPOCHS[ei];
            UserSK *sks=(UserSK*)calloc(n,sizeof(UserSK));
            UserPK *pks=(UserPK*)calloc(n,sizeof(UserPK));
            CombPK *cpks=(CombPK*)calloc(n,sizeof(CombPK));
            for(int i=0;i<n;i++){PKG(i,&sks[i],&pks[i]);cpks[i].Phi=NULL;}
            for(int e=1;e<=ep;e++)
                for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],e);
            double s_tot=0,v_tot=0;
            int iters=50;
            for(int it=0;it<iters;it++){
                double ts=now_ms();
                Sig *s=Sign(MSG,ep,cpks,pks,sks,0,n);
                s_tot+=now_ms()-ts;
                double tv=now_ms();
                Verify(MSG,s,cpks,pks,n);
                v_tot+=now_ms()-tv;
                free_sig(s);
            }
            fprintf(f_epoch,"%d,%.6f,%.6f\n",ep,s_tot/iters,v_tot/iters);
            printf("%-12d %-12.4f %-12.4f\n",ep,s_tot/iters,v_tot/iters);
            free_user(sks,pks,n);
            for(int i=0;i<n;i++)if(cpks[i].Phi)BN_free(cpks[i].Phi);
            free(sks);free(pks);free(cpks);
        }
    }
    fclose(f_epoch);

    /* ── 3. Benchmark iterations sweep: 5 to 200 step 5 (ring=2, epoch=1) ── */
    fprintf(f_bench,"bench_iters,sign_ms,verify_ms\n");
    printf("\n=== SWEEP 3: Benchmark Iterations (5 to 200, step 5) ===\n");
    printf("%-14s %-12s %-12s\n","bench_iters","sign_ms","verify_ms");
    {
        int n=2;
        UserSK *sks=(UserSK*)calloc(n,sizeof(UserSK));
        UserPK *pks=(UserPK*)calloc(n,sizeof(UserPK));
        CombPK *cpks=(CombPK*)calloc(n,sizeof(CombPK));
        for(int i=0;i<n;i++){PKG(i,&sks[i],&pks[i]);cpks[i].Phi=NULL;}
        for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],1);
        for(int bi=0;bi<N_BENCH;bi++){
            int iters=BENCH_ITERS[bi];
            double s_tot=0,v_tot=0;
            for(int it=0;it<iters;it++){
                for(int i=0;i<n;i++)UKG(i,&sks[i],&pks[i],&cpks[i],1);
                double ts=now_ms();
                Sig *s=Sign(MSG,1,cpks,pks,sks,0,n);
                s_tot+=now_ms()-ts;
                double tv=now_ms();
                Verify(MSG,s,cpks,pks,n);
                v_tot+=now_ms()-tv;
                free_sig(s);
            }
            fprintf(f_bench,"%d,%.6f,%.6f\n",iters,s_tot/iters,v_tot/iters);
            printf("%-14d %-12.4f %-12.4f\n",iters,s_tot/iters,v_tot/iters);
        }
        free_user(sks,pks,n);
        for(int i=0;i<n;i++)if(cpks[i].Phi)BN_free(cpks[i].Phi);
        free(sks);free(pks);free(cpks);
    }
    fclose(f_bench);

    printf("\nDone! CSV files written:\n");
    printf("  results_ring_size_dynamic.csv\n");
    printf("  results_epochs_dynamic.csv\n");
    printf("  results_bench_iter_dynamic.csv\n");

    BN_free(G_p);BN_free(G_q);BN_free(G_g);BN_free(G_x);BN_free(G_y);
    BN_CTX_free(G_ctx);
    return 0;
}