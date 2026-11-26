// RUN: --std=c11 -DEXPECT_STD=201112L -DEXPECT_STRICT=1
// RUN: --std gnu17 -DEXPECT_STD=201710L -DEXPECT_STRICT=0
// RUN: -std=c23 -DEXPECT_STD=202311L -DEXPECT_STRICT=1
// RUN: --std=gnu89 -DEXPECT_STD=0 -DEXPECT_STRICT=0
// EXPECT: ^VERIFICATION SUCCESSFUL$
#ifndef __STDC_VERSION__
#define __STDC_VERSION__ 0
#endif
#if __STDC_VERSION__ != EXPECT_STD
#error Wrong C standard
#endif
#ifdef __STRICT_ANSI__
#define STRICT 1
#else
#define STRICT 0
#endif
#if STRICT != EXPECT_STRICT
#error Wrong GNU extension mode
#endif
int main(void) { return 0; }
