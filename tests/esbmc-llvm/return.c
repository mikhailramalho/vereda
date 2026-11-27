int main()
{
  int x = 'c';
  assert(x==99);
  return 0;
}
// Imported from ESBMC regression/llvm/return
// EXPECT: ^VERIFICATION SUCCESSFUL$
