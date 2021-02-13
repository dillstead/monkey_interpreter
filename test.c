#include <stdlib.h>
#include <stdio.h>

struct foo
{
    int i;
    int j;
};

struct foo get(int k)
{
    return (struct foo){ .i = k, .j = k };
}
    
int main(void)
{
    char a[10];
    char (*b)[10];

    b = &a;
    printf("%p %p %p %p\n", a, (void *) &a, (void *) b, (void *) &b);

    return EXIT_SUCCESS;
}

