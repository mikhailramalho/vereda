// EXPECT: ^VERIFICATION FAILED$
// After a call without a body, the object it wrote may still hold the
// pointers it held, so a second call may write x again (#114).
extern void __VERIFIER_assert(int);
struct S {
  int *p;
};
void ext(struct S *);
int main(void) {
  int x = 0;
  struct S s = {&x};
  ext(&s);
  x = 0;
  ext(&s);
  __VERIFIER_assert(x == 0);
  return 0;
}
