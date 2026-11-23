// Building the counterexample read the handler back from the table and then
// queried the function itself, aborting with "Unimplemented type'd expression
// (5) in smt get".
int __VERIFIER_nondet_int(void);

void (*handlers[6])(int);

void handler(int sig)
{
  (void)sig;
}

int main(void)
{
  int installed = __VERIFIER_nondet_int();
  __VERIFIER_assume(installed >= 0 && installed < 6);
  handlers[installed] = handler;

  int raised = __VERIFIER_nondet_int();
  __VERIFIER_assume(raised >= 0 && raised < 6);
  void (*resolved)(int) = handlers[raised];
  __VERIFIER_assert(resolved == 0); // no handler installed for the raised slot
  return 0;
}
// Imported from ESBMC regression/esbmc/trace_function_pointer_array_element
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  resolved = &handler$
// Vereda: unmet EXPECT: ^  resolved = &handler$
// MODE: FUTURE
// Vereda: verdict FAILED
