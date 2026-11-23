// A .i file's linemarkers make clang take the file and line they name as the
// location of what follows, so the call to reach_error below sat in foo.c at
// line 300 and got no witness. vereda.py compiles a copy with the linemarkers
// blanked, so the witness names this file and its own lines.
// RUN: --witness -
// EXPECT: file_name: "[^"]*witness_linemarkers\.i"\n\s*line: 22\n\s*column: 5\n\s*function: "main"
// EXPECT-NOT: foo\.c
// EXPECT: VERIFICATION FAILED
# 1 "foo.c"
# 1 "<built-in>" 1
# 1 "foo.c" 2
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "foo.c", 3, "reach_error"); }
extern int __VERIFIER_nondet_int(void);
# 1 "bar.h" 1
int bar(void) { return 1; }
# 100 "foo.c" 2
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 3)
#line 300 "foo.c"
    reach_error();
  return 0;
}
