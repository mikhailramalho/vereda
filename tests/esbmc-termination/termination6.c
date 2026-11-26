// SPDX-FileCopyrightText: 2021 Y. Cyrus Liu <yliu195@stevens.edu>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * Date: 2021-06-21
 * Author: yliu195@stevens.edu
 */



#include <stdbool.h>

extern int __VERIFIER_nondet_int(void);

int v; // count the number of bits set in v
unsigned int c, z; // c accumulates the total bits set in v
int y;       // word value to compute the parity of

int main(){
  y = __VERIFIER_nondet_int();
  v = __VERIFIER_nondet_int();
  if (v>=0){
  for (c = 0; v; v >>= 1)
    {
      z = v & 1;
      c = c+z;
    }
  y = z+1;
  } else {
    y=1;
  }
//  return (void*) z;
return 0;
}
// Imported from ESBMC regression/termination/termination6
// Needs: --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check --64 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
