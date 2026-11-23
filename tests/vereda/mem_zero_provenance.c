// A zero-filled object has constant arrays of its own, and storing a byte's
// provenance 0 into them changes nothing, so it folds away (#98). A pointer
// stored there must still keep its provenance when read at a nondet index.
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
  __VERIFIER_assert(*slots[i] == 5 || (i == 4 && *slots[i] == 6));
  __VERIFIER_assert(slots[i + 1] == 0 || i == 3);
  return 0;
}
// EXPECT: ^VERIFICATION SUCCESSFUL$
