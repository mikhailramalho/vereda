// RUN: --property-file %S/properties/termination.prp --error-label ERROR
// EXPECT: ^TRUE$
// The check --error-label puts at a reached label calls nothing, so a
// program that takes a function's address terminates as without the flag.
static int twice(int x) { return 2 * x; }
int (*fp)(int) = twice;

int main(void) {
  int s = 0;
  for (int i = 0; i < 4; i++)
    s += twice(i);
  if (s == 12) {
  ERROR:
    s = 0;
  }
  return s + (fp == 0);
}
