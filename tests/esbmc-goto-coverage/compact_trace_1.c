int main()
{
 int a = 1;
 int b = 2;

 if (a>b){}
 else;
}
// Imported from ESBMC regression/goto-coverage/compact_trace_1
// Needs: --branch-coverage
// RUN: --branch-coverage
// EXPECT: \ba = 1\b
// EXPECT: \bb = 2\b
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// MODE: FUTURE
// Vereda: verdict none
