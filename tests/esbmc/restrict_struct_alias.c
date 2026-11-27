/* restrict pointers to a named struct: the pointee type is symbolic, which must
   not abort element-size computation. Aliasing writes are still UB. */
struct S
{
  int x;
};

void f(struct S *restrict a, struct S *restrict b)
{
  a->x = 1;
  b->x = 2;
}

int main(void)
{
  struct S s = {0};
  f(&s, &s);
  return 0;
}
// Imported from ESBMC regression/esbmc/restrict_struct_alias
// Needs: --restrict-check
// RUN: --restrict-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*restrict pointer aliasing$
// Vereda: verdict none
