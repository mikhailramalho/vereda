// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2024 Paulína Ayaziová
//
// SPDX-License-Identifier: GPL-3.0-or-later

extern void __assert_fail(const char *, const char *, unsigned int, const char *) __attribute__ ((__nothrow__, __leaf__)) __attribute__ ((__noreturn__));
void reach_error(){ __assert_fail("0", "while.c", 2, "reach_error"); }

extern int __VERIFIER_nondet_int();

int main() {
            
    int count = 0;    
    int a = __VERIFIER_nondet_int();

    if (a <= 0)
        return 0;

    while (a > 0) {
        int n = __VERIFIER_nondet_int();
        if (n > 0)
            a = a - n;
        count++;
    }

    if (count == -a)
        reach_error();

    return 0;
}

// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/svcomp_while_1-1A0
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --k-step 2 --unlimited-k-steps --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --unlimited-k-steps --no-vla-size-check --no-pointer-check --interval-analysis --no-bounds-check --error-label ERROR --k-induction --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
