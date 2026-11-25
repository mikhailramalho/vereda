// SPDX-FileCopyrightText: 2021 Y. Cyrus Liu <yliu195@stevens.edu>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * Date: 2021-06-21
 * Author: yliu195@stevens.edu
 */

extern int __VERIFIER_nondet_int(void);

int main (){
    int x;
    x = __VERIFIER_nondet_int();
    while (x>5){
        if (x != 10 ){
           x = x&(x-1);
        }
        else {
            x = x-1;
        }
    }
    return 0;
}
// Imported from ESBMC regression/termination/termination1
// Needs: --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2  --unlimited-k-steps --no-vla-size-check --64 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
