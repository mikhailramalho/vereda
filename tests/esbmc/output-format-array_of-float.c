
float A[2];

int main()
{
	assert(!A);
}
// Imported from ESBMC regression/esbmc/output-format-array_of-float
// Needs: --witness-output-graphml -
// RUN: --witness-output-graphml -
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bA = \{ 0\.000000f \}
// EXPECT: (?s)\A(?!.*key="assumption">[^<]*[{}])
// MODE: FUTURE
// Vereda: verdict none
