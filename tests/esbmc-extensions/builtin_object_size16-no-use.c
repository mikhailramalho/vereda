char global_buffer[64];

int main()
{
  __builtin_object_size(global_buffer, 0);
  return 0;
}
// Imported from ESBMC regression/extensions/builtin_object_size16-no-use
// EXPECT: ^VERIFICATION SUCCESSFUL$
