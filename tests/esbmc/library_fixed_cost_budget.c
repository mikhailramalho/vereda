/* Every model symbol linked here is cost this program did not ask for: the
 * budget in test.desc caps how many (issue #6831, cause 2). */
int main(void)
{
  return 0;
}
// Imported from ESBMC regression/esbmc/library_fixed_cost_budget
// Needs: REQUIRES bundled_libc
// MODE: FUTURE
// EXPECT: ^operational-model library \(clib\): [0-9]{4} symbols present, (1[0-9][0-9]|[0-9]{1,2}) kept; deserialise [0-9.]+s, dependency scan [0-9.]+s, select [0-9.]+s, link [0-9.]+s$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL
