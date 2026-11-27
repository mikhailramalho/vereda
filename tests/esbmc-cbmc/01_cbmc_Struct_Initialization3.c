typedef struct
{
 int a;
} S;
 
int main(void)
{
  S s;
  S *var1=&s;
  S var2 = *var1;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Struct_Initialization3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
