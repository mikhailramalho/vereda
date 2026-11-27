/* #11015: a whole union stored through a misaligned pointer. */
union u4 { unsigned int i; unsigned char c[4]; };
struct d { unsigned long long x; };
int main(void)
{
  struct d d = {0};
  union u4 w = {0xffeeddcc};
  *(union u4 *)((unsigned char *)&d + 2) = w;
  return (int)(d.x >> 16);
}
// Imported from ESBMC regression/esbmc/github_11015_union_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: Incorrect alignment when accessing data object\b
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
