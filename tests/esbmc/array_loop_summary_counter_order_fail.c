// Another counter's update follows the loop counter's.
unsigned __VERIFIER_nondet_uint(void);
int main() {
  unsigned x = __VERIFIER_nondet_uint();
  unsigned y = x;
  while (x < 100) { x++; y++; }
  __VERIFIER_assert(x == y + 1);
}
// Imported from ESBMC regression/esbmc/array_loop_summary_counter_order_fail
// EXPECT: ^VERIFICATION FAILED$
