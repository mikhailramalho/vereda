// An __VERIFIER_assert whose description is a runtime pointer rather than a string
// literal used to abort the whole run out of get_string_constant. Only the
// assertion's description is read from there, so it degrades to a warning and
// verification proceeds. #1557
int __VERIFIER_nondet_int(void);

static void checked(const char *why, int ok)
{
  __VERIFIER_assert(ok); // why
}

int main(void)
{
  int x = __VERIFIER_nondet_int();
  checked("x equals itself", x == x);
  return 0;
}
// Imported from ESBMC regression/esbmc/assert_desc_not_literal
// EXPECT: ^VERIFICATION SUCCESSFUL$
