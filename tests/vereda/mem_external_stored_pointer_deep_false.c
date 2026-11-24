// EXPECT: ^VERIFICATION FAILED$
// A call without a body may follow stored pointers any number of times
// (#114).
extern void __VERIFIER_assert(int);
struct S {
  int *p;
};
struct T {
  struct S *s;
};
void ext(struct T *);
int main(void) {
  int x = 0;
  struct S s = {&x};
  struct T t = {&s};
  ext(&t);
  __VERIFIER_assert(x == 0);
  return 0;
}
