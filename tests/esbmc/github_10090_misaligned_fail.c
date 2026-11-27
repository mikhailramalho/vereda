/* #10090: a store that starts in an array member at an odd offset and ends in
 * the next member is still misaligned. */
struct S
{
  unsigned char c[6];
  unsigned short h;
};
struct S s[2];
int main(void)
{
  unsigned char *p = (unsigned char *)&s[1] + 5;
  *(unsigned short *)p = 0x0201u;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10090_misaligned_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: Incorrect alignment when accessing data object
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
