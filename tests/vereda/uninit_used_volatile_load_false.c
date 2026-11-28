// RUN: --ub-uninit-check
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \[main\.uninitialised-variable\.1\]  line 8  use of uninitialized variable: x
// A volatile read of an uninitialised int whose value is returned is a
// use, which MemorySanitizer reports, and is still checked (#375).
int main(void) {
  volatile int x;
  return x;
}
