typedef union {
	int x[12000];
} t1;

typedef struct {
	int x;
	t1 y;
} t2;

int main()
{
	t2 a;
	int *b = &a.x;
	b[1] = a.x;
	assert(*b);
}
// Imported from ESBMC regression/esbmc/github_732-2-2
// RUN: --no-incremental-bmc
// MODE: THOROUGH
// EXPECT: ^VERIFICATION FAILED$
