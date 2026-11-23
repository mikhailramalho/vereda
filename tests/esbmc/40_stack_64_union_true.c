union MixedData
{
  char var1; // i8
  short var2; // i16
  int var3; // i32
  char var4; // i8
};

int main() {
  union MixedData a; // total: 32
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_union_true
// Needs: --stack-limit 32
// RUN: --64 --stack-limit 32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
