void fun(int a)
{
}

int test1()
{
  char i;
  void (*fp)()=fun;
  // this requires a type conversion for i
  fp(i);
}

int func(int a_decl, int b_decl);
int func2(int a_decl, int b_decl);

int func(int a_def, int b_def) { return a_def+b_def; }
int func2(int a_def, int b_def) { return a_def-b_def; }

void test2()
{
  char a=5;
  int (*fun) ();
    
  if(a)
    fun = func;
  else
    fun = func2;
            
  int b=(*fun) (a, a);
}

int main()
{
  test1();
  test2();
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function_Pointer10
// EXPECT: ^VERIFICATION SUCCESSFUL$
