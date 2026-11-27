// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2025 RWTH Aachen
//
// SPDX-License-Identifier: LicenseRef-RWTH-Aachen

typedef enum {false,true} bool;

extern int __VERIFIER_nondet_int(void);

int main() {
    int i;
    i = __VERIFIER_nondet_int();
    
    while (i > 0) {
        if (i != 5) {
            i = i-1;
        }
    }
    
    return 0;
}
// Imported from ESBMC regression/witnesses_validate/termination-violationwitness/Ex02_good1
// Needs: --force-realloc-success --termination --max-inductive-step 3 --interval-analysis --validate-violation-witness --witness witness.yml
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --unlimited-k-steps --vla-size-check=false -m64 --ub-pointer-check=false --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis --validate-violation-witness --witness witness.yml
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
