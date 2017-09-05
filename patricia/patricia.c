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

    bit = 0;

    /* Allocate memory for the new node */
    n = malloc(sizeof(struct patricia_node));
    if ( NULL == n ) {
        /* Memory alloation error */
        return -1;
    }
    n->bit = 0;
    n->left = NULL;
    n->right = NULL;
    n->key = key;
    n->prefixlen = prefixlen;
    n->data = data;

    /* Replace the root with the new node if no root node is specified. */
    if ( NULL == *r ) {
        n->bit = prefixlen - 1;
        *r = n;

        return 0;
    }

    /* Parent */
    p = NULL;
    t = *r;

    /* Traverse the trie to find the insertion point */
    do {
        p = t;
        if ( BT(key, t->bit) ) {
            /* Right */
            t = t->right;
        } else {
            /* Left */
            t = t->left;
        }
    } while ( NULL != t && p->bit < t->bit );

    if ( NULL == t ) {
        /* Reached at the leaf, then determine the insertion point */
        while ( EXTRACT(p->key, bit) == EXTRACT(key, bit) ) {
            bit++;
        }

        if ( bit >= p->bit ) {
            /* Leaf node */
            t = n;
            if ( BT(key, bit) ) {
                /* Right */
                t->right = t;
            } else {
                /* Left */
                t->left = t;
            }
            /* Add as a child of the parent node */
            if ( BT(key, p->bit) ) {
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

    if ( KEY_CMP(key, t->key) && prefixlen == t->prefixlen ) {
        /* Already exists */
        free(n);
        return -1;
    }

    while ( EXTRACT(t->key, bit) == EXTRACT(key, bit) ) {
        bit++;
    }

    p = NULL;
    x = *r;
    do {
        p = x;
        if ( BT(key, x->bit) ) {
            x = x->right;
        } else {
            x = x->left;
        }
    } while ( NULL != x || (p->bit < x->bit && x->bit < bit) );

    if ( NULL == x ) {
        /* Must not be reached here */
        free(n);
        return -1;
    }

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
patricia_lookup(struct patricia *pat, acl_key_t key)
{
    return NULL;
}

/*
 * Release the instance
 */
void
patricia_release(struct patricia *pat)
{
    free(pat);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
