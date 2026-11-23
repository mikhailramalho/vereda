// The assertion is still checked when its description is not a literal, and
// the report falls back to the guard rather than an empty message. #1557
int __VERIFIER_nondet_int(void);

static void checked(const char *why, int ok)
{
  __VERIFIER_assert(ok); // why
}

int main(void)
{
  int x = __VERIFIER_nondet_int();
  checked("x must be positive", x > 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/assert_desc_not_literal_fail
// EXPECT: ^VERIFICATION FAILED$
