union U;

struct S {
	union U *u;
};

union U {
	struct S s;
};

int main()
{
	union U u;
	u.s.u = &u;
}
// Imported from ESBMC regression/esbmc/github_637-2_decl
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
