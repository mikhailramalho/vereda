// RUN: --ub-uninit-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
// A volatile read of an uninitialised int whose value nothing uses is no
// use of it: MemorySanitizer reports none, and neither does Vereda (#375).
int main(void) {
  volatile int x;
  x;
  return 0;
}
