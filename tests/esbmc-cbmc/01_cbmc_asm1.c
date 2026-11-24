// this is a GCC extension
char *strerror(int) __asm("_" "strerror" "$UNIX2003");

int main()
{
  __asm("mov ax, dx");
}
// Imported from ESBMC regression/cbmc/01_cbmc_asm1
// EXPECT: ^VERIFICATION SUCCESSFUL$
