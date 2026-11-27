#include <stdio.h>

typedef struct {
  int id;
  int version;
} block;

typedef struct blocknode {
  block* b;
  struct blocknode* next;
} blocknode;

typedef blocknode* bl;

int main () {
  block* bp = (block*)(malloc(sizeof(block)));
  __VERIFIER_assume(bp);
  bl l = (bl)(malloc(sizeof(blocknode)));
  __VERIFIER_assume(l);
  bp->version = 1;
  l->b = bp;
  l->b->version = l->b->version+1;
  assert ((l->b->version) == 1);
}
// Imported from ESBMC regression/nonz3/01_cbmc_Pointer14
// EXPECT: ^VERIFICATION FAILED$
