int foo (int x)
{
  x += 2;
  if (x & 1)
    goto a;
b:
__attribute__ ((__maybe_unused__));
  x *= 3;
a:
  return x;
}
