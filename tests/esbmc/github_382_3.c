void foo(unsigned int *p){
    *p = 5; // p is pointing to address 0X06
}

int main() {
   int x = 6;
   unsigned int *p = (unsigned int *)x;
   foo(p);
   return 0; 
}

// Imported from ESBMC regression/esbmc/github_382_3
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
