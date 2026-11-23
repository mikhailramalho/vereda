typedef struct { unsigned a; unsigned arr[4]; unsigned z; } S;
S g;
int main(void)
{
  S *p;
  if (__VERIFIER_nondet_int()) p = &g; else p = (S *)((char *)0 + 16);
  unsigned *q = &p->arr[1];
  unsigned v = *q;
  return v;
}
// Imported from ESBMC regression/esbmc/array_member_ptr_null_offset_read_fail
// EXPECT: ^  dereference failure: NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: NULL pointer$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
