extern void abort(void);
extern void __assert_fail(const char *, const char *, unsigned int, const char *) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__noreturn__));
void reach_error() { __assert_fail("0", "trex02-2.c", 3, "reach_error"); }

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: {reach_error();abort();}
  }
  return;
}
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
  __VERIFIER_assert(x==0);
}



// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/svcomp_trex02_2_1
// Needs: --force-realloc-success --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// RUN: -m32 --no-div-by-zero-check --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check --no-pointer-check --interval-analysis --no-bounds-check --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
