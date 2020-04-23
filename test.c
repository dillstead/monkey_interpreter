#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    printf("%lu %lu %lu %lu %lu\n", sizeof '\0', sizeof "\0", sizeof "", sizeof '"', sizeof "\"");
    return EXIT_SUCCESS;
}
