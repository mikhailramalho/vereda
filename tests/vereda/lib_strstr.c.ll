; ModuleID = 'tests/vereda/lib_strstr.c'
source_filename = "tests/vereda/lib_strstr.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"bc\00", align 1, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !17 {
entry:
  %retval = alloca i32, align 4
  %s = alloca [4 x i8], align 1
  %p = alloca ptr, align 8
  store i32 0, ptr %retval, align 4
    #dbg_declare(ptr %s, !22, !DIExpression(), !26)
  %call = call signext i8 @__VERIFIER_nondet_char(), !dbg !27
  store i8 %call, ptr %s, align 1, !dbg !28
  %arrayinit.element = getelementptr inbounds i8, ptr %s, i64 1, !dbg !28
  store i8 98, ptr %arrayinit.element, align 1, !dbg !28
  %arrayinit.element1 = getelementptr inbounds i8, ptr %s, i64 2, !dbg !28
  store i8 99, ptr %arrayinit.element1, align 1, !dbg !28
  %arrayinit.element2 = getelementptr inbounds i8, ptr %s, i64 3, !dbg !28
  store i8 0, ptr %arrayinit.element2, align 1, !dbg !28
    #dbg_declare(ptr %p, !29, !DIExpression(), !31)
  %arraydecay = getelementptr inbounds [4 x i8], ptr %s, i64 0, i64 0, !dbg !32
  %call3 = call ptr @strstr(ptr noundef %arraydecay, ptr noundef @.str) #3, !dbg !33
  store ptr %call3, ptr %p, align 8, !dbg !31
  %0 = load ptr, ptr %p, align 8, !dbg !34
  %arraydecay4 = getelementptr inbounds [4 x i8], ptr %s, i64 0, i64 0, !dbg !35
  %add.ptr = getelementptr inbounds i8, ptr %arraydecay4, i64 1, !dbg !36
  %cmp = icmp eq ptr %0, %add.ptr, !dbg !37
  %conv = zext i1 %cmp to i32, !dbg !37
  call void @__VERIFIER_assert(i32 noundef %conv), !dbg !38
  ret i32 0, !dbg !39
}

declare signext i8 @__VERIFIER_nondet_char() #1

; Function Attrs: nounwind willreturn memory(read)
declare ptr @strstr(ptr noundef, ptr noundef) #2

declare void @__VERIFIER_assert(i32 noundef) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nounwind willreturn memory(read) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nounwind willreturn memory(read) }

!llvm.dbg.cu = !{!7}
!llvm.module.flags = !{!9, !10, !11, !12, !13, !14, !15}
!llvm.ident = !{!16}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 10, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "tests/vereda/lib_strstr.c", directory: "/home/mgadelha/box/work/strstr", checksumkind: CSK_MD5, checksum: "d1b4819d41fdc727bc06401d13b68c5f")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 24, elements: !5)
!4 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!5 = !{!6}
!6 = !DISubrange(count: 3)
!7 = distinct !DICompileUnit(language: DW_LANG_C11, file: !2, producer: "Ubuntu clang version 21.1.8 (6ubuntu1)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !8, splitDebugInlining: false, nameTableKind: None)
!8 = !{!0}
!9 = !{i32 7, !"Dwarf Version", i32 5}
!10 = !{i32 2, !"Debug Info Version", i32 3}
!11 = !{i32 1, !"wchar_size", i32 4}
!12 = !{i32 8, !"PIC Level", i32 2}
!13 = !{i32 7, !"PIE Level", i32 2}
!14 = !{i32 7, !"uwtable", i32 2}
!15 = !{i32 7, !"frame-pointer", i32 2}
!16 = !{!"Ubuntu clang version 21.1.8 (6ubuntu1)"}
!17 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 8, type: !18, scopeLine: 8, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !7, retainedNodes: !21)
!18 = !DISubroutineType(types: !19)
!19 = !{!20}
!20 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!21 = !{}
!22 = !DILocalVariable(name: "s", scope: !17, file: !2, line: 9, type: !23)
!23 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 32, elements: !24)
!24 = !{!25}
!25 = !DISubrange(count: 4)
!26 = !DILocation(line: 9, column: 8, scope: !17)
!27 = !DILocation(line: 9, column: 16, scope: !17)
!28 = !DILocation(line: 9, column: 15, scope: !17)
!29 = !DILocalVariable(name: "p", scope: !17, file: !2, line: 10, type: !30)
!30 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!31 = !DILocation(line: 10, column: 9, scope: !17)
!32 = !DILocation(line: 10, column: 20, scope: !17)
!33 = !DILocation(line: 10, column: 13, scope: !17)
!34 = !DILocation(line: 11, column: 21, scope: !17)
!35 = !DILocation(line: 11, column: 26, scope: !17)
!36 = !DILocation(line: 11, column: 28, scope: !17)
!37 = !DILocation(line: 11, column: 23, scope: !17)
!38 = !DILocation(line: 11, column: 3, scope: !17)
!39 = !DILocation(line: 12, column: 3, scope: !17)
