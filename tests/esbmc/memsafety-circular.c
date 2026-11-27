
static struct item {
	struct item *next;
} *A;

int main()
{
	A = malloc(sizeof(*A));
	A->next = A;
}
// Imported from ESBMC regression/esbmc/memsafety-circular
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --force-malloc-success --memory-leak-check --no-reachable-memory-leak --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
