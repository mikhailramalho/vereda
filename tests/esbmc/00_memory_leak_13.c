
struct nodet {
  struct nodet *n;
  int payload;
};

int main() {
  unsigned i;
  struct nodet *list=(void *)0;
  struct nodet *new_node;
  
  for(i=0; i<2; i++) {
    new_node=malloc(sizeof(*new_node));
    __VERIFIER_assume(new_node);
    new_node->n=list;
    new_node->payload=i;
    list=new_node;
  }

  assert(new_node->payload==1);
}
// Imported from ESBMC regression/esbmc/00_memory_leak_13
// Needs: --memory-leak-check --claim 1 --no-reachable-memory-leak --no-abnormal-memory-leak
// RUN: --memory-leak-check --claim 1 --no-reachable-memory-leak --no-abnormal-memory-leak
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
