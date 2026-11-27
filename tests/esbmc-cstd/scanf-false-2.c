#include <stdio.h>
int main(int argc, char *argv[])
{
  char reg_name[12];
  printf("Enter your username:");
  scanf("%12s", reg_name); //Buffer overflow here
  printf("The program is now registered to %s.\n", reg_name);
  return 0;
}
// Imported from ESBMC regression/cstd/scanf-false-2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m64
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
