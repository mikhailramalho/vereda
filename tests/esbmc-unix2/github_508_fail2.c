#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int getPassword()
{
  char buf[4];
  fgets(buf, 5, stdin);
  return strcmp(buf, "SMT");
}

int main()
{
  int x = getPassword();
  if(x)
  {
    printf("Access Denied\n");
    exit(0);
  }
  printf("Access Granted\n");
  return 0;
}
// Imported from ESBMC regression/esbmc-unix2/github_508_fail2
// RUN: --unwind 6
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
