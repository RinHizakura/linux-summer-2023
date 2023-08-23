#ifndef HINA_H
#define HINA_H

#include <stdatomic.h>
#include "type.h"

void hina_init();
void hina_spawn(task_t task, dtor_t dtor, void *args);
void hina_run();
void hina_exit();

#endif
