#include "treeint_rb.h"
#include <assert.h>
#include "common.h"
#include "rbtree.h"

#define treeint_rb_entry(ptr) container_of(ptr, struct treeint_rb, rb_n)

struct treeint_rb {
    int value;
    struct rb_node rb_n;
};

void *treeint_rb_init()
{
    struct rb_root *root;
    root = calloc(sizeof(struct rb_root), 1);
    *root = RB_ROOT;
    return root;
}

static void __treeint_rb_destroy(struct rb_node *n)
{
    if (rb_left(n))
        __treeint_rb_destroy(rb_left(n));

    if (rb_right(n))
        __treeint_rb_destroy(rb_right(n));

    struct treeint_rb *entry = treeint_rb_entry(n);
    free(entry);
}

int treeint_rb_destroy(void *ctx)
{
    struct rb_root *root = (struct rb_root *) ctx;
    if (rb_root(root))
        __treeint_rb_destroy(rb_root(root));

    free(root);
    return 0;
}

static struct treeint_rb *__treeint_rb_insert(void *ctx, int a)
{
    struct rb_root *root = (struct rb_root *) ctx;

    struct rb_node **n = &rb_root(root);
    struct rb_node *p = NULL;
    struct treeint_rb *entry;

    while (*n) {
        p = *n;
        entry = treeint_rb_entry(p);
        if (a == entry->value)
            return NULL;

        if (a < entry->value)
            n = &(*n)->rb_left;
        else if (a > entry->value)
            n = &(*n)->rb_right;
    }

    struct treeint_rb *i = calloc(sizeof(struct treeint_rb), 1);
    assert(i);
    i->value = a;
    rb_link_node(&i->rb_n, p, n);
    return i;
}

int treeint_rb_insert(void *ctx, int a)
{
    struct treeint_rb *node = __treeint_rb_insert(ctx, a);
    if (node == NULL)
        return -1;

    struct rb_root *root = (struct rb_root *) ctx;
    rb_insert_color(&node->rb_n, root);
    return 0;
}

void *treeint_rb_find(void *ctx, int a)
{
    struct rb_root *root = (struct rb_root *) ctx;
    struct rb_node *n = rb_root(root);
    struct treeint_rb *entry;

    while (n) {
        entry = treeint_rb_entry(n);
        if (a == entry->value)
            return entry;

        if (a < entry->value)
            n = n->rb_left;
        else if (a > entry->value)
            n = n->rb_right;
    }

    return NULL;
}

int treeint_rb_remove(void *ctx, int a)
{
    struct rb_root *root = (struct rb_root *) ctx;
    struct treeint_rb *entry = treeint_rb_find(ctx, a);

    if (!entry)
        return -1;

    rb_erase(&entry->rb_n, root);
    free(entry);
    return 0;
}

#ifdef PRINT_DEBUG
static void treeint_rb_dump_preorder(struct rb_node *n)
{
    if (!n)
        return;

    treeint_rb_dump_preorder(rb_left(n));

    struct treeint_rb *v = treeint_rb_entry(n);
    pr_debug("%d\n", v->value);

    treeint_rb_dump_preorder(rb_right(n));
}

static int treeint_rb_height(struct rb_node *node)
{
    if (node == NULL)
        return 0;

    int lheight = treeint_rb_height(rb_left(node));
    int rheight = treeint_rb_height(rb_right(node));

    return (lheight > rheight) ? (lheight + 1) : (rheight + 1);
}

static void __treeint_rb_dump_lvorder(struct rb_node *node, int level)
{
    if (node == NULL) {
        pr_debug("NULL,");
        return;
    }

    struct treeint_rb *v = treeint_rb_entry(node);
    if (level == 1) {
        pr_debug("%d,", v->value);
        return;
    }

    __treeint_rb_dump_lvorder(rb_left(node), level - 1);
    __treeint_rb_dump_lvorder(rb_right(node), level - 1);
}

static void treeint_rb_dump_lvorder(struct rb_node *root)
{
    int h = treeint_rb_height(root);
    for (int i = 1; i <= h; i++)
        __treeint_rb_dump_lvorder(root, i);
}

void treeint_rb_dump(void *ctx, enum dump_mode mode)
{
    struct rb_root *root = (struct rb_root *) ctx;

    pr_debug("[");
    if (mode == PRE_ORDER)
        treeint_rb_dump_preorder(rb_root(root));
    else
        treeint_rb_dump_lvorder(rb_root(root));
    pr_debug("]\n");
}
#else
void treeint_rb_dump(__unused void *ctx, __unused enum dump_mode mode) {}
#endif
