//#include <assert.h>

int main()
{
  int a = 1;
  assert(a == 1);
  a += 1;
  a /= 1;
  a %= 1;
  a -= 1;
  a >>= 1;
  a <<= 1;
  a &= 1;
  a |= 1;
  a ^= 1;
  return 0;
}
// Imported from ESBMC regression/llvm/compound_assing
// EXPECT: ^VERIFICATION SUCCESSFUL$
