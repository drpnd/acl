/*_
 * Copyright (c) 2015-2017 Hirochika Asai <asai@jar.jp>
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "acl.h"
#include "patricia.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialize an instance
 */
struct patricia *
patricia_init(struct patricia *pat)
{
    if ( NULL == pat ) {
        pat = malloc(sizeof(struct patricia));
        if ( NULL == pat ) {
            return NULL;
        }
        pat->_allocated = 1;
    } else {
        pat->_allocated = 0;
    }
    pat->defentry = NULL;
    pat->root = NULL;

    return pat;
}

/*
 * Add data
 */
static int
_add_data(struct patricia_node **r, acl_key_t key, int prefixlen, void *data)
{
    struct patricia_node *n;
    struct patricia_node *p;
    struct patricia_node *t;
    struct patricia_node *x;
    int bit;

    /* Allocate memory for the new node */
    n = malloc(sizeof(struct patricia_node));
    if ( NULL == n ) {
        /* Memory alloation error */
        return -1;
    }
    n->bit = prefixlen - 1;
    n->left = NULL;
    n->right = NULL;
    n->key = key;
    n->prefixlen = prefixlen;
    n->data = data;

    /* Parent */
    p = NULL;
    t = *r;

    /* Traverse the trie */
    while ( NULL != t && (NULL == p || p->bit < t->bit) ) {
        p = t;
        if ( BT(key, t->bit) ) {
            /* Right */
            t = t->right;
        } else {
            /* Left */
            t = t->left;
        }
    }

    if ( NULL == t ) {
        /* Reached at the leaf but the insertion point is not found. */
        if ( NULL == p ) {
            /* Replace the root with the new one */
            *r = n;

            return 0;
        } else if ( n->bit >= p->bit ) {
            /* Leaf node */
            t = n;
            if ( BT(n->key, n->bit) ) {
                /* Right */
                t->right = t;
            } else {
                /* Left */
                t->left = t;
            }
            /* Add as a child of the parent node */
            if ( BT(n->key, p->bit) ) {
                /* Right */
                p->right = t;
            } else {
                /* Left */
                p->left = t;
            }

            return 0;
        } else {
            /* Insert to an intermediate position in the tree */
            t = p;
        }
    }

    if ( KEY_CMP(n->key, t->key) && n->prefixlen == t->prefixlen ) {
        /* Already exists */
        return -1;
    }




#if 0
    bit = 0;

    if ( NULL == *n ) {
        /* Allocate memory for the new node */
        *n = malloc(sizeof(struct patricia_node));
        if ( NULL == *n ) {
            /* Memory allocation error */
            return -1;
        }
        (*n)->bit = bit;
        (*n)->left = NULL;
        (*n)->right = NULL;
        (*n)->key = key;
        (*n)->prefixlen = prefixlen;
        (*n)->data = data;

        if ( BT(key, bit) ) {
            (*n)->right = *n;
        } else {
            (*n)->left = *n;
        }

        return 0;
    }

    /* Parent */
    p = NULL;
    t = *n;

    /* Traverse the trie */
    while ( NULL == p || p->bit < t->bit ) {
        p = t;
        if ( BT(key, t->bit) ) {
            /* Right */
            t = t->right;
        } else {
            t = t->left;
        }
        if ( NULL == t ) {
            /* Reached at the leaf */
            break;
        }
    }

    if ( NULL == t ) {
        
    }
#endif
    return 0;
}
int
patricia_add_data(struct patricia *pat, acl_key_t key, int prefixlen,
                  void *data)
{
    if ( 0 == prefixlen ) {
        pat->defentry = data;
        return 0;
    } else {
        return _add_data(&pat->root, key, prefixlen, data);
    }
}

/*
 * Lookup
 */
void *
_lookup(struct radix_tree_node *cur, struct radix_tree_node *cand,
        acl_key_t key, int depth)
{
    if ( NULL == cur ) {
        /* Return the candidate node as the longest prefix matching result */
        return NULL != cand ? cand->data : NULL;
    }

    if ( cur->valid ) {
        /* Update thte candidate return value according to the longest prefix
           matching policy */
        cand = cur;
    }

    /* Check the bit corresponding to the depth */
    if ( BT(key, depth) ) {
        /* Right node */
        return _lookup(cur->right, cand, key, depth + 1);
    } else {
        /* Left node */
        return _lookup(cur->left, cand, key, depth + 1);
    }
}
void *
radix_lookup(struct radix_tree *radix, acl_key_t key)
{
    return _lookup(radix->root, NULL, key, 0);
}

/*
 * Release the instance
 */
void
radix_release(struct radix_tree *radix)
{
    free(radix);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
