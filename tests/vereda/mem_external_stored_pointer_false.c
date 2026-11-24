// EXPECT: ^VERIFICATION FAILED$
// A call without a body may write through a pointer stored in an object it
// gets (#114).
extern void __VERIFIER_assert(int);
struct S {
  int *p;
};
void ext(struct S *);
int main(void) {
  int x = 0;
  struct S s = {&x};
  ext(&s);
  __VERIFIER_assert(x == 0);
  return 0;
}
