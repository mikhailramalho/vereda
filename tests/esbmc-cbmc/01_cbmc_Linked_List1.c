
struct nodet {
  struct nodet *n;
  int payload;
};

int main() {
  unsigned i;
  struct nodet *list=(void *)0;
  struct nodet *new_node;
  
  for(i=0; i<10; i++) {
    new_node=malloc(sizeof(*new_node));
    __VERIFIER_assume(new_node);
    new_node->n=list;
    list=new_node;
  }
}
// Imported from ESBMC regression/cbmc/01_cbmc_Linked_List1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
