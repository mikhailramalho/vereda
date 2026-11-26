/* esbmc/esbmc#1320 */
_Bool __VERIFIER_nondet_bool();
int __VERIFIER_nondet_int();

//x is an input variable
int x;

void foo() {
  x--;
}

int main() {
  x=__VERIFIER_nondet_int();
  while (x > 0) {
    _Bool c = __VERIFIER_nondet_bool();
    if(c) foo();
    else foo();
  }
  assert(x<=0);
}



// Imported from ESBMC regression/k-induction/trex02
// Needs: --k-induction
// RUN: --k-induction --std=gnu89
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
