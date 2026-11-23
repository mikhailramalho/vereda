// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2022 Dirk Beyer, Matthias Dangl, Daniel Dietsch, Matthias Heizmann, Thomas Lemberger, and Michael Tautschnig
//
// SPDX-License-Identifier: Apache-2.0

extern void __assert_fail(const char *, const char *, unsigned int, const char *) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
void reach_error() { __assert_fail("0", "linear-inequality-inv-a.c", 2, "reach_error"); }
extern unsigned char __VERIFIER_nondet_uchar(void);
int main() {
  unsigned char n = __VERIFIER_nondet_uchar();
  if (n == 0) {
    return 0;
  }
  unsigned char v = 0;
  unsigned int  s = 0;
  unsigned int  i = 0;
  while (i < n) {
    v = __VERIFIER_nondet_uchar();
    s += v;
    ++i;
  }
  if (s < v) {
    reach_error();
    return 1;
  }
  if (s > 65025) {
    reach_error();
    return 1;
  }
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/svcomp_linear_inequality_inv_a_2
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --k-step 2 --unlimited-k-steps --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml REQUIRES arch32
// RUN: --32 --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --unlimited-k-steps --no-vla-size-check --no-pointer-check --interval-analysis --no-bounds-check --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
