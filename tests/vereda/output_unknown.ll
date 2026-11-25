; EXPECT: ^\[RESULTS\]\n\nVERIFICATION UNKNOWN: unsupported: call to pthread_create
; EXPECT-NOT: VERIFICATION SUCCESSFUL|\[COUNTEREXAMPLE\]
declare i32 @pthread_create()
define i32 @main() {
  %x = call i32 @pthread_create()
  ret i32 %x
}
