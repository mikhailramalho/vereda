// The counterexample printed a pointer read from an array at a symbolic index
// as `&0` rather than the object it points to.
int __VERIFIER_nondet_int(void);

int a = 1, b = 2, c = 3;
int *ptrs[3];

int main(void)
{
  ptrs[0] = &a;
  ptrs[1] = &b;
  ptrs[2] = &c;
  int idx = __VERIFIER_nondet_int();
  __VERIFIER_assume(idx >= 0 && idx < 3);
  int *p = ptrs[idx];
  __VERIFIER_assert(*p != 3); // pointer read at a symbolic index
  return 0;
}
// Imported from ESBMC regression/esbmc/trace_data_pointer_array_element
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  p = &c$
// Vereda: unmet EXPECT: ^  p = &c$
// MODE: FUTURE
// Vereda: verdict FAILED
