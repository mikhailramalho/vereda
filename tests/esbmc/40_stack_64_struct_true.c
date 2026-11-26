struct MixedData
{
  char var1; // i8
  short var2; // i16
  int var3; // i32
  char var4; // i8
};

int main() {
  struct MixedData a; // total: 96
  assert(sizeof(a) == 12);
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_struct_true
// Needs: --stack-limit 96
// RUN: -m64 --stack-limit 96 -Wno-error=implicit-function-declaration
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
