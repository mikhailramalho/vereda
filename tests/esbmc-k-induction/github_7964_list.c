// #7964: the inductive step must not pin a list cursor to its loop-entry node.
struct node { int v; struct node *next; };
struct node n3 = {0, 0};
struct node n2 = {1, &n3};
struct node n1 = {1, &n2};
int main()
{
  struct node *p = &n1;
  while (p)
  {
    __VERIFIER_assert(p->v == 1); // every node holds 1
    p = p->next;
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/github_7964_list
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
