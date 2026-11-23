int more();

int main()
{
  int n = 0;
  while (more())
    n++;
  return n;
}
// Imported from ESBMC regression/esbmc/loop_local_l1_reuse
// Needs: --program-only
// RUN: --unwind 3 --no-unwinding-assertions --program-only
// MODE: FUTURE
// EXPECT: return_value\$_more\$1\?1!
// EXPECT: \A(?![\s\S]*return_value\$_more\$1\?(?!1!)\d)
// Vereda: verdict none
