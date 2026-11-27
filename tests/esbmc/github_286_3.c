
int array[10];

// function returns array of numbers
int* getNumbers(void) {
    for (int i = 0; i < 10; ++i) {
       array[i] = i;
    }

    return array;
}

int* getNumbers2(void) {
    int* numbers = getNumbers();
    // numbers2 is local
    int numbers2[10];

    for (int i = 0; i < 10; ++i) {
        numbers2[i] = numbers[i];
    }

    return numbers2;
}

int main(void) {
   int *numbers = getNumbers2();
   numbers[0] = 100;

   return 0;
}
// Imported from ESBMC regression/esbmc/github_286_3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
