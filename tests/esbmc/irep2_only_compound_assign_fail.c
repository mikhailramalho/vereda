/* The overflow claim the promotion keeps falsifiable: 3 + INT_MAX overflows in
   int. Narrowing `a` to char first would make it unreachable. */
char b;

int main(void)
{
  int a = 2147483647;

  b = 3;
  b += a;
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_compound_assign_fail
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
