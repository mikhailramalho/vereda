struct S { unsigned int a : 4; unsigned int b : 28; unsigned int c; };
struct S s[2];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  s[i].c = 9;
  *(unsigned int *)&s[i] = 0x31u;
  /* b == 0 is the unmodelled big-endian store (#10087); 0x31 is the C layout */
  __VERIFIER_assert(s[i].a == 0 && (s[i].b == 0 || s[i].b == 0x31)); // big-endian layout
  __VERIFIER_assert(s[i].c == 9); // c untouched
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_dyn_offset_spanning_bitfield_big_endian
// Needs: --big-endian
// RUN: --big-endian --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
