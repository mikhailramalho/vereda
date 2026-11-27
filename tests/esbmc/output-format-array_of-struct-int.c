
struct { int x, y; } A[2];

int main()
{
	assert(!A);
}
// Imported from ESBMC regression/esbmc/output-format-array_of-struct-int
// Needs: --witness-output-graphml
// RUN: --witness-output-graphml - --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bA = \{ \{ \.x\s*=\s*0 \} \}
// EXPECT: (?s)\A(?!.*key="assumption">[^<]*[{}])
// MODE: FUTURE
// Vereda: verdict none
