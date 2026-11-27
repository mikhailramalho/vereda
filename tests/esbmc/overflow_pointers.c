#include <assert.h>

unsigned int my_strlen(const char* s)
{
  const char *eos = s;
  while (*eos++);
  return (int) (eos - s - 1);
}

int main() 
{
  const char* str = "Test!";
  assert(my_strlen(str) == 5);
} 
// Imported from ESBMC regression/esbmc/overflow_pointers
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --unwind 6
// EXPECT: ^VERIFICATION SUCCESSFUL$
