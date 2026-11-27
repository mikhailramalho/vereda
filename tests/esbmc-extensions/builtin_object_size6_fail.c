#include <assert.h>
#include <string.h>

int main() {
    char buffer[10];
    size_t buf_size = __builtin_object_size(buffer, 0);
    
    assert(buf_size == 10);
    
    if (buf_size <= 15) 
         memset(buffer, 'B', 15);  // Buffer overflow
    
    return 0;
}
// Imported from ESBMC regression/extensions/builtin_object_size6_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
