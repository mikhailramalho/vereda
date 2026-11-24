// EXPECT: ^VERIFICATION FAILED$
// A call without a body writes no constant it gets, but may write through a
// pointer stored in one (#114).
extern void __VERIFIER_assert(int);
struct S {
  int *p;
};
void ext(const struct S *);
static int x;
static const struct S s = {&x};
int main(void) {
  ext(&s);
  __VERIFIER_assert(x == 0);
  return 0;
}
