#include <stdio.h>
int main(int argc, char *argv[])
{
  char reg_name[12];
  printf("Enter your username:");
  scanf("%s", reg_name); //Buffer overflow here
  printf("The program is now registered to %s.\n", reg_name);
  return 0;
}
// Imported from ESBMC regression/cstd/scanf-bug
// RUN: --overflow-check -m64
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
