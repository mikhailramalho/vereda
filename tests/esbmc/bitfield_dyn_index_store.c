struct s
{
  unsigned long m;
  unsigned int a : 8;
  unsigned int p : 1;
};

static struct s area[2];

int __VERIFIER_nondet_int(void);

int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 1);
  struct s *q = &area[i];
  q->p = 1;
  __VERIFIER_assert(area[i].p == 1 && area[1 - i].p == 0); // the store sets only area[i].p
  return 0;
}
// Imported from ESBMC regression/esbmc/bitfield_dyn_index_store
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
