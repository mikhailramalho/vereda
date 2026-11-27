// Reduced from SV-COMP c/float-newlib/float_req_bl_0680a (#7792). --no-slice
// keeps both union reads in each claim's formula.
typedef unsigned int u32;
typedef union
{
  float value;
  u32 word;
} shape;

int main(void)
{
  float x = -0.0f, y = 0.0f;
  u32 hx, hy;

  do
  {
    shape u;
    u.value = x;
    hx = u.word;
  } while (0);
  do
  {
    shape u;
    u.value = y;
    hy = u.word;
  } while (0);

  __VERIFIER_assert(hx == 0x80000000u); // -0.0f puns to 0x80000000
  __VERIFIER_assert(hy == 0x80000000u); // +0.0f does not pun to 0x80000000
  return 0;
}
// Imported from ESBMC regression/floats-regression/github_7792_fail
// RUN: -m32 --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
