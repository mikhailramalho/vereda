#include <string.h>

int main()
{
  char a[100];
  const char *p;
  unsigned int i;

  // this should work
  p=strerror(1);

  // this should work
  i=strlen(p);

  // this sould work
  if(i<sizeof(a))
    strcpy(a, p);
    
  // and even this should work
  char ch;
  ch=*p;
}
// Imported from ESBMC regression/cbmc/01_cbmc_String_Abstraction5
// RUN: --unwind 102
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
