struct S
{
  int v[3];
};

int main()
{
  struct S s[2];
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++)
      s[i].v[j] = 1;

  int sum = 0;
  for (int *p = &s[0].v[0]; p != &s[1].v[2]; p++)
    sum += *p;
  __VERIFIER_assert(sum == 4); // the walk does not cover four elements
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_member_index_bound_fail
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
