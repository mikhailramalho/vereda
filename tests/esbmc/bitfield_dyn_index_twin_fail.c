struct s
{
  unsigned long m;
  signed int t : 3;
};

struct u
{
  unsigned long m;
  unsigned int t : 3;
};

static struct s area[2];

int __VERIFIER_nondet_int(void);

int main(void)
{
  area[0].t = -3;
  area[1].t = 2;
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 1);
  struct u *q = (struct u *)&area[i];
  __VERIFIER_assert(q->t != 5); // area[0].t reads as 5
  return 0;
}
// Imported from ESBMC regression/esbmc/bitfield_dyn_index_twin_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
