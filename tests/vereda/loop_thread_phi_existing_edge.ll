; EXPECT: ^VERIFICATION SUCCESSFUL$
; Threading a trivial i1-phi merge block must not create duplicate pred -> target
; edges when pred already branches directly to target (PR #281 review).
; %other supplies a distinct nonzero value (2) to %res and %res is passed
; directly to __VERIFIER_assert so InstCombine keeps the PHI live past
; threadBoolPhis.
declare i1 @__VERIFIER_nondet_bool()
declare void @__VERIFIER_assert(i32)

define i32 @main() {
entry:
  %c = call i1 @__VERIFIER_nondet_bool()
  br i1 %c, label %mid, label %exit

mid:
  %cond = phi i1 [ false, %entry ], [ %d, %other ]
  br i1 %cond, label %other, label %exit

other:
  %d = call i1 @__VERIFIER_nondet_bool()
  br i1 %d, label %mid, label %exit

exit:
  %res = phi i32 [ 1, %entry ], [ 1, %mid ], [ 2, %other ]
  call void @__VERIFIER_assert(i32 %res)
  ret i32 0
}
