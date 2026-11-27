; An empty or null !llvm.ident entry names no producer: the alignment check
; reads the access's own alignment, and a misaligned one still fails.
; RUN: --ub-alignment-check
; EXPECT: ^  dereference failure: Incorrect alignment when accessing data object$
; EXPECT: ^VERIFICATION FAILED$
define i32 @main() {
  %a = alloca [8 x i8], align 8
  %p = getelementptr i8, ptr %a, i64 2
  %v = load i32, ptr %p, align 4
  ret i32 %v
}

!llvm.ident = !{!0, !1}
!0 = !{}
!1 = !{null}
