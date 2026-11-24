// EXPECT: ^VERIFICATION FAILED$
// A call without a body may store a pointer it gets into another object it
// writes, so a later call may write through it (#114).
extern void __VERIFIER_assert(int);
struct S {
  int *p;
};
void ext2(struct S *, int *);
void ext3(struct S *);
int main(void) {
  struct S s = {0};
  int y = 0;
  ext2(&s, &y);
  y = 0;
  ext3(&s);
  __VERIFIER_assert(y == 0);
  return 0;
}
