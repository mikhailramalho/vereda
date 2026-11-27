#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char name[20] = "";

  if (argc > 1 && argv[1] != NULL)
  {
    memcpy(name, argv[1], 30);
  }
}
// Imported from ESBMC regression/esbmc/github_2312_6
// RUN: --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
