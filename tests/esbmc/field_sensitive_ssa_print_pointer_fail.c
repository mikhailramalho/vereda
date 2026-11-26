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
  void *storage = malloc(8);
  alias = storage;
  alias->next = &container->node;
  struct node copy = *(struct node *)storage;
  int x = __VERIFIER_nondet_int();
  struct pair t = {x, 2};
  t = (struct pair){t.b, t.a};
  if (t.b == 5)
    (void)copy.next->next;
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_print_pointer_fail
// RUN: -m64 --force-malloc-success
// MODE: FUTURE
// EXPECT: ^  copy = \{ \.next=\( struct node \*\)8 \}$
// EXPECT: ^  t = \{ \.a=2, \.b=5 \}$
// EXPECT: ^\s*FAILED\s+\[main\.null-pointer-dereference\.\d+\]\s+line 33 .*NULL pointer$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL
