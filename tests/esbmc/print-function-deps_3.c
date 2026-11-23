struct point {
  int x;
  int y;
};

int distance(struct point a, struct point b) {
  int dx = a.x - b.x;
  int dy = a.y - b.y;
  return dx * dx + dy * dy;
}

void scale(struct point *p, int factor) {
  p->x *= factor;
  p->y *= factor;
}

int clamp(int val, int lo, int hi) {
  if (val < lo) return lo;
  if (val > hi) return hi;
  return val;
}

int process(int *arr, int n) {
  int sum = 0;
  for (int i = 0; i < n; i++)
    sum += clamp(arr[i], 0, 100);
  return sum;
}

int add(int a, int b) {
  return a + b;
}

int square(int v) {
  return v * v;
}

int sum_of_squares(int a, int b) {
  return add(square(a), square(b));
}

int normalize(int val, int lo, int hi) {
  int clamped = clamp(val, lo, hi);
  return clamped - lo;
}

int main() {
  struct point p1 = {1, 2};
  struct point p2 = {4, 6};

  int d = distance(p1, p2);
  scale(&p1, 3);
  int d2 = distance(p1, p2);

  int values[3] = {-5, 50, 200};
  int total = process(values, 3);

  int c = clamp(d, 0, total);

  // nested: clamp(add(...), ..., ...)
  int n1 = clamp(add(d, d2), 0, 100);

  // double nesting: add(clamp(...), clamp(...))
  int n2 = add(clamp(d, 0, 50), clamp(d2, 10, 90));

  // triple nesting: clamp(add(clamp(...), ...), ..., ...)
  int n3 = clamp(add(clamp(total, 0, 200), c), 0, 500);

  // main -> sum_of_squares -> square, add
  int n4 = sum_of_squares(d, d2);

  // main -> normalize -> clamp
  int n5 = normalize(total, 0, 300);

  return n1 + n2 + n3 + n4 + n5;
}
// Imported from ESBMC regression/esbmc/print-function-deps_3
// Needs: --show-call-sites
// RUN: --show-call-sites
// MODE: FUTURE
// EXPECT: ^c:@F@main -> c:@F@distance\(p1, p2\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@scale\(&p1, 3\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@process\(&values\[0\], 3\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@clamp\(d, 0, total\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@process -> c:@F@clamp\(arr\[\(signed long( long)? int\)i\], 0, 100\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@add\(d, d2\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@clamp\(return_value\$_add\$\d+, 0, 100\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@clamp\(d, 0, 50\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@clamp\(d2, 10, 90\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@add\(return_value\$_clamp\$\d+, return_value\$_clamp\$\d+\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@clamp\(total, 0, 200\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@add\(return_value\$_clamp\$\d+, c\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@clamp\(return_value\$_add\$\d+, 0, 500\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@sum_of_squares\(d, d2\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@sum_of_squares -> c:@F@square\(a\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@sum_of_squares -> c:@F@square\(b\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@sum_of_squares -> c:@F@add\(return_value\$_square\$\d+, return_value\$_square\$\d+\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@main -> c:@F@normalize\(total, 0, 300\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^c:@F@normalize -> c:@F@clamp\(val, lo, hi\) \[.*print\-function\-deps_3\.c:\d+\]$
// EXPECT: ^__ESBMC_main -> c:@F@main\(\) \[.*print\-function\-deps_3\.c:\d+\]$
// Vereda: verdict none
