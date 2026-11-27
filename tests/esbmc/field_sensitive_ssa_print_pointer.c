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

struct pair
{
  int a, b;
};

struct node *alias;
struct container *container;

int __VERIFIER_nondet_int(void);

int main(void)
{
  void *storage = malloc(sizeof(struct node));
  if (!storage)
    return 0;
  alias = storage;
  alias->next = &container->node;
  struct node copy = *(struct node *)storage;
  int x = __VERIFIER_nondet_int();
  struct pair t = {x, 2};
  t = (struct pair){t.b, t.a};
  assert(copy.next == &container->node && t.a == 2 && t.b == x);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_print_pointer
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
