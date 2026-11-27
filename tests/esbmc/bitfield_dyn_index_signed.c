struct s
{
  unsigned long m;
  signed int t : 3;
};

static struct s area[2];

int __VERIFIER_nondet_int(void);

int main(void)
{
  area[0].t = -3;
  area[1].t = 2;
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 1);
  struct s *q = &area[i];
  __VERIFIER_assert(q->t == (i == 0 ? -3 : 2)); // the signed bit-field keeps its value
  return 0;
}
// Imported from ESBMC regression/esbmc/bitfield_dyn_index_signed
// EXPECT: ^VERIFICATION SUCCESSFUL$
