typedef struct list {
	struct list *next;
	int data;
} List;

int main() {
}
// Imported from ESBMC regression/cbmc/01_cbmc_Recursive_Structure1
// EXPECT: ^VERIFICATION SUCCESSFUL$
