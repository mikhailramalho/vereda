// This file is part of the SV-Benchmarks collection of verification tasks:
// https://gitlab.com/sosy-lab/benchmarking/sv-benchmarks
//
// SPDX-FileCopyrightText: 2022-2023 University of Tartu & Technische Universität München
//
// SPDX-License-Identifier: MIT
#include <assert.h>
extern void abort(void);
void reach_error() { assert(0); }
void __VERIFIER_assert(int cond) { if(!(cond)) { ERROR: {reach_error();abort();} } }

#include <setjmp.h>

jmp_buf my_jump_buffer;

void foo(int count)
{
    __VERIFIER_assert(count >= 0 && count <= 5);
    longjmp(my_jump_buffer, 1);
}

int main(void)
{
    volatile int count = 0;
    setjmp(my_jump_buffer);
    if (count < 5) {
        count++;
        foo(count);
    }
    __VERIFIER_assert(count == 5);
}


// Imported from ESBMC regression/cstd/jump_unreach
// Needs: --sv-comp --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3
// RUN: --sv-comp --ub-div-by-zero-check=false --no-align-check --k-step 2 --unlimited-k-steps --vla-size-check=false -m64 --ub-pointer-check=false --interval-analysis --no-bounds-check --error-label ERROR --k-induction --max-inductive-step 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
