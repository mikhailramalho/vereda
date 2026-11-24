; ModuleID = 'tests/esbmc/array_loop_summary_vla.c'
source_filename = "tests/esbmc/array_loop_summary_vla.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !10 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca ptr, align 8
  %4 = alloca i64, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 0, ptr %1, align 4
    #dbg_declare(ptr %2, !15, !DIExpression(), !17)
  %7 = call i32 @__VERIFIER_nondet_uint(), !dbg !18
  store i32 %7, ptr %2, align 4, !dbg !17
  %8 = load i32, ptr %2, align 4, !dbg !19
  %9 = icmp ugt i32 %8, 77, !dbg !20
  br i1 %9, label %10, label %13, !dbg !21

10:                                               ; preds = %0
  %11 = load i32, ptr %2, align 4, !dbg !22
  %12 = icmp ult i32 %11, 100, !dbg !23
  br label %13

13:                                               ; preds = %10, %0
  %14 = phi i1 [ false, %0 ], [ %12, %10 ], !dbg !24
  %15 = zext i1 %14 to i32, !dbg !21
  %16 = call i32 (i32, ...) @__VERIFIER_assume(i32 noundef %15), !dbg !25
  %17 = load i32, ptr %2, align 4, !dbg !26
  %18 = zext i32 %17 to i64, !dbg !27
  %19 = call ptr @llvm.stacksave.p0(), !dbg !27
  store ptr %19, ptr %3, align 8, !dbg !27
  %20 = alloca i32, i64 %18, align 16, !dbg !27
  store i64 %18, ptr %4, align 8, !dbg !27
    #dbg_declare(ptr %4, !28, !DIExpression(), !24)
    #dbg_declare(ptr %20, !30, !DIExpression(), !34)
    #dbg_declare(ptr %5, !35, !DIExpression(), !37)
  store i32 0, ptr %5, align 4, !dbg !37
  br label %21, !dbg !38

21:                                               ; preds = %30, %13
  %22 = load i32, ptr %5, align 4, !dbg !39
  %23 = load i32, ptr %2, align 4, !dbg !41
  %24 = icmp ult i32 %22, %23, !dbg !42
  br i1 %24, label %25, label %33, !dbg !43

25:                                               ; preds = %21
  %26 = load i32, ptr %5, align 4, !dbg !44
  %27 = load i32, ptr %5, align 4, !dbg !45
  %28 = zext i32 %27 to i64, !dbg !46
  %29 = getelementptr inbounds nuw i32, ptr %20, i64 %28, !dbg !46
  store i32 %26, ptr %29, align 4, !dbg !47
  br label %30, !dbg !46

30:                                               ; preds = %25
  %31 = load i32, ptr %5, align 4, !dbg !48
  %32 = add i32 %31, 1, !dbg !48
  store i32 %32, ptr %5, align 4, !dbg !48
  br label %21, !dbg !49, !llvm.loop !50

33:                                               ; preds = %21
    #dbg_declare(ptr %6, !52, !DIExpression(), !54)
  store i32 0, ptr %6, align 4, !dbg !54
  br label %34, !dbg !55

34:                                               ; preds = %47, %33
  %35 = load i32, ptr %6, align 4, !dbg !56
  %36 = load i32, ptr %2, align 4, !dbg !58
  %37 = icmp ult i32 %35, %36, !dbg !59
  br i1 %37, label %38, label %50, !dbg !60

38:                                               ; preds = %34
  %39 = load i32, ptr %6, align 4, !dbg !61
  %40 = zext i32 %39 to i64, !dbg !62
  %41 = getelementptr inbounds nuw i32, ptr %20, i64 %40, !dbg !62
  %42 = load i32, ptr %41, align 4, !dbg !62
  %43 = load i32, ptr %6, align 4, !dbg !63
  %44 = icmp eq i32 %42, %43, !dbg !64
  %45 = zext i1 %44 to i32, !dbg !64
  %46 = call i32 (i32, ...) @__VERIFIER_assert(i32 noundef %45), !dbg !65
  br label %47, !dbg !65

47:                                               ; preds = %38
  %48 = load i32, ptr %6, align 4, !dbg !66
  %49 = add i32 %48, 1, !dbg !66
  store i32 %49, ptr %6, align 4, !dbg !66
  br label %34, !dbg !67, !llvm.loop !68

50:                                               ; preds = %34
  %51 = load ptr, ptr %3, align 8, !dbg !70
  call void @llvm.stackrestore.p0(ptr %51), !dbg !70
  %52 = load i32, ptr %1, align 4, !dbg !70
  ret i32 %52, !dbg !70
}

