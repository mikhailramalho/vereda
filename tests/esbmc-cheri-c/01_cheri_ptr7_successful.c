#include <assert.h>
#include <cheri/cheric.h>

char array[2];

int main(int argc, char *argv[])
{
  char *__capability arrayp;
  int ii;
  arrayp = cheri_ptr(array, sizeof(array));
  for(ii = 0; ii < sizeof(array); ii++)
  {
    assert(arrayp[ii] == 0);
  }
  return 0;
}
// Imported from ESBMC regression/cheri-c/01_cheri_ptr7_successful
// Needs: --cheri hybrid
// RUN: --cheri hybrid
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
