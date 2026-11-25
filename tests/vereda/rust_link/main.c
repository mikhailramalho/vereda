// FILES: helper.rs
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int increment(int);
extern void __VERIFIER_assert(int);
int main(void) {
  __VERIFIER_assert(increment(4) == 5);
}
