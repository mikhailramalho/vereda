// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2022 Dirk Beyer, Matthias Dangl, Daniel Dietsch, Matthias Heizmann, Thomas Lemberger, and Michael Tautschnig
//
// SPDX-License-Identifier: Apache-2.0

extern void __assert_fail(const char *, const char *, unsigned int, const char *) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
void reach_error() { __assert_fail("0", "linear-inequality-inv-a.c", 2, "reach_error"); }
extern unsigned short __VERIFIER_nondet_ushort(void);
int main() {
  unsigned short n = __VERIFIER_nondet_ushort();
  if (n == 0) {
    return 0;
  }
  unsigned short v = 0;
  unsigned int  s = 0;
  unsigned int  i = 0;
  while (i < n) {
    v = __VERIFIER_nondet_ushort();
    s += v;
    ++i;
  }
  if (s < v) {
    reach_error();
    return 1;
  }
  if (s > 4294836225) { // (2**16 - 1)**2
    reach_error();
    return 1;
  }
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/svcomp_linear_inequality_inv_c_2
// Needs: --force-realloc-success --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// RUN: -m32 --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --unlimited-k-steps --vla-size-check=false --ub-pointer-check=false --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
