// Data-only reads through a choice leave provenance available to loads and copies.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
struct Item { int *pointer; double value; };
int main(void) {
  int x = 11, y = 22;
  struct Item a = {&x, 2.0}, b = {&y, 4.0};
  int choice = __VERIFIER_nondet_int();
  struct Item *selected = choice ? &a : &b;
  volatile unsigned char byte = *(unsigned char *)selected;
  volatile double number = selected->value;
  __VERIFIER_assert(number == (choice ? 2.0 : 4.0));
  int *copied;
  memcpy(&copied, &selected->pointer, sizeof(copied));
  __VERIFIER_assert(copied == (choice ? &x : &y));
  __VERIFIER_assert(*selected->pointer == (choice ? 11 : 22));
  *copied = 37;
  __VERIFIER_assert(choice ? x == 37 && y == 22 : y == 37 && x == 11);
}
