/* #8097: on a big-endian target a union's members share its lowest-addressed
 * bytes, so .c is the most significant byte of .i. */
unsigned int __VERIFIER_nondet_uint();
union V
{
  unsigned char c;
  unsigned int i;
};
int main()
{
  union
  {
    unsigned long long l;
    struct
    {
      union V v[1];
      char c[2];
    } s;
  } u;
  union V w;
  w.i = __VERIFIER_nondet_uint();
  u.s.v[0] = w;
  __VERIFIER_assert(u.s.v[0].i != 0x01020304u); // show
}
// Imported from ESBMC regression/esbmc/github_8097_trace_big_endian_union
// Needs: --big-endian
// RUN: --big-endian
// MODE: FUTURE
// EXPECT: ^  u\.s\.v\[0\] = \{ \.c=1
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
