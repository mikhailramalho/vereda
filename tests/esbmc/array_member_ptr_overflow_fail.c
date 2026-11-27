typedef struct { unsigned a; unsigned arr[4]; unsigned z; } S;
S g; S *p = &g;
int main(void)
{
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k <= 4);
  unsigned *q = &p->arr[k];
  *q = 1;               /* k == 4 writes past arr into z */
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_ptr_overflow_fail
// EXPECT: ^  dereference failure: array bounds violated$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: array bounds violated$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
