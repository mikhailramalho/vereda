; EXPECT: ^VERIFICATION SUCCESSFUL$
; Threading a trivial i1-phi merge block must not create duplicate pred -> target
; edges when pred already branches directly to target (PR #281 review).
declare i1 @__VERIFIER_nondet_bool()
declare void @__VERIFIER_assert(i32)

define i32 @main() {
entry:
  %c = call i1 @__VERIFIER_nondet_bool()
  br i1 %c, label %mid, label %exit

mid:
  %cond = phi i1 [ false, %entry ], [ true, %other ]
  br i1 %cond, label %other, label %exit

other:
  br label %mid

exit:
  %res = phi i32 [ 1, %entry ], [ 1, %mid ]
  call void @__VERIFIER_assert(i32 %res)
  ret i32 0
}
