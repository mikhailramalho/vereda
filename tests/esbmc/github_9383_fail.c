/* #9383: one byte past an array whose element has an unnamed bit-field. */
struct A
{
  char c;
  int : 4;
};
struct A a[3];

int main(void)
{
  char *b = (char *)a;
  b[sizeof a + 1] = 1;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9383_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  FAILED       \[main\.array-bounds-violated\.1\]  line 12  dereference failure: array bounds violated$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  FAILED       \[main\.array-bounds-violated\.1\]  line 12  dereference failure: array bounds violated$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
