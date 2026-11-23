#include <stdio.h>
#include <string.h>

extern void __VERIFIER_assume(_Bool);
extern _Bool __ESBMC_forall(void *, _Bool);
extern _Bool __ESBMC_exists(void *, _Bool);

typedef struct
{
  char name[20];
  int age;
} Person;

int main()
{
  Person people[] = {{"Alice", 25}, {"Bob", 30}, {"Charlie", 22}};
  unsigned n = sizeof(people) / sizeof(people[0]);
  unsigned i;

  // Check if a person named 'Alice' exists
  __VERIFIER_assume(n > 0);
  __VERIFIER_assert(
    __ESBMC_exists(&i, (i < n && strcmp(people[i].name, "Alice") == 0))
); // Alice is missing!

  return 0;
}
// Imported from ESBMC regression/cvc/quantifiers-field
// RUN: --solver cvc5 --incremental-bmc
// MODE: KNOWNBUG
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED
