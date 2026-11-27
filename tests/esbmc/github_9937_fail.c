/* #9937: writing a member of a union whose other member holds an
 * out-of-bounds pointer must not make the path vacuous. */
int main()
{
  union
  {
    int *p;
    struct
    {
      int *a;
      long c;
    } s;
  } u;
  int arr[2];
  u.p = arr + 100;
  u.s.c = 4;
  __VERIFIER_assert(u.s.c != 4); // reachable
}
// Imported from ESBMC regression/esbmc/github_9937_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  u\.s\.c != 4$
