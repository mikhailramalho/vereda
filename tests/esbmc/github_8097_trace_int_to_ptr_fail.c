/* #8097: printing u.s.c[1] = 4 read c back from u's bytes, which hold &x;
 * decoding them as a pointer asserted side constraints and discarded the
 * model being read. */
int main()
{
  union
  {
    int *p;
    struct
    {
      int *a[1];
      char c[2];
    } s;
  } u;
  int x;
  u.p = &x;
  u.s.c[1] = 4;
  int *r = u.s.a[0];
  __VERIFIER_assert(r != &x); // r aliases u.p
}
// Imported from ESBMC regression/esbmc/github_8097_trace_int_to_ptr_fail
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^  u\.s\.c\[1\] = 4
// EXPECT: ^  r = &x;$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  r = &x$
// MODE: FUTURE
// Vereda: verdict FAILED
