#include <stdio.h>
#include "hina.h"

static work_t *qsort_algo(__attribute__((unused)) work_t *work)
{
    printf("qsort_algo start!\n");
    /* TODO */
    return NULL;
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
    hina_init();
    hina_add_task(qsort_algo);
    hina_exit();
    return 0;
}
