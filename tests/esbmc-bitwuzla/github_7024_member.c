struct S
{
  int f;
};

int main()
{
  struct S m[4];
  int i;
  int *p;

  i = 2;
  p = &m[3].f;

  __VERIFIER_assert(
    __ESBMC_forall(&i, !(0 <= i && i < 4) || &m[i].f != p)
); // no member aliases p
}
// Imported from ESBMC regression/bitwuzla/github_7024_member
// RUN: --solver bitwuzla
// EXPECT: ^VERIFICATION FAILED$
