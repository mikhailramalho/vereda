int more();

int main()
{
  int n = 0;
  do
    n++;
  while (more());
  return n;
}
// Imported from ESBMC regression/esbmc/do_while_condition_l1_reuse
// Needs: --program-only
// RUN: --unwind 3 --no-unwinding-assertions --program-only
// MODE: FUTURE
// EXPECT: return_value\$_more\$1\?1!
// EXPECT: \A(?![\s\S]*return_value\$_more\$1\?(?!1!)\d)
// Vereda: verdict none
