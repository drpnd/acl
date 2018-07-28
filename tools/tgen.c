/*_
 * Copyright (c) 2018 Hirochika Asai <asai@jar.jp>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

typedef enum {
    TGEN_ROSS,
} tgen_type_t;

struct tgen_ross
{
    uint32_t prefix;
    uint32_t addr;
    uint32_t ub;
    int preflen;
};

/*
 * Traffic generator context
 */
typedef struct {
    int type;
    union {
        struct tgen_ross ross;
    } u;
} tgen_t;

static __inline__ uint32_t
swap32(uint32_t x)
{
    return ((x >> 24) & 0xff) | ((x << 24) & 0xff000000) |
        ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000);
}

/*
 * Reverse-byte order sequential scan
 */
int
tgen_ross_init(tgen_t *tgen, uint32_t prefix, int preflen)
{
    tgen->type = TGEN_ROSS;
    tgen->u.ross.prefix = prefix;
    tgen->u.ross.addr = 0;
    tgen->u.ross.ub = prefix | ~(1ULL << (32 - preflen));
    tgen->u.ross.preflen = preflen;

    return 0;
}

/*
 * Run
 */
int
tgen_run(tgen_t *tgen, int nr)
{
    int i;
    uint8_t proto;
    uint32_t saddr;
    uint32_t daddr;
    uint16_t sport;
    uint16_t dport;
    uint16_t flags;

    for ( i = 0; i < nr; i++ ) {
        proto = 6;
        saddr = random();
        daddr = tgen->u.ross.prefix
            + swap32(tgen->u.ross.addr << (tgen->u.ross.preflen));
        sport = 5060;           /* SIP */
        dport = random();
        flags = 0x2;
        printf("%02x %08x %08x %04x %04x %04x\n",
               proto, saddr, daddr, sport, dport, flags);

        tgen->u.ross.addr++;
        if ( tgen->u.ross.addr >= (1 << (32 - tgen->u.ross.preflen)) ) {
            tgen->u.ross.addr = 0;
        }
    }

    return 0;
}

int
tgen_ross(uint32_t prefix, int preflen)
{
    uint32_t addr;
    uint32_t ub;

    addr = prefix;
    ub = prefix | ~(1ULL << (32 - preflen));

    for ( ;; ) {
        printf("%x\n", addr);
        if ( addr == ub ) {
            break;
        }
        addr++;
    }

    return 0;
}

/*
 * Main routine
 */
int
main(int argc, const char *const argv[])
{
    tgen_t tgen;

    srandom(0);
    tgen_ross_init(&tgen, 0xc0a80000, 16);
    tgen_run(&tgen, 65536);

    return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
