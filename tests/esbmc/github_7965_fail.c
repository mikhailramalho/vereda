// esbmc/esbmc#7965: a pointer into a NULL container, stored in malloc'd storage
// and read back through void *, must still be NULL-based when followed.
#include <assert.h>
#include <stdlib.h>

struct node
{
  struct node *next;
};

struct container
{
  int header;
  struct node node;
};

struct node *alias;
struct container *container;

int main(void)
{
  void *storage = malloc(8);
  alias = storage;
  alias->next = &container->node;
  struct node copy = *(struct node *)storage;
  (void)copy.next->next;
  assert(0);
}
// Imported from ESBMC regression/esbmc/github_7965_fail
// RUN: -m64 --incremental-bmc --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.null-pointer-dereference\.\d+\] +line +26 +dereference failure: NULL pointer$
// Vereda: unmet EXPECT: ^  FAILED +\[main\.null-pointer-dereference\.\d+\] +line +26 +dereference failure: NULL pointer$
// MODE: FUTURE
// Vereda: verdict FAILED
