// The failing twin of mem_zero_provenance: the pointer read back through
// slots' own arrays names y on one path.
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int x = 5, y = 6;
int *slots[512];
int main(void) {
  slots[3] = &x;
  if (__VERIFIER_nondet_int())
    slots[4] = &x;
  else
    slots[4] = &y;
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i == 3 || i == 4);
  __VERIFIER_assert(*slots[i] == 5);
  return 0;
}
// EXPECT: ^VERIFICATION FAILED$
