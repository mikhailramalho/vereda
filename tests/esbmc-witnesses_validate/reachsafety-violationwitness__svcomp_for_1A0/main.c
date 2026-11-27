// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2024 Paulína Ayaziová
//
// SPDX-License-Identifier: GPL-3.0-or-later

extern void __assert_fail(const char *, const char *, unsigned int, const char *) __attribute__ ((__nothrow__, __leaf__)) __attribute__ ((__noreturn__));
void reach_error(){ __assert_fail("0", "for.c", 2, "reach_error"); }

extern char __VERIFIER_nondet_char();
extern unsigned int __VERIFIER_nondet_uint();

int main() {
    char a[20];
    
    unsigned int count = 0;    
    for (int i = 0; i < 20; i++) {
        a[i] =  __VERIFIER_nondet_char();
        if (a[i] == 'a')
            count++;
    }

    if (count == 0)
        reach_error();

    if (count == __VERIFIER_nondet_uint())
        reach_error();

    return 0;
}

// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/svcomp_for_1A0
// Needs: --force-realloc-success --interval-analysis --k-induction --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --unlimited-k-steps --vla-size-check=false --ub-pointer-check=false --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
