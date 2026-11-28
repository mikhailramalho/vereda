// RUN: --ub-uninit-check
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \[main\.uninitialised-variable\.1\]  line 8  use of uninitialized variable: x
// An atomic load of an uninitialised int whose value is returned is a use,
// which MemorySanitizer reports, and is still checked (#375).
int main(void) {
  int x;
  return __atomic_load_n(&x, __ATOMIC_RELAXED);
}
