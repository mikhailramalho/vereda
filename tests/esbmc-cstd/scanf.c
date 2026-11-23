#include <stdio.h>
int main(int argc, char *argv[])
{
  char reg_name[12];
  printf("Enter your username:");
  scanf("%11s", reg_name); 
  printf("The program is now registered to %s.\n", reg_name);
  return 0;
}
// Imported from ESBMC regression/cstd/scanf
// RUN: --overflow-check -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
