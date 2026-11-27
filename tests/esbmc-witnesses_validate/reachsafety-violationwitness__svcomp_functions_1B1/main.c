// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2024 Paulína Ayaziová
//
// SPDX-License-Identifier: GPL-3.0-or-later

extern void __assert_fail(const char *, const char *, unsigned int, const char *) __attribute__ ((__nothrow__, __leaf__)) __attribute__ ((__noreturn__));
void reach_error(){ __assert_fail("0", "functions.c", 2, "reach_error"); }

extern int __VERIFIER_nondet_int();
int foo();
int bar();
int baz();

int foo(int x) {
    int y = __VERIFIER_nondet_int();

    if (x > 100 || y > 100)
        return 0;

    if (x > y)
        return bar(x + y);

    return 0;
}

int bar(int x) {
    int y = __VERIFIER_nondet_int();

    if (x > 100 || y > 100)
        return 0;

    if (x > y)
        return baz(x * y);

    return 0;
}

int baz(int x) {
    int y = __VERIFIER_nondet_int();
    if (x > y)
        reach_error();
    return 100;
}

int main() {
    int x = __VERIFIER_nondet_int();
    
    int y = foo(x);
    int z = bar(y);
    baz(z);

    return 0;    
}

// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/svcomp_functions_1B1
// Needs: --force-realloc-success --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --unlimited-k-steps --vla-size-check=false --ub-pointer-check=false --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
