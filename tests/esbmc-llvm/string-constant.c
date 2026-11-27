int main()
{
  char* str = "blah";
}
// Imported from ESBMC regression/llvm/string-constant
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
