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
#include "radix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialize an instance
 */
struct radix_tree *
radix_init(struct radix_tree *radix)
{
    if ( NULL == radix ) {
        radix = malloc(sizeof(struct radix_tree));
        if ( NULL == radix ) {
            return NULL;
        }
        radix->_allocated = 1;
    } else {
        radix->_allocated = 0;
    }
    radix->root = NULL;

    return radix;
}

/*
 * Add data
 */
static int
_add_data(struct radix_tree_node **cur, acl_key_t key, int prefixlen,
          void *data, int depth)
{
    struct radix_tree_node *new;

    /* Allocate a new node */
    if ( NULL == *cur ) {
        new = malloc(sizeof(struct radix_tree_node));
        if ( NULL == new ) {
            return -1;
        }
        memset(new, 0, sizeof(struct radix_tree_node));
        *cur = new;
    }

    if ( prefixlen == depth ) {
        /* The current node is the point to add the data */
        if ( (*cur)->valid ) {
            /* Already exists */
            return -1;
        }
        (*cur)->valid = 1;
        (*cur)->data = data;

        return 0;
    } else {
        if ( BT(key, depth) ) {
            /* Right node */
            return _add_data(&(*cur)->right, key, prefixlen, data, depth + 1);
        } else {
            /* Left node */
            return _add_data(&(*cur)->left, key, prefixlen, data, depth + 1);
        }
    }
}
int
radix_add_data(struct radix_tree *radix, acl_key_t key, int prefixlen,
               void *data)
{
    return _add_data(&radix->root, key, prefixlen, data, 0);
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
