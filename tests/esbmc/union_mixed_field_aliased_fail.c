union u
{
  int a;
  short b;
};

int main(void)
{
  union u u;
  u.a = 0x12345678;
  u.b = 1;

  /* Guards member2t::do_simplify's union arm (src/util/expr/expr_simplifier.cpp):
     dropping it folds this read past the sibling write and flips the verdict. */
  __VERIFIER_assert(u.a == 0x12345678); // an aliased read must not fold
  return 0;
}
// Imported from ESBMC regression/esbmc/union_mixed_field_aliased_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
