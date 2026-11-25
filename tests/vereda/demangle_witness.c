// Witness locations keep the original function symbol.
// RUN: --witness -
// EXPECT: function: \"_RNvCsislBgIXLfl0_7largest7largest\"
// EXPECT: VERIFICATION FAILED
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
void example(void) __asm__("_RNvCsislBgIXLfl0_7largest7largest");
void example(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 3)
    reach_error();
}
int main(void) { example(); }
