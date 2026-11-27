typedef struct list List;

struct list {
	int x;
	int y;
};

int main() {
}
// Imported from ESBMC regression/cbmc/01_cbmc_Forward_Declaration1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
