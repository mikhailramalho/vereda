struct teststr {
   int a;
   int b;
   int c;
};

struct teststr str_array[] = {
  { .a = 3 },
  { .b = 4 }
};

int main()
{
  assert(str_array[0].a==3);
  assert(str_array[0].b==0);
  assert(str_array[1].b==4);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Struct_Initialization2
// EXPECT: ^VERIFICATION SUCCESSFUL$
