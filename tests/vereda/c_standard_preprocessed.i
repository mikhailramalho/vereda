// RUN: --std c23
// EXPECT: ^VERIFICATION SUCCESSFUL$
# 1 "original.c"
extern void __VERIFIER_assert(int);
int main(void) {
  bool value = true;
  __VERIFIER_assert(value);
}
