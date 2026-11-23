#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
// Imported from ESBMC regression/esbmc/00_memory_leak_03
// Needs: --compact-trace --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --compact-trace --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
