// A bodyless call follows a stored pointer only on the paths that stored
// it: x and y keep their values where no object the call reaches points
// to them, through one stored pointer (box) or two (outer, box2) (#505).
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
  if (!c)
    __VERIFIER_assert(x == 1);
  if (c)
    __VERIFIER_assert(y == 2);
  return 0;
}
