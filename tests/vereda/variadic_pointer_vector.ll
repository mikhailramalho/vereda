; C cannot write a vector of pointers (clang refuses vector_size on a
; pointer element), so no C ABI says where va_arg would find one: a
; variadic call passing one stays unsupported (#491).
; EXPECT: ^VERIFICATION UNKNOWN: unsupported: variadic call with argument <2 x ptr>
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@a = global i32 1
@b = global i32 2

declare void @llvm.va_start.p0(ptr)
declare void @llvm.va_end.p0(ptr)

define i32 @first(i32 %n, ...) {
  %ap = alloca [24 x i8], align 16
  call void @llvm.va_start.p0(ptr %ap)
  call void @llvm.va_end.p0(ptr %ap)
  ret i32 %n
}

define i32 @main() {
  %v0 = insertelement <2 x ptr> poison, ptr @a, i32 0
  %v = insertelement <2 x ptr> %v0, ptr @b, i32 1
  %r = call i32 (i32, ...) @first(i32 0, <2 x ptr> %v)
  ret i32 %r
}
