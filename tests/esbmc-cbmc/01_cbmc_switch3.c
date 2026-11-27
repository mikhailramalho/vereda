int main()
{
  char ch=__VERIFIER_nondet_char();
  
  switch(ch)
  {
  case 'P':
  case 'p':
    assert(ch==80 || ch==112);
  
  default:;
  
  }
}
// Imported from ESBMC regression/cbmc/01_cbmc_switch3
// EXPECT: ^VERIFICATION SUCCESSFUL$
