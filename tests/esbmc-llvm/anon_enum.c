int main()
{
  enum { BLAH, FOO };

  int x = BLAH;
  assert(x == 0);

}
// Imported from ESBMC regression/llvm/anon_enum
// EXPECT: ^VERIFICATION SUCCESSFUL$
