
int main()
{
  int x;
  switch(x)
  {
    case 1:
    case 0:
      return 1;

    case 2:
      break;

    default:
      return 2;
  }

  return 0;
}
// Imported from ESBMC regression/llvm/switch
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
