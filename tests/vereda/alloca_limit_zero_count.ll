; A zero count is valid even when the element itself exceeds the address space.
; EXPECT: ^VERIFICATION SUCCESSFUL$
target datalayout = "e-p:32:32"
define i32 @main() {
  %p = alloca [4294967296 x i8], i32 0
  call void @__vereda_uninitialised(ptr %p)
  ret i32 0
}
declare void @__vereda_uninitialised(ptr)
