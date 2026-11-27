static int top=0;

int get_top(void)
{
  return top;
}

int main(void) 
{
  if (get_top()==0) 
  {
    return 0;
  } 

  assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/01_pthread44
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
