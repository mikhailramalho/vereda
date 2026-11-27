int main()
{
  union {
    unsigned char c;
    unsigned int i;
  } u = {0};
  u.i = 0x1200;
  u.c = 5;
  __VERIFIER_assert(u.i == 0x1205); // the write to c keeps the upper bytes of i
  return 0;
}
// Imported from ESBMC regression/esbmc/union_narrow_member_write
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
