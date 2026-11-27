struct foo1
{
  struct
  {
    int x;
  };
  
  int y;
}; 

union foo2
{
  struct
  {
    int x;
  };
  
  int y;
}; 

int main()
{
  struct foo1 s;
  union foo2 u;
  
  s.y=1;
  s.x=2;
  
  u.y=1;
  u.x=2;

  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Structs5
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
