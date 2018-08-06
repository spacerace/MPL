/*
 * Quick Sort
 */

int     (*qs__cmp)();
static  int     size;

qsort(a, n, es, fc)
char *a;
int n, es;
int (*fc)();
{
        qs__cmp = fc;
        size = es;
        q_sort(a, a+n*es);
}

q_sort(a, l)
char *a, *l;
{
        register char *i, *j;
        register int es;
        char *lp, *hp;
        int n, c;

        es = size;

start:
        if((n=l-a) <= es)
                return;
        n = es * ( n/(2*es));
        hp = lp = a+n;
        i = a;
        j = l-es;
        for(;;) {
                if(i < lp) {
                        if((c = (*qs__cmp)(i, lp)) == 0) {
                                q_exc(i, lp -= es);
                                continue;
                        }
                        if(c < 0) {
                                i += es;
                                continue;
                        }
                }

loop:
                if(j > hp) {
                        if((c = (*qs__cmp)(hp, j)) == 0) {
                                q_exc(hp += es, j);
                                goto loop;
                        }
                        if(c > 0) {
                                if(i == lp) {
                                        q_tex(i, hp += es, j);
                                        i = lp += es;
                                        goto loop;
                                }
                                q_exc(i, j);
                                j -= es;
                                i += es;
                                continue;
                        }
                        j -= es;
                        goto loop;
                }


                if(i == lp) {
                        if(lp-a >= l-hp) {
                                q_sort(hp+es, l);
                                l = lp;
                        } else {
                                q_sort(a, lp);
                                a = hp+es;
                        }
                        goto start;
                }
                q_tex(j, lp -= es, i);
                j = hp -= es;
        }
}

q_exc(i, j)
char *i, *j;
{
        register char *ri, *rj, c;
        int n;

        n = size;
        ri = i;
        rj = j;
        do {
                c = *ri;
                *ri++ = *rj;
                *rj++ = c;
        } while(--n);
}

q_tex(i, j, k)
char *i, *j, *k;
{
        register char *ri, *rj, *rk;
        int c;
        int n;

        n = size;
        ri = i;
        rj = j;
        rk = k;
        do {
                c = *ri;
                *ri++ = *rk;
                *rk++ = *rj;
                *rj++ = c;
        } while(--n);
}
