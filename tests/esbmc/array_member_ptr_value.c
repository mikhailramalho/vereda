typedef struct { unsigned a; unsigned arr[4]; unsigned z; } S;
S g; S *p = &g;
int main(void)
{
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 4);
  unsigned *q = &p->arr[k];
  *q = 7;
  __VERIFIER_assert(g.arr[k] == 7 && g.a == 0 && g.z == 0); // value
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_ptr_value
// EXPECT: ^VERIFICATION SUCCESSFUL$
