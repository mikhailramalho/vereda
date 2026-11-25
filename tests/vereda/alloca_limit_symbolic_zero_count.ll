; Only a zero count survives when an element exceeds the pointer width.
; RUN: --no-vla-size-check
; EXPECT: ^VERIFICATION SUCCESSFUL$
target datalayout = "e-p:32:32"
define i32 @main() {
  %n = call i32 @__VERIFIER_nondet_uint()
  %p = alloca [4294967296 x i8], i32 %n
  call void @__vereda_uninitialised(ptr %p)
  %zero = icmp eq i32 %n, 0
  call void @__VERIFIER_assert(i1 %zero)
  ret i32 0
}
declare i32 @__VERIFIER_nondet_uint()
declare void @__VERIFIER_assert(i1)
declare void @__vereda_uninitialised(ptr)
