typedef struct list List;

int main() {
	List l;
}

struct list {
	int x;
};
// Imported from ESBMC regression/cbmc/01_cbmc_Forward_Declaration2
// RUN: --no-incremental-bmc
// EXPECT: ^ERROR: PARSING ERROR$
// Vereda: unmet EXPECT: ^ERROR: PARSING ERROR$
// MODE: FUTURE
// Vereda: verdict none
