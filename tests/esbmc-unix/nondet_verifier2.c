// This file is part of the SV-Benchmarks collection of verification tasks:
// https://gitlab.com/sosy-lab/benchmarking/sv-benchmarks
//
// SPDX-FileCopyrightText: 2025 The SV-Benchmarks Community
//
// SPDX-License-Identifier: Apache-2.0

typedef long unsigned int size_t;

void reach_error() {}
extern void __VERIFIER_nondet_memory(void *ptr, size_t size);

struct A {
  int x;
  int y;
};

int main() {
  struct A a = {0, 0};
  __VERIFIER_nondet_memory(&a, sizeof(a));
  if (a.x != a.y) {
    reach_error();
  }
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/nondet_verifier2
// Needs: --force-realloc-success --interval-analysis --error-label ERROR
// RUN: --ub-div-by-zero-check=false --force-malloc-success --force-realloc-success --k-step 2 --unlimited-k-steps --vla-size-check=false -m64 --ub-pointer-check=false --interval-analysis --error-label ERROR --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
