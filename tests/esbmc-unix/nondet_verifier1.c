// This file is part of the SV-Benchmarks collection of verification tasks:
// https://gitlab.com/sosy-lab/benchmarking/sv-benchmarks
//
// SPDX-FileCopyrightText: 2025 The SV-Benchmarks Community
//
// SPDX-License-Identifier: Apache-2.0

typedef long unsigned int size_t;

void reach_error() {}
extern void __VERIFIER_nondet_memory(void *ptr, size_t size);

enum Color { RED, GREEN, BLUE };

int main() {
  enum Color color = RED;
  __VERIFIER_nondet_memory(&color, sizeof(color));
  if (color != RED && color != GREEN && color != BLUE) {
    reach_error();
  }
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/nondet_verifier1
// Needs: --force-realloc-success --interval-analysis --error-label ERROR --goto-unwind --unlimited-goto-unwind
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check --64 --no-pointer-check --interval-analysis --no-bounds-check --error-label ERROR --goto-unwind --unlimited-goto-unwind --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
