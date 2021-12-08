/* Um programa para calcular mdc */
int gdc (int u, int v)
{
if (v == 0) return u;
else return gdc(v,u-u/v*v);
/* u-u / v*v == u mod v */
}
void lvb(void) {
    int a;
}
void main(void)
{
int x;
int y;
x = lvb();
}