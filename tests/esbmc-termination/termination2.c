// SPDX-FileCopyrightText: 2021 Y. Cyrus Liu <yliu195@stevens.edu>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * Date: 2021-06-21
 * Author: yliu195@stevens.edu
 */

extern int __VERIFIER_nondet_int(void);

int main (){
    int x, y, res;
    x = __VERIFIER_nondet_int();
    y = __VERIFIER_nondet_int();
    while (x>=y && y > 0){
        x = (x-1)&y;
    }
    return 0;
}
// Imported from ESBMC regression/termination/termination2
// Needs: --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check -m64 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
