// Regression for GitHub #7840 (unsigned counterpart): the widened-operand
// shortcut also applies to unsigned multiplication under
// --unsigned-overflow-check — two 32-bit unsigned factors can never overflow
// a 64-bit unsigned product.
unsigned long long q(unsigned a, unsigned b)
{
  return (unsigned long long)a * (unsigned long long)b;
}
// Imported from ESBMC regression/esbmc/github_7840_unsigned
// RUN: --unsigned-overflow-check --entry q
// EXPECT: ^Generated 3 VCC\(s\), 2 remaining after simplification
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^Generated 3 VCC\(s\), 2 remaining after simplification
// MODE: FUTURE
