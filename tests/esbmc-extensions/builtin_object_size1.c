#include <assert.h>

int main() {
    char buffer[100];
    int numbers[10];
    
    assert(__builtin_object_size(buffer, 0) == 100);
    assert(__builtin_object_size(numbers, 0) == 40); // 10 * sizeof(int)
    
    return 0;
}
// Imported from ESBMC regression/extensions/builtin_object_size1
// EXPECT: ^VERIFICATION SUCCESSFUL$
