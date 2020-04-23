#include <stdlib.h>
#include <stdio.h>

#include "repl.h"

int main(void)
{
    printf("Hello! This is the Monkey programming language!\n");
    printf("Feel free to type in commands\n");
    repl_start();
    return EXIT_SUCCESS;
}
