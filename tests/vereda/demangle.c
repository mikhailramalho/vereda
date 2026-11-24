// Human-readable names are demangled; property IDs retain their symbols.
// EXPECT: no body for function foo\(\)
// EXPECT: function largest::largest thread 0
// EXPECT: function largest::largest\n
// EXPECT: \[_RNvCsislBgIXLfl0_7largest7largest\.assertion\.1\]
// EXPECT: VERIFICATION FAILED
extern int unknown(void) __asm__("_Z3foov");
extern void __VERIFIER_assert(int);
void example(void) __asm__("_RNvCsislBgIXLfl0_7largest7largest");
void example(void) {
  int x = unknown();
  __VERIFIER_assert(x != 3);
}
int main(void) { example(); }
