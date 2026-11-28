// Loops number as ESBMC's --unwindsetname counts them, in the order of
// their back edges: the inner loop is 0, the loop around it 1 and the last
// loop 2. Each gets one more than its trip count, so all unwind in full.
// RUN: --no-summarize-array-loops --unwind 1 --unwindsetname main:0:3,main:1:4,main:2:3
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);

int main(void) {
  int s = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 2; j++)
      s++;
  for (int k = 0; k < 2; k++)
    s++;
  __VERIFIER_assert(s == 8);
  return 0;
}