declare i32 @__VERIFIER_nondet_uint() #1

declare i32 @__VERIFIER_assume(...) #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave.p0() #2

declare i32 @__VERIFIER_assert(...) #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore.p0(ptr) #2

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind willreturn }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "Ubuntu clang version 21.1.2 (2ubuntu6)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "tests/esbmc/array_loop_summary_vla.c", directory: "/home/mgadelha/tools/vereda/.claude/worktrees/agent-aa68ffb9039910939", checksumkind: CSK_MD5, checksum: "5164df035a975aa35ea4a3aab4354678")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 8, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 2}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Ubuntu clang version 21.1.2 (2ubuntu6)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 4, type: !11, scopeLine: 5, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "n", scope: !10, file: !1, line: 6, type: !16)
!16 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!17 = !DILocation(line: 6, column: 12, scope: !10)
!18 = !DILocation(line: 6, column: 16, scope: !10)
!19 = !DILocation(line: 7, column: 21, scope: !10)
!20 = !DILocation(line: 7, column: 23, scope: !10)
!21 = !DILocation(line: 7, column: 28, scope: !10)
!22 = !DILocation(line: 7, column: 31, scope: !10)
!23 = !DILocation(line: 7, column: 33, scope: !10)
!24 = !DILocation(line: 0, scope: !10)
!25 = !DILocation(line: 7, column: 3, scope: !10)
!26 = !DILocation(line: 8, column: 9, scope: !10)
!27 = !DILocation(line: 8, column: 3, scope: !10)
!28 = !DILocalVariable(name: "__vla_expr0", scope: !10, type: !29, flags: DIFlagArtificial)
!29 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!30 = !DILocalVariable(name: "a", scope: !10, file: !1, line: 8, type: !31)
!31 = !DICompositeType(tag: DW_TAG_array_type, baseType: !13, elements: !32)
!32 = !{!33}
!33 = !DISubrange(count: !28)
!34 = !DILocation(line: 8, column: 7, scope: !10)
!35 = !DILocalVariable(name: "i", scope: !36, file: !1, line: 9, type: !16)
!36 = distinct !DILexicalBlock(scope: !10, file: !1, line: 9, column: 3)
!37 = !DILocation(line: 9, column: 17, scope: !36)
!38 = !DILocation(line: 9, column: 8, scope: !36)
!39 = !DILocation(line: 9, column: 24, scope: !40)
!40 = distinct !DILexicalBlock(scope: !36, file: !1, line: 9, column: 3)
!41 = !DILocation(line: 9, column: 28, scope: !40)
!42 = !DILocation(line: 9, column: 26, scope: !40)
!43 = !DILocation(line: 9, column: 3, scope: !36)
!44 = !DILocation(line: 10, column: 12, scope: !40)
!45 = !DILocation(line: 10, column: 7, scope: !40)
!46 = !DILocation(line: 10, column: 5, scope: !40)
!47 = !DILocation(line: 10, column: 10, scope: !40)
!48 = !DILocation(line: 9, column: 32, scope: !40)
!49 = !DILocation(line: 9, column: 3, scope: !40)
!50 = distinct !{!50, !43, !51}
!51 = !DILocation(line: 10, column: 12, scope: !36)
!52 = !DILocalVariable(name: "x", scope: !53, file: !1, line: 11, type: !16)
!53 = distinct !DILexicalBlock(scope: !10, file: !1, line: 11, column: 3)
!54 = !DILocation(line: 11, column: 17, scope: !53)
!55 = !DILocation(line: 11, column: 8, scope: !53)
!56 = !DILocation(line: 11, column: 24, scope: !57)
!57 = distinct !DILexicalBlock(scope: !53, file: !1, line: 11, column: 3)
!58 = !DILocation(line: 11, column: 28, scope: !57)
!59 = !DILocation(line: 11, column: 26, scope: !57)
!60 = !DILocation(line: 11, column: 3, scope: !53)
!61 = !DILocation(line: 12, column: 25, scope: !57)
!62 = !DILocation(line: 12, column: 23, scope: !57)
!63 = !DILocation(line: 12, column: 31, scope: !57)
!64 = !DILocation(line: 12, column: 28, scope: !57)
!65 = !DILocation(line: 12, column: 5, scope: !57)
!66 = !DILocation(line: 11, column: 32, scope: !57)
!67 = !DILocation(line: 11, column: 3, scope: !57)
!68 = distinct !{!68, !60, !69}
!69 = !DILocation(line: 12, column: 32, scope: !53)
!70 = !DILocation(line: 13, column: 1, scope: !10)
