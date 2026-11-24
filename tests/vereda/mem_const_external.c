// A call without a body given a pointer to a const object or a string
// literal may not write it, so it is no violation, as in ESBMC, and the
// object keeps its contents: here the pointer is the const object or a
// local array, and only the array gets nondet contents.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: dereference failure
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern void fill(char *);

const char name[] = "abc";

int main(void) {
  char buf[4];
  char *p = __VERIFIER_nondet_int() ? buf : (char *)name;
  fill(p);
  fill("xyz");
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 3);
  __VERIFIER_assert(((const volatile char *)name)[i] != 0); // not folded
  return 0;
}
