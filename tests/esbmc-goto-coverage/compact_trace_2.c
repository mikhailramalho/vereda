int main()
{
 int a = 1;
 int b = 2;

 if (a>b){}
 else;
}
// Imported from ESBMC regression/goto-coverage/compact_trace_2
// Needs: --branch-coverage
// RUN: --branch-coverage --no-slice
// MODE: FUTURE
// EXPECT: \ba = 1\b
// EXPECT: \bb = 2\b
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
