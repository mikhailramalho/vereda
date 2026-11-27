#include <stdlib.h>
#include <stddef.h>

struct my_item {
    int data;
    struct { void *next; } link;
};

int main() {
    struct my_item *ptr = NULL;
    
    // This should fail - accessing member of NULL pointer
    void *member_ptr = &ptr->link;
    void *tmp = member_ptr - offsetof(struct my_item, link);
    struct my_item *recovered = (struct my_item*)tmp;
    
    int bad_access = recovered->data; // Should fail verification
    
    return 0;
}
// Imported from ESBMC regression/esbmc/github_2512_5
// RUN: --force-malloc-success --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
