struct T;

struct S {
	struct T *t;
};

struct T {
	struct S s;
};

int main()
{
	struct T t;
	t.s.t = &t;
}
// Imported from ESBMC regression/esbmc/github_637-1_decl
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
