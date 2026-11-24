void __VERIFIER_nondet_memory(void *, __SIZE_TYPE__);
const int t[2] = {1, 2};
int main()
{
  __VERIFIER_nondet_memory((void *)t, sizeof t);
  return 0;
}
// Imported from ESBMC regression/esbmc/const_object_write_nondet_memory_fail
// EXPECT: ^  dereference failure: write access to const object `t'$
// EXPECT: ^VERIFICATION FAILED$
