int nums[2]; 
int *p; 

int main() { 
        nums[1] = 1; 
        p = &nums[0]; 
        p++; 

        assert(*p == 1); 
} 
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
