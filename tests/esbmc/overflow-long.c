
int main()
{
int a = -136;
int v = 32;
int dl = (long)v * (long)a;
}
// Imported from ESBMC regression/esbmc/overflow-long
// RUN: --overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
