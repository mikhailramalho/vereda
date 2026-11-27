#include <stdio.h>
//#include <stdlib.h>

#define ACCTS 5

typedef struct Account {
    int name;
    double amount;
} Account;

static Account *accounts[ACCTS];

int main()
{
  accounts[0] = (Account *) malloc(sizeof(Account));
  __VERIFIER_assume(accounts[0]);
  accounts[0]->name=1;
  assert(accounts[0]->name!=1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc25
// EXPECT: ^VERIFICATION FAILED$
