struct InnerStruct
{
   char innerValue[10];
};

int main()
{
   struct InnerStruct array[2];
   strcpy(array[0].innerValue, "20");
   char *ptr;
   assert(strtol(array[0].innerValue, ptr, 10) == 40);
}
// Imported from ESBMC regression/esbmc/github_1408-1
// Needs: --no-bound
// RUN: --unwind 3 --ub-pointer-check=false --no-bound --ub-div-by-zero-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
