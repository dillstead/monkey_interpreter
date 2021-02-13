#include <text.h>

#include "util.h"

int text_cmp(const void *x, const void *y)
{
    return Text_cmp(*((Text_T *) x), *((Text_T *)y));
}

unsigned text_hash(const void *x)
{
    int i;
    unsigned h = 0;
    const Text_T *t = x;

    for (i = 0; i < t->len; i++)
    {
        h = (h<<1) + t->str[i];
    }
    return h;
}
