// Two pointers from outside the program may point to the same memory (#137).
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int *get(void);

int main(void) {
  int *p = get(), *q = get();
  if (!p || !q)
    return 0;
  *p = 1;
  *q = 2;
  __VERIFIER_assert(*p == 1);
  return 0;
}
