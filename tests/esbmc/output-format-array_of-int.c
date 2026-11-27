
int A[2];

int main()
{
	assert(!A);
}
// Imported from ESBMC regression/esbmc/output-format-array_of-int
// Needs: --witness-output-graphml
// RUN: --witness-output-graphml - --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bA = \{ 0 \}
// MODE: FUTURE
// Vereda: verdict none
