
#include "file1.h"

 int main()

{

    func1(32);

}
// Imported from ESBMC regression/esbmc/github_902
// FILES: file1.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
