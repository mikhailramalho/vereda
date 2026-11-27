/* Negative twin: -1 is exactly what a width-relaxed typecast fold would
 * wrongly produce for the cross-width read — it must stay refuted. */
typedef union
{
  signed char c;
  int i;
} u_t;
static u_t u;
int main(void)
{
  u.c = -1;
  __VERIFIER_assert(u.i == -1); // the width-relaxed fold's answer is wrong
  return 0;
}
// Imported from ESBMC regression/esbmc/union_cross_member_width_guard_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
