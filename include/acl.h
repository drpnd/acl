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

#ifndef _ACL_H
#define _ACL_H

#include <stdint.h>

#define ACL_KEY_LENGTH 512

/* 512-bit key */
typedef struct { uint64_t a[8]; } acl_key_t;

#define KEY_MASK(key, mask) do {              \
        (key).a[0] &= ~(mask).a[0];            \
        (key).a[1] &= ~(mask).a[1];            \
        (key).a[2] &= ~(mask).a[2];            \
        (key).a[3] &= ~(mask).a[3];            \
        (key).a[4] &= ~(mask).a[4];            \
        (key).a[5] &= ~(mask).a[5];            \
        (key).a[6] &= ~(mask).a[6];            \
        (key).a[7] &= ~(mask).a[7];            \
    } while ( 0 )

#define KEY_MASK_CMP(key0, mask0, key1, mask1)                          \
    (((key0).a[0] & ~(mask0).a[0]) == ((key1).a[0] & ~(mask1).a[0])     \
     && ((key0).a[1] & ~(mask0).a[1]) == ((key1).a[1] & ~(mask1).a[1])  \
     && ((key0).a[2] & ~(mask0).a[2]) == ((key1).a[2] & ~(mask1).a[2])  \
     && ((key0).a[3] & ~(mask0).a[3]) == ((key1).a[3] & ~(mask1).a[3])  \
     && ((key0).a[4] & ~(mask0).a[4]) == ((key1).a[4] & ~(mask1).a[4])  \
     && ((key0).a[5] & ~(mask0).a[5]) == ((key1).a[5] & ~(mask1).a[5])  \
     && ((key0).a[6] & ~(mask0).a[6]) == ((key1).a[6] & ~(mask1).a[6])  \
     && ((key0).a[7] & ~(mask0).a[7]) == ((key1).a[7] & ~(mask1).a[7]))

#define KEY_MASK_CMP2(key0, mask0, key1)                              \
    (((key0).a[0] & ~(mask0).a[0]) == ((key1).a[0])                   \
     && ((key0).a[1] & ~(mask0).a[1]) == ((key1).a[1])                \
     && ((key0).a[2] & ~(mask0).a[2]) == ((key1).a[2])                \
     && ((key0).a[3] & ~(mask0).a[3]) == ((key1).a[3])                \
     && ((key0).a[4] & ~(mask0).a[4]) == ((key1).a[4])                \
     && ((key0).a[5] & ~(mask0).a[5]) == ((key1).a[5])                \
     && ((key0).a[6] & ~(mask0).a[6]) == ((key1).a[6])                \
     && ((key0).a[7] & ~(mask0).a[7]) == ((key1).a[7]))

#define KEY_CMP(x, y)                                                   \
    ((x).a[0] == (y).a[0] && (x).a[1] == (y).a[1] && (x).a[2] == (y).a[2] \
     && (x).a[3] == (y).a[3] && (x).a[4] == (y).a[4] && (x).a[5] == (y).a[5] \
     && (x).a[6] == (y).a[6] && (x).a[7] == (y).a[7])

#define BT(key, bit)                                                    \
    ( (key).a[(bit) >> 6] & (0x8000000000000000ULL >> ((bit) & 0x3f)) )
#define BTS(key, bit) do {                                              \
        (key).a[(bit) >> 6] |= (0x8000000000000000ULL >> ((bit) & 0x3f)); \
    } while ( 0 )
#define BTC(key, bit) do {                                              \
        (key).a[(bit) >> 6] &= ~(0x8000000000000000ULL >> ((bit) & 0x3f)); \
    } while ( 0 )


#define EXTRACT(key, bit)      (((key).a[(bit) >> 6] >> ((bit) & 0x3f)) & 1)

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
