# Forward-Secure Certificateless DualRing Signature (FSCLDRS)

## Overview

This repository contains the implementation and experimental evaluation of a Forward-Secure Certificateless DualRing Signature (FSCLDRS) scheme.

The proposed construction combines:

* Forward Security
* Certificateless Cryptography
* DualRing Ring Signatures
* Pairing-Free Cryptography
* Discrete Logarithm Based Security

The scheme is designed to provide anonymous authentication while protecting previously generated signatures even after secret key exposure.

---

## Motivation

Existing schemes provide only partial combinations of:

* Forward Security
* Certificateless Key Management
* DualRing Efficiency

This work combines all three properties into a single framework.

The proposed scheme eliminates:

* Key escrow problems of identity-based cryptography
* Certificate management overhead
* Dependence on expensive bilinear pairings

---

## Repository Structure

```text
src/
├── proposed/
│   └── fs_clrs_dynamic.c

├── baselines/
│   ├── clrs_chow_yap.c
│   └── fsdr_pairing.c

scripts/
└── plot_comparison.py

results/
figures/
docs/
paper/
```

---

## Implemented Schemes

### Proposed Scheme

Forward-Secure Certificateless DualRing Signature (FSCLDRS)

Features:

* Forward-secure key evolution
* Certificateless public key infrastructure
* DualRing anonymity
* Pairing-free implementation
* Schnorr-style proof construction

### Baseline 1

Chow-Yap Certificateless Ring Signature (CLRS)

### Baseline 2

Forward-Secure DualRing Signature (FSDR Pairing-Based)

---

## Cryptographic Assumptions

The proposed construction relies on:

* Discrete Logarithm Problem (DLP)
* SHA-256 Hash Functions
* Random Oracle Model

---

## Dependencies

* GCC
* OpenSSL
* GMP
* PBC Library (for baseline implementations)

---

## Build

### FSCLDRS

```bash
gcc fs_clrs_dynamic.c -o fs_clrs_dynamic \
-lssl -lcrypto -lm
```

### Chow-Yap CLRS

```bash
gcc clrs_chow_yap.c -o clrs \
-lpbc -lgmp -lssl -lcrypto -lm
```

### FSDR Pairing-Based

```bash
gcc fsdr_pairing.c -o fsdr \
-lpbc -lgmp -lssl -lcrypto -lm
```

---

## Experimental Evaluation

The repository includes benchmarking across:

* Ring Size Scalability
* Epoch Evolution
* Signing Time
* Verification Time
* Signature Size
* Benchmark Iterations

Generated results are stored in:

```text
results/
```

Generated figures are stored in:

```text
figures/
```

---

## Applications

The proposed scheme is suitable for:

* Digital Forensics
* Privacy-Preserving Authentication
* Anonymous Reporting Systems
* Whistleblowing Platforms
* Electronic Voting
* Decentralized Security Systems

---

## Author

Gabriel Robby Susmith Kari

B.Tech Mechanical Engineering

IIITDM Kancheepuram

---

## Citation

If you use this repository in academic work, please cite the corresponding paper:

Forward-Secure Certificateless DualRing Signatures (FSCLDRS).

```
```
