struct S { unsigned short a, b, c; };
struct S s[4];
int main(void)
{
  unsigned char *p = (unsigned char *)&s[2] + 4;
  *(unsigned int *)p = 0x00020001u;
  __VERIFIER_assert(s[3].a == 0); // next
  return 0;
}
// Imported from ESBMC regression/esbmc/array_struct_spanning_const_fail
// RUN: --no-incremental-bmc
// EXPECT: ^  next$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  next$
// MODE: FUTURE
// Vereda: verdict FAILED
