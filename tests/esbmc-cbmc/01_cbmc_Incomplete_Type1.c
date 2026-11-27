typedef struct list List;

int main() {
	List l;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Incomplete_Type1
// RUN: --no-incremental-bmc
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: unmet EXPECT: ^ERROR: PARSING ERROR$
// MODE: FUTURE
// Vereda: verdict none
