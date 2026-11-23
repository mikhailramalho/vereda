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
// Imported from ESBMC regression/esbmc/40_stack_64_union_false
// Needs: --stack-limit 31
// RUN: --64 --stack-limit 31
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
