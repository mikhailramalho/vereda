// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2024 Paulína Ayaziová
//
// SPDX-License-Identifier: GPL-3.0-or-later

extern void __assert_fail(const char *, const char *, unsigned int, const char *) __attribute__ ((__nothrow__, __leaf__)) __attribute__ ((__noreturn__));
void reach_error(){ __assert_fail("0", "switch.c", 2, "reach_error"); }
extern char __VERIFIER_nondet_char();
extern int __VERIFIER_nondet_int();

int main() {
    int a = __VERIFIER_nondet_int();
    char b = __VERIFIER_nondet_char();

    switch(a) {
    case 1:
        return 0;
    case 2:
        switch(b){
        case 1:
            reach_error();
        case 2:
            return 0;
        case 3:
            a++;
            break;    
        default:
            b++;   
        }
    case 3:
        a++;
        break;    
    default:
        b++;   
    }
    return 0;
}

// Imported from ESBMC regression/witnesses_validate/reachsafety-violationwitness/svcomp_switch_2A1
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --unlimited-k-steps --no-vla-size-check --no-pointer-check --interval-analysis --no-bounds-check --error-label ERROR --k-induction --max-inductive-step 3 --validate-violation-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
