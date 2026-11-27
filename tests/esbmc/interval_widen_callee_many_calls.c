// Without widening on entry to check(), the interval analysis took over two
// minutes here.
unsigned offset, count, job;
void check(int c)
{
}
void task(void)
{
  offset += count++;
}
#define STEP                                                                   \
  task();                                                                      \
  switch (job)                                                                 \
  {                                                                            \
  case 0:                                                                      \
    check(0);                                                                  \
  case 1:                                                                      \
    check(1);                                                                  \
  case 2:                                                                      \
    check(2);                                                                  \
  case 3:                                                                      \
    check(3);                                                                  \
  }
#define STEP10 STEP STEP STEP STEP STEP STEP STEP STEP STEP STEP
#define STEP40 STEP10 STEP10 STEP10 STEP10
int main()
{
  STEP40;
  STEP40;
  STEP40;
  STEP40;
  __VERIFIER_assert(count == 160); // count
}
// Imported from ESBMC regression/esbmc/interval_widen_callee_many_calls
// Needs: --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate
// RUN: --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
