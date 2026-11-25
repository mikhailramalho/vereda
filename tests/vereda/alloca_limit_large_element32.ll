; An element larger than the pointer width must fail before its size truncates.
; EXPECT: ^VERIFICATION FAILED$
; EXPECT: FAILED +\[main\.assertion\.1\].*alloca: size exceeds PTRDIFF_MAX
target datalayout = "e-p:32:32"
define i32 @main() {
  %p = alloca [4294967296 x i8]
  store volatile i8 1, ptr %p
  ret i32 0
}
