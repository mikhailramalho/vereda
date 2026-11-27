typedef struct {
    int field;
} MyStruct;


void f(MyStruct *s)
{
    int y = s->field;
}

int main() {
	MyStruct s;

	f(&s);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Anonymous_Struct1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
