// Witness generation still has compile-unit files, data model and nondet
// return types after the LLVM module is freed before solving in single-walk
// mode.
// RUN: --witness -
// EXPECT: format_version: "2\.0"
// EXPECT: data_model: "LP64"
// EXPECT: type: "function_return"
// EXPECT: value: "\\\\result == 42"
// EXPECT: type: "function_return"
// EXPECT: value: "\\\\result == 0x1\.8p1"
// EXPECT: type: "branching"
// EXPECT: type: "target"
// EXPECT: ^VERIFICATION FAILED$
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_after_module_reset.c", 15, "reach_error"); }
extern int __VERIFIER_nondet_int(void);
extern double __VERIFIER_nondet_double(void);
extern void *__VERIFIER_nondet_pointer(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 0) {
    double d = __VERIFIER_nondet_double();
    void *p = __VERIFIER_nondet_pointer();
    if (x == 42 && d == 3.0 && p != 0)
      reach_error();
  }
  return 0;
}
