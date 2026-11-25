; EXPECT: ^\[INFO\] Target:
; EXPECT: ^\[INFO\] IR preparation time:
; EXPECT: ^\[INFO\] Walk time:
; EXPECT: ^\[INFO\] Solving time:
; EXPECT: ^\[RESULTS\]\n\nProperties: 0 checked, 0 passed, 0 failed, 0 not checked\n\nVERIFICATION SUCCESSFUL$
; EXPECT-NOT: \[COUNTEREXAMPLE\]
define i32 @main() {
  ret i32 0
}
