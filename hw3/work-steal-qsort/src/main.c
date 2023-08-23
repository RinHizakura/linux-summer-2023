#include <stdio.h>
#include "hina.h"

static void qsort_algo(__attribute__((unused)) void *args[])
{
    printf("qsort_algo start!\n");
    /* TODO */
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
    hina_init();
    hina_add_task(qsort_algo);
    hina_exit();
    return 0;
}
