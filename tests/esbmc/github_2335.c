#include <stdlib.h>

struct s {
  int datum;
  struct s *next;
};

struct s *slot[2];

struct s *new(int x) {
  struct s *p = malloc(sizeof(struct s));
  p->datum = x;
  p->next = ((void *)0);
  return p;
}
void list_add(struct s *node, struct s *list) {
  struct s *temp = list->next;
  list->next = node;
  node->next = temp;
}
int main () {
  slot[0] = new(1);
  list_add(new(2), slot[0]);
  slot[1] = new(1);
  list_add(new(2), slot[1]);
  list_add(new(3), slot[1]);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2335
// Needs: --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --malloc-zero-is-null
// RUN: --no-div-by-zero-check --force-malloc-success --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check  --memory-leak-check --no-reachable-memory-leak --no-abnormal-memory-leak --malloc-zero-is-null
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
