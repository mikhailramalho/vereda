// A pointer from outside the program may point to an object whose address
// the program gave out (#137), here to a call without a body.
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern void register_counter(int *);
extern int *get_counter(void);

int counter;

int main(void) {
  register_counter(&counter);
  int *p = get_counter();
  if (p)
    *p = 1;
  __VERIFIER_assert(counter == 0);
  return 0;
}
