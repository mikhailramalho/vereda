struct X
{
  struct
  {
    int i;
  };

  struct
  {
    int j;
  };
};

struct Y
{
  struct
  {
    int i;
  };

  struct
  {
    int j;
  };
};

int main()
{
  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Anonymous_Struct2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
