#ifndef STREE_H
#define STREE_H

#define st_root(r) (r->root)
#define st_left(n) (n->left)
#define st_right(n) (n->right)
#define st_rparent(n) (st_right(n)->parent)
#define st_lparent(n) (st_left(n)->parent)
#define st_parent(n) (n->parent)

/* S-Tree uses hints to decide whether to perform a balancing operation or not.
 * Hints are similar to AVL-trees' height property, but they are not
 * required to be absolutely accurate. A hint provides an approximation
 * of the longest chain of nodes under the node to which the hint is attached.
 */
struct st_node {
    short hint;
    struct st_node *parent;
    struct st_node *left, *right;
};

typedef int cmp_t(const void *, const void *);
struct st_tree {
    struct st_node *root;
    cmp_t *cmp;
    struct st_node *(*create_node)();
    void (*destroy_node)(struct st_node *n);
};

enum st_dir { LEFT, RIGHT };

struct st_tree *st_create(cmp_t *cmp,
                          struct st_node *(*create_node)(),
                          void (*destroy_node)(struct st_node *n));
void st_destroy(struct st_tree *tree);
void st_insert(struct st_node **root,
               struct st_node *p,
               struct st_node *n,
               enum st_dir d);
void st_remove(struct st_node **root, struct st_node *del);

#endif
