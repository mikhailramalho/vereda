/* C11 6.5.16.2p3: `b += a` is equivalent to `b = b + (a)`, so the addition
   happens in int after promoting b -- 3 + INT_MAX overflows. Narrowing a to
   char first would make the overflow claim unfalsifiable. */
char b;

int main()
{
  b = 3;
  int a = 2147483647;
  b += a;
  return 0;
}
// Imported from ESBMC regression/esbmc/compound_assign_narrow_overflow
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
