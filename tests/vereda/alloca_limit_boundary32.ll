; Exactly PTRDIFF_MAX bytes remains valid on ILP32 too.
; EXPECT: ^VERIFICATION SUCCESSFUL$
target datalayout = "e-p:32:32"
define i32 @main() {
  %p = alloca i8, i32 2147483647
  store volatile i8 1, ptr %p
  ret i32 0
}
