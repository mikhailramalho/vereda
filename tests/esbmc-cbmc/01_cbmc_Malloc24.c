#include <stdio.h>
//#include <stdlib.h>

static struct rec
{
    int i;
//    float f;
//    char c;
};

int main()
{
    struct rec *p;
    p=(struct rec *) malloc (sizeof(struct rec));
    __VERIFIER_assume(p);
    (*p).i=10;
//    (*p).f=3.14;
//    (*p).c='a';
//    printf("%d %f %c\n",(*p).i,(*p).f,(*p).c);
//    free(p);
    assert((*p).i!=10);
    return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc24
// EXPECT: ^VERIFICATION FAILED$
