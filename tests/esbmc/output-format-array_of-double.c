
double A[2];

int main()
{
	assert(!A);
}
// Imported from ESBMC regression/esbmc/output-format-array_of-double
// Needs: --witness-output-graphml - --compact-trace
// RUN: --witness-output-graphml - --compact-trace -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bA = \{ 0\.000000 \}
// EXPECT: (?s)\A(?!.*key="assumption">[^<]*[{}])
// Vereda: verdict none
