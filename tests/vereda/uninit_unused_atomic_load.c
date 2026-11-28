// RUN: --ub-uninit-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
// An atomic load of an uninitialised int whose value nothing uses is no
// use of it: MemorySanitizer reports none, and neither does Vereda (#375).
int main(void) {
  int x;
  __atomic_load_n(&x, __ATOMIC_RELAXED);
  return 0;
}
