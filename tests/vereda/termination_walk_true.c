// RUN: --property-file %S/properties/termination.prp
// EXPECT: Termination not shown without a walk: SCEV cannot bound the loop
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^TRUE$
// SCEV cannot count a search through an array nor bound recursion, but the
// walk unwinds both completely.
int depth(int n) { return n <= 0 ? 0 : 1 + depth(n - 1); }

int main(void) {
  int a[4] = {1, 2, 0, 3}, i = 0;
  while (a[i])
    i++;
  return depth(i);
}
