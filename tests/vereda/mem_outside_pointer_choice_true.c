// A store through a choice between a local whose address the program keeps
// and a pointer read from memory after a store at a
// symbolic index writes the local on its side (#137).
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int c = __VERIFIER_nondet_int();
  int x = 0, y = 0;
  int *p[2] = {0, 0};
  p[__VERIFIER_nondet_int() & 1] = &y; // p[0] is now a read from memory
  int *q = c ? &x : p[0];
  if (!q)
    return 0;
  *q = 1;
  if (c)
    __VERIFIER_assert(x == 1);
  return 0;
}
