// Under a leak check the step has to model the nodes earlier iterations
// allocated, so the loop's nodes are not left unhavoced: that would let the
// inductive step prove that the two frees release every node. The step is
// disabled and the base case finds the leak.
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);

struct node
{
  struct node *next;
};

int main()
{
  struct node *head = 0;
  while (__VERIFIER_nondet_int())
  {
    struct node *n = malloc(sizeof *n);
    __VERIFIER_assume(n);
    n->next = head;
    head = n;
  }
  if (head)
  {
    struct node *s = head->next;
    free(head);
    free(s);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_fresh_leak_fail
// Needs: --k-induction --memory-leak-check
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check --memory-leak-check --no-assertions
// MODE: FUTURE
// EXPECT: Disabling inductive step
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
