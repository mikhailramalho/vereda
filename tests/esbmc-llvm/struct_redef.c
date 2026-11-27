struct X;

struct X {
  int a;
};

union Y;

union Y {
  int a;
  int b;
};

enum Z;

enum Z { A, B };

int main()
{
}
// Imported from ESBMC regression/llvm/struct_redef
// EXPECT: ^VERIFICATION SUCCESSFUL$
