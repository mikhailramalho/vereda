// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2022 Dirk Beyer, Matthias Dangl, Daniel Dietsch, Matthias Heizmann, Thomas Lemberger, and Michael Tautschnig
//
// SPDX-License-Identifier: Apache-2.0

extern void __assert_fail(const char *, const char *, unsigned int, const char *) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
void reach_error() { __assert_fail("0", "linear-inequality-inv-a.c", 2, "reach_error"); }
extern unsigned int __VERIFIER_nondet_uint(void);
int main() {
  unsigned int n = __VERIFIER_nondet_uint();
  if (n == 0) {
    return 0;
  }
  unsigned int   v = 0;
  unsigned long  s = 0;
  unsigned long  i = 0;
  while (i < n) {
    v = __VERIFIER_nondet_uint();
    s += v;
    ++i;
  }
  if (s < v) {
    reach_error();
    return 1;
  }
  if (s > 18446744065119617025ULL) { // (2**32 - 1)**2
    reach_error();
    return 1;
  }
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/svcomp_linear_inequality_inv_d_1
// Needs: --force-realloc-success --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// RUN: -m64 --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --unlimited-k-steps --vla-size-check=false --ub-pointer-check=false --interval-analysis --no-bounds-check --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
