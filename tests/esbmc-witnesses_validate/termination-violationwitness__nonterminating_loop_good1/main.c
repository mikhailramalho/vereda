// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2025 The SV-Benchmarks Community
//
// SPDX-License-Identifier: GPL-3.0-or-later

extern int __VERIFIER_nondet_int();

int main() {

    int x = __VERIFIER_nondet_int();
    int y = __VERIFIER_nondet_int();
    int tmp;

    while(x != y) {
        tmp = x;
        x = y;
        y = tmp;

        if (__VERIFIER_nondet_int())
            break;
    }

    return 0;
}
// Imported from ESBMC regression/witnesses_validate/termination-violationwitness/nonterminating_loop_good1
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --termination --max-inductive-step 3 --interval-analysis --validate-violation-witness --witness witness.yml
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check --64 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
