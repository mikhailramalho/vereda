; Four-byte elements overflow the pointer width; the unwrapped size is checked.
; EXPECT: ^VERIFICATION FAILED$
; EXPECT: FAILED +\[main\.assertion\.1\].*alloca: size exceeds PTRDIFF_MAX
target datalayout = "e-p:32:32"
define i32 @main() {
  %p = alloca i32, i32 1073741825
  store volatile i32 1, ptr %p
  ret i32 0
}
