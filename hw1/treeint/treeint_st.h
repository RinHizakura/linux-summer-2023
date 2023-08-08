#ifndef TREEINT_ST_H
#define TREEINT_ST_H

#include "treeint_common.h"

extern void *treeint_st_init();
extern int treeint_st_destroy(void *ctx);
extern int treeint_st_insert(void *ctx, int a);
extern void *treeint_st_find(void *ctx, int a);
extern int treeint_st_remove(void *ctx, int a);
extern void treeint_st_dump(void *ctx, enum dump_mode mode);

#endif
