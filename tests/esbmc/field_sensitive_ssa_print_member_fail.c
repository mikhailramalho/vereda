union u
{
  int i;
  char c[4];
};

struct holder
{
  union u un;
  int t;
};

int main(void)
{
  struct holder v;
  v.un.i = 9;
  v.t = 4;
  __VERIFIER_assert(v.t != 4); // t is 4
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_print_member_fail
// RUN: --no-slice
// MODE: FUTURE
// EXPECT: ^  v\.un\.i = 9; //
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
