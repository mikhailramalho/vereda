
int main()
{
int a = -136;
int v = 32;
int dl = (long)v * (long)a;
}
// Imported from ESBMC regression/esbmc/overflow-long
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
