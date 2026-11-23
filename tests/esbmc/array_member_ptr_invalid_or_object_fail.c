typedef struct { unsigned a; unsigned arr[4]; } S;
S g;
int main(void)
{
  S *p;
  if (__VERIFIER_nondet_int())
    p = &g;
  unsigned *q = &p->arr[1];
  return *q;
}
// Imported from ESBMC regression/esbmc/array_member_ptr_invalid_or_object_fail
// EXPECT: ^  dereference failure
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
