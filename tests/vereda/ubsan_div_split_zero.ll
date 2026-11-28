; The combined division trap of ubsan_div_split_overflow, built from
; ubsan_div_zero_only.c: (a & 255) / b, which never overflows. With
; --ub-signed-overflow-check=false only the divisor's part reaches the trap,
; and it is violated (#446): a division by zero (#514).
; RUN: --ub-signed-overflow-check=false
; EXPECT: ^  FAILED +\[main\.division-by-zero\.1\]  line 0  division by zero$
; EXPECT: ^VERIFICATION FAILED$
; ModuleID = 'ubsan_div_zero_only.c'
source_filename = "ubsan_div_zero_only.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.src = private unnamed_addr constant [22 x i8] c"ubsan_div_zero_only.c\00", align 1
@0 = private unnamed_addr constant { i16, i16, [6 x i8] } { i16 0, i16 11, [6 x i8] c"'int'\00" }

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %4 = call i32 @__VERIFIER_nondet_int()
  %5 = and i32 %4, 255
  store i32 %5, ptr %2, align 4
  %6 = call i32 @__VERIFIER_nondet_int()
  store i32 %6, ptr %3, align 4
  %7 = load i32, ptr %2, align 4
  %8 = load i32, ptr %3, align 4
  %9 = icmp ne i32 %8, 0, !nosanitize !5
  %10 = icmp ne i32 %7, -2147483648, !nosanitize !5
  %11 = icmp ne i32 %8, -1, !nosanitize !5
  %12 = or i1 %10, %11, !nosanitize !5
  %13 = and i1 %9, %12, !nosanitize !5
  br i1 %13, label %15, label %14, !prof !6, !nosanitize !5

14:                                               ; preds = %0
  call void @llvm.ubsantrap(i8 3) #3, !nosanitize !5
  unreachable, !nosanitize !5

15:                                               ; preds = %0
  %16 = sdiv i32 %7, %8
  ret i32 %16
}

declare i32 @__VERIFIER_nondet_int() #1

; Function Attrs: cold noreturn nounwind memory(inaccessiblemem: write)
declare void @llvm.ubsantrap(i8 immarg) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { cold noreturn nounwind memory(inaccessiblemem: write) }
attributes #3 = { nomerge noreturn nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 8, !"PIC Level", i32 2}
!1 = !{i32 7, !"PIE Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 2}
!4 = !{!"clang version 23.1.6 (https://github.com/llvm/llvm-project 5b10d2d74929eae78912771222c9855af97496f4)"}
!5 = !{}
!6 = !{!"branch_weights", i32 1048575, i32 1}
