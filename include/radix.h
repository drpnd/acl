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

#ifndef _ACL_RADIX_H
#define _ACL_RADIX_H

#include "acl.h"

/*
 * Node data structure of radix tree
 */
struct radix_tree_node {
    int valid;

    /* Left child */
    struct radix_tree_node *left;

    /* Right child */
    struct radix_tree_node *right;

    /* Data */
    void *data;
};

/*
 * Data structure for radix tree
 */
struct radix_tree {
    struct radix_tree_node *root;
    int _allocated;
};

#ifdef __cplusplus
extern "C" {
#endif

    struct radix_tree * radix_init(struct radix_tree *);
    int radix_add_data(struct radix_tree *, acl_key_t, int, void *);
    void * radix_lookup(struct radix_tree *, acl_key_t);

#ifdef __cplusplus
}
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
