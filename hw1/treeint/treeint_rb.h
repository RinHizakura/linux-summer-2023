#ifndef TREEINT_RBTREE_H
#define TREEINT_RBTREE_H

#include "treeint_common.h"

extern void *treeint_rb_init();
extern int treeint_rb_destroy(void *ctx);
extern int treeint_rb_insert(void *ctx, int a);
extern void *treeint_rb_find(void *ctx, int a);
extern int treeint_rb_remove(void *ctx, int a);
extern void treeint_rb_dump(void *ctx, enum dump_mode mode);
#endif
