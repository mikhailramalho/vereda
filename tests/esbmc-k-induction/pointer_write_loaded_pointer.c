// The loop writes through t[i], a pointer loaded through t = tbl, so what it
// writes is what tbl's elements point to: havocking lists proves the bound.
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }
unsigned __VERIFIER_nondet_uint(void);

struct reg
{
  int off;
};

struct reg lists[2];
struct reg *tbl[2] = {&lists[0], &lists[1]};

int main()
{
  struct reg **t = tbl;
  for (;;)
  {
    unsigned i = __VERIFIER_nondet_uint() & 1u;
    t[i]->off = (t[i]->off + 1) % 10;
    __VERIFIER_assert(lists[i].off < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_loaded_pointer
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
