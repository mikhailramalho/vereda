// Implicit unsigned-long -> void* conversion. clang 15+ treats -Wint-conversion
// as a hard error by default and rejects this at parse time unless ESBMC passes
// -Wno-int-conversion, which it does under SV-COMP mode (--sv-comp). Mirrors the
// `struct mutex` sentinel initializer in the SV-COMP imon benchmark (#5142),
// which CIL emits as (void *)0xffffffffffffffffUL.
int main(void)
{
  void *p = 0xdeadUL; // implicit int -> pointer
  unsigned long v = (unsigned long)p;
  __VERIFIER_assert(v == 0xdeadUL); // int->ptr->int round-trip preserved
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5142_int_conversion
// Needs: --sv-comp
// RUN: --sv-comp --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
