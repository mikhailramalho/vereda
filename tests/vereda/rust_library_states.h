extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
#line 1 "/rustc/123abc/library/core/src/lib.rs"
int helper(int input) {
  int internal = input + 1;
  return internal;
}
#line 1 "/rustc/123abc/libraryish/user.c"
int near_path(int input) {
  int visible = input + 2;
  return visible;
}
#line 1 "user_program.c"
int main(void) {
  int value = helper(__VERIFIER_nondet_int());
  int result = near_path(value);
  __VERIFIER_assert(result != 7);
}
