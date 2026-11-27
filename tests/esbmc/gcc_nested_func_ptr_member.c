// Nested function reads/writes a captured struct pointer via `->`.
// Exercises the tokenizer's single-token handling of the `->` operator.
struct S
{
  int x;
  int y;
};

int main()
{
  struct S s = {1, 2};
  struct S *p = &s;

  void bump()
  {
    p->x += 10;
    p->y = p->x + p->y;
  }

  bump();
  __VERIFIER_assert(p->x == 11); // bump via -> updated x
  __VERIFIER_assert(p->y == 13); // bump via -> updated y
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_ptr_member
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
