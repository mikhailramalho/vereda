struct MixedData
{
  char var1; // i8
  short var2; // i16
  int var3; // i32
  char var4; // i8
};

int main() {
  struct MixedData a; // total: 64
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_struct_false
// Needs: --stack-limit 63
// RUN: -m64 --stack-limit 63
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
