// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// count and tick belong to a file not given: tick, without a body, may
// change count.
extern int count;
void tick(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int before = count;
  tick();
  __VERIFIER_assert(count == before);
  return 0;
}
