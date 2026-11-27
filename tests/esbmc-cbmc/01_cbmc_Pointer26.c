#define NULL 0

struct list_el {
   int val;
   struct list_el *next;
};

typedef struct list_el item;

int main()
{
  item *curr, *nextt;

  nextt = NULL;
  curr->val=1;
  curr->next = nextt;

  assert(curr->val==2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer26
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
