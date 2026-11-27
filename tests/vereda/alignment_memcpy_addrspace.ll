; A struct copy through a byte offset into a global outside address space
; 0 is not checked for alignment, as loads and stores there are not.
; RUN: --ub-alignment-check
; EXPECT: ^VERIFICATION SUCCESSFUL$
%struct.N = type { i32 }

@buf = addrspace(1) global [8 x i8] zeroinitializer, align 4

define i32 @main() {
  %r = alloca %struct.N, align 4
  %p = getelementptr inbounds i8, ptr addrspace(1) @buf, i64 1
  call void @llvm.memcpy.p0.p1.i64(ptr align 4 %r, ptr addrspace(1) align 1 %p, i64 4, i1 false)
  ret i32 0
}

declare void @llvm.memcpy.p0.p1.i64(ptr, ptr addrspace(1), i64, i1)

!llvm.ident = !{!0}
!0 = !{!"clang version 23.1.6"}
