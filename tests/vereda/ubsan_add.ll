; A clang sanitizer check, compiled from ubsan_add.c with
; -fsanitize=signed-integer-overflow -fsanitize-trap=all: the trap is a
; failed check named after its kind.
; EXPECT: ^Violated property:\n  file ubsan_add\.c line 2 column 12 function add\n  arithmetic overflow on add$
; EXPECT: ^VERIFICATION FAILED$
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.src = private unnamed_addr constant [12 x i8] c"ubsan_add.c\00", align 1
@0 = private unnamed_addr constant { i16, i16, [6 x i8] } { i16 0, i16 11, [6 x i8] c"'int'\00" }

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @add(i32 noundef %0, i32 noundef %1) #0 !dbg !10 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
    #dbg_declare(ptr %3, !15, !DIExpression(), !16)
  store i32 %1, ptr %4, align 4
    #dbg_declare(ptr %4, !17, !DIExpression(), !18)
  %5 = load i32, ptr %3, align 4, !dbg !19
  %6 = load i32, ptr %4, align 4, !dbg !20
  %7 = call { i32, i1 } @llvm.sadd.with.overflow.i32(i32 %5, i32 %6), !dbg !21, !nosanitize !14
  %8 = extractvalue { i32, i1 } %7, 0, !dbg !21, !nosanitize !14
  %9 = extractvalue { i32, i1 } %7, 1, !dbg !21, !nosanitize !14
  %10 = xor i1 %9, true, !dbg !21, !nosanitize !14
  br i1 %10, label %12, label %11, !dbg !21, !prof !22, !nosanitize !14

11:                                               ; preds = %2
  call void @llvm.ubsantrap(i8 0) #4, !dbg !21, !nosanitize !14
  unreachable, !dbg !21, !nosanitize !14

12:                                               ; preds = %2
  ret i32 %8, !dbg !23
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare { i32, i1 } @llvm.sadd.with.overflow.i32(i32, i32) #1

; Function Attrs: cold noreturn nounwind
declare void @llvm.ubsantrap(i8 immarg) #2

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 !dbg !24 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = call i32 @__VERIFIER_nondet_int(), !dbg !27
  %3 = call i32 @add(i32 noundef %2, i32 noundef 1), !dbg !28
  ret i32 %3, !dbg !29
}

declare i32 @__VERIFIER_nondet_int() #3

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { cold noreturn nounwind }
attributes #3 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nomerge noreturn nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 21.1.8 (https://github.com/llvm/llvm-project 2078da43e25a4623cab2d0d60decddf709aaea28)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "ubsan_add.c", directory: ".", checksumkind: CSK_MD5, checksum: "6aef5f8cb22ca02285728e63176981da")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"clang version 21.1.8 (https://github.com/llvm/llvm-project 2078da43e25a4623cab2d0d60decddf709aaea28)"}
!10 = distinct !DISubprogram(name: "add", scope: !1, file: !1, line: 1, type: !11, scopeLine: 1, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !13, !13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "a", arg: 1, scope: !10, file: !1, line: 1, type: !13)
!16 = !DILocation(line: 1, column: 13, scope: !10)
!17 = !DILocalVariable(name: "b", arg: 2, scope: !10, file: !1, line: 1, type: !13)
!18 = !DILocation(line: 1, column: 20, scope: !10)
!19 = !DILocation(line: 2, column: 10, scope: !10)
!20 = !DILocation(line: 2, column: 14, scope: !10)
!21 = !DILocation(line: 2, column: 12, scope: !10)
!22 = !{!"branch_weights", i32 1048575, i32 1}
!23 = !DILocation(line: 2, column: 3, scope: !10)
!24 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 5, type: !25, scopeLine: 5, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0)
!25 = !DISubroutineType(types: !26)
!26 = !{!13}
!27 = !DILocation(line: 7, column: 14, scope: !24)
!28 = !DILocation(line: 7, column: 10, scope: !24)
!29 = !DILocation(line: 7, column: 3, scope: !24)
