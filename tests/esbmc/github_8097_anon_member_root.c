/* #8097: the value-set suffix of g.list.next runs through the anonymous union
 * member, named "node::(anonymous at main.c:...)", whose name holds a '.'. */
#include <stdlib.h>

struct node
{
  int kind;
  union
  {
    struct
    {
      struct node *next;
    } list;
    int spins;
  };
};

struct node g;

int main()
{
  g.list.next = malloc(sizeof(struct node));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8097_anon_member_root
// Needs: --memory-leak-check --no-reachable-memory-leak
// RUN: --memory-leak-check --no-reachable-memory-leak --force-malloc-success --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
