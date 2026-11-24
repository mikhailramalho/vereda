// A write into an object defined const, through a pointer cast from its
// address, is undefined (C11 6.7.3p6): ESBMC's pointer check reports it,
// naming the object, and so does Vereda, although InstCombine would delete
// the store.
// EXPECT: ^Violated property:\n  file \S*mem_const_write_false\.c line 14 column 20 function main\n  dereference failure: write access to const object `limit'$
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 14  dereference failure: write access to const object `limit'$
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);

const int limit = 10;

int main(void) {
  if (__VERIFIER_nondet_int())
    *(int *)&limit = 5;
  return 0;
}
