/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>


  linux/include/linux/rbtree.h

*/

#ifndef RBTREE_H
#define RBTREE_H

#include <stddef.h>

#define rb_root(r) (r->rb_node)
#define rb_left(n) (n->rb_left)
#define rb_right(n) (n->rb_right)

struct rb_node {
    unsigned long __rb_parent_color;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));
/* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_root {
    struct rb_node *rb_node;
};

#define RB_ROOT      \
    (struct rb_root) \
    {                \
        NULL,        \
    }

static inline void rb_link_node(struct rb_node *node,
                                struct rb_node *parent,
                                struct rb_node **rb_link)
{
    node->__rb_parent_color = (unsigned long) parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

void rb_insert_color(struct rb_node *, struct rb_root *);
void rb_erase(struct rb_node *, struct rb_root *);

#endif
