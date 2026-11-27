union u_type
{
  int i;
  char ch;
};

int main() {
  union u_type u[2];
  
  u[0].i=1;
  assert(u[0].i==1);
  
  u[1].ch=2;
  assert(u[1].ch==2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_union4
// EXPECT: ^VERIFICATION SUCCESSFUL$
