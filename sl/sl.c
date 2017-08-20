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
#include "sl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialize an instance
 */
struct sorted_list *
sl_init(struct sorted_list *sl)
{
    if ( NULL == sl ) {
        sl = malloc(sizeof(struct sorted_list));
        if ( NULL == sl ) {
            return NULL;
        }
        sl->_allocated = 1;
    } else {
        sl->_allocated = 0;
    }
    sl->head = NULL;

    return sl;
}

/*
 * Add data
 */
static int
_add_data(struct sorted_list_entry **head, acl_key_t key, acl_key_t mask,
          int priority, void *data)
{
    struct sorted_list_entry *ent;
    struct sorted_list_entry *cur;
    struct sorted_list_entry *prev;

    /* Allocate a sorted list entry */
    ent = malloc(sizeof(struct sorted_list_entry));
    if ( NULL == ent ) {
        /* Memory allocation failed. */
        return -1;
    }
    /* Set the key, value and priority */
    ent->addr = key;
    ent->mask = mask;
    ent->priority = priority;
    ent->data = data;
    ent->next = NULL;

    /* Search the insertion position */
    prev = NULL;
    cur = *head;
    while ( NULL != cur && cur->priority > priority ) {
        prev = cur;
        cur = cur->next;
    }

    if ( NULL == prev ) {
        *head = ent;
        ent->next = cur;
    } else {
        prev->next = ent;
        ent->next = cur;
    }

    return 0;
}
int
sl_add_data(struct sorted_list *sl, acl_key_t key, acl_key_t mask, int priority,
            void *data)
{
    return _add_data(&sl->head, key, mask, priority, data);
}

/*
 * Lookup
 */
void *
sl_lookup(struct sorted_list *sl, acl_key_t key)
{
    struct sorted_list_entry *ent;

    ent = sl->head;
    while ( NULL != ent ) {
        if ( KEY_MASK_CMP(ent->addr, ent->mask, key, ent->mask) ) {
            return ent->data;
        }
        ent = ent->next;
    }

    return NULL;
}

/*
 * Release the instance
 */
void
sl_release(struct sorted_list *sl)
{
    free(sl);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
