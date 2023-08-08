#include "treeint_st.h"
#include <assert.h>
#include "common.h"
#include "s_tree.h"

#define treeint_st_entry(ptr) container_of(ptr, struct treeint_st, st_n)

struct treeint_st {
    int value;
    struct st_node st_n;
};

static int treeint_st_cmp(struct st_node *node, void *key)
{
    struct treeint_st *n = treeint_st_entry(node);
    int value = *(int *) key;

    return n->value - value;
}

static struct st_node *treeint_st_node_create(void *key)
{
    int value = *(int *) key;
    struct treeint_st *i = calloc(sizeof(struct treeint_st), 1);
    assert(i);

    i->value = value;
    // return the st_node reference of the new node
    return &i->st_n;
}

static void treeint_st_node_destroy(struct st_node *n)
{
    struct treeint_st *i = treeint_st_entry(n);
    free(i);
}

void *treeint_st_init()
{
    struct st_tree *tree;
    tree = st_create(treeint_st_cmp, treeint_st_node_create,
                     treeint_st_node_destroy);
    assert(tree);
    return tree;
}

int treeint_st_destroy(void *ctx)
{
    struct st_tree *tree = (struct st_tree *) ctx;

    assert(tree);
    st_destroy(tree);
    return 0;
}

int treeint_st_insert(void *ctx, int a)
{
    struct st_tree *tree = (struct st_tree *) ctx;
    return st_insert(tree, (void *) &a);
}

void *treeint_st_find(void *ctx, int a)
{
    struct st_tree *tree = (struct st_tree *) ctx;
    struct st_node *n = st_find(tree, (void *) &a);
    return n ? treeint_st_entry(n) : NULL;
}

int treeint_st_remove(void *ctx, int a)
{
    struct st_tree *tree = (struct st_tree *) ctx;
    return st_remove(tree, (void *) &a);
}

#ifdef PRINT_DEBUG
static void treeint_st_dump_preorder(struct st_node *n)
{
    if (!n)
        return;

    treeint_st_dump_preorder(st_left(n));

    struct treeint_st *v = treeint_st_entry(n);
    pr_debug("%d\n", v->value);

    treeint_st_dump_preorder(st_right(n));
}

static int treeint_st_height(struct st_node *node)
{
    if (node == NULL)
        return 0;

    int lheight = treeint_st_height(st_left(node));
    int rheight = treeint_st_height(st_right(node));

    return (lheight > rheight) ? (lheight + 1) : (rheight + 1);
}

static void __treeint_st_dump_lvorder(struct st_node *node, int level)
{
    if (node == NULL) {
        if (level == 1)
            pr_debug("null,");
        return;
    }

    struct treeint_st *v = treeint_st_entry(node);
    if (level == 1) {
        pr_debug("%d,", v->value);
        return;
    }

    __treeint_st_dump_lvorder(st_left(node), level - 1);
    __treeint_st_dump_lvorder(st_right(node), level - 1);
}

static void treeint_st_dump_lvorder(struct st_node *root)
{
    int h = treeint_st_height(root);
    for (int i = 1; i <= h; i++)
        __treeint_st_dump_lvorder(root, i);
}

void treeint_st_dump(void *ctx, enum dump_mode mode)
{
    struct st_tree *tree = (struct st_tree *) ctx;

    pr_debug("[");
    if (mode == PRE_ORDER)
        treeint_st_dump_preorder(st_root(tree));
    else
        treeint_st_dump_lvorder(st_root(tree));
    pr_debug("]\n");
}
#else
void treeint_st_dump(__unused void *ctx, __unused enum dump_mode mode) {}
#endif
