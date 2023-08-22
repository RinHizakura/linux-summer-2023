#ifndef HINA_H
#define HINA_H

#include <stdatomic.h>
#include "work.h"

void hina_init();
void hina_add_task(task_t task);
void hina_exit();

#endif
