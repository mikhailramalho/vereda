#include <pthread.h>
struct S
{
  int a[3];
} s;
struct S *p = &s;
int r;
void *t(void *arg)
{
  p->a[1] = 1;
  return 0;
}
int main(void)
{
  pthread_t id;
  pthread_create(&id, 0, t, 0);
  r = s.a[1];
  pthread_join(id, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9688_race_witness_member_fail
// Needs: --data-races-check --no-assertions --witness-output-yaml -
// RUN: --data-races-check --no-assertions --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: (?s)segment:\s+- waypoint:\s+type: "target"[^\n]*\n[^\n]*\n\s+location:[^\n]*\n[^\n]*\n\s+line: (?:10|17)\b.*?- waypoint:\s+type: "target"[^\n]*\n[^\n]*\n\s+location:[^\n]*\n[^\n]*\n\s+line: (?:10|17)\b
// Vereda: verdict none
