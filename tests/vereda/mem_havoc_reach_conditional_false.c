// As mem_havoc_reach_conditional, on the other paths: there the call
// reaches x through box, and y through outer and box2, and may change
// them (#505).
// RUN: --multi-property
// EXPECT: ^  FAILED +\[main\.assertion\.1\]  line 24  __VERIFIER_assert$
// EXPECT: ^  FAILED +\[main\.assertion\.2\]  line 26  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
void opaque(void *p);

int main(void) {
  int x = 1, y = 2;
  int *box = 0, *box2 = 0;
  int **outer = &box2;
  int c = __VERIFIER_nondet_int();
  if (c)
    box = &x;
  else
    box2 = &y;
  opaque(&box);
  opaque(&outer);
  if (c)
    __VERIFIER_assert(x == 1);
  if (!c)
    __VERIFIER_assert(y == 2);
  return 0;
}
