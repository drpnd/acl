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

typedef enum {
    ACL_PERMIT,
    ACL_DENY,
} acl_action_t;

typedef enum {
    ACL_IP,
    ACL_TCP,
    ACL_UDP,
} acl_proto_t;

typedef struct {
    uint32_t prefix;
    uint32_t mask;
} ack_ipv4_mask_t;
typedef struct {
    uint16_t lb;
    uint16_t ub;
} acl_port_range_t;

typedef struct {
    acl_action_t action;
    acl_proto_t proto;
    union {
        ack_ipv4_mask_t ip4;
    } saddr;
    acl_port_range_t sport;
    union {
        ack_ipv4_mask_t ip4;
    } daddr;
    acl_port_range_t dport;
} acl_t;

void
usage(const char *prog)
{
    fprintf(stderr, "Usage: %s filename\n", prog);
}

/*
 * Parse an ACL line
 */
int
parse_line(char *buf)
{
    char *saveptr;
    char *action;
    char *proto;
    char *src;
    char *dst;
    const char *sep = " \t";
    acl_t *acl;
    acl = alloca(sizeof(acl_t));

    /* Trim the tail CR/LF */
    while ( '\r' == buf[strlen(buf) - 1]
            || '\n' == buf[strlen(buf) - 1]) {
        buf[strlen(buf) - 1] = '\0';
    }

    /* Action */
    action = strtok_r(buf, sep, &saveptr);
    if ( NULL == action ) {
        return -1;
    } else if ( 0 == strcasecmp(action, "permit") ) {
        acl->action = ACL_PERMIT;
    } else if ( 0 == strcasecmp(action, "deny") ) {
        acl->action = ACL_DENY;
    } else {
        return -1;
    }

    /* Protocol */
    proto = strtok_r(NULL, sep, &saveptr);
    if ( NULL == proto ) {
        return -1;
    } else if ( 0 == strcasecmp(proto, "ip") ) {
        acl->proto = ACL_IP;
    } else if ( 0 == strcasecmp(proto, "tcp") ) {
        acl->proto = ACL_TCP;
    } else if ( 0 == strcasecmp(proto, "udp") ) {
        acl->proto = ACL_UDP;
    }


    src = strtok_r(NULL, sep, &saveptr);
    dst = strtok_r(NULL, sep, &saveptr);

    printf("%s %s %s %s\n", action, proto, src, dst);

    return 0;
}

/*
 * Parse the specified ACL
 */
int
parse(FILE *fp)
{
    char buf[1024];

    while ( NULL != fgets(buf, sizeof(buf), fp) ) {
        parse_line(buf);
    }

    return 0;
}

void
range2mask_rec(acl_port_range_t range, uint16_t prefix, uint16_t mask, int b)
{

    if ( prefix >= range.lb && (prefix | mask) <= range.ub ) {
        printf(">>> %x %x\n", prefix, mask);
        return;
    } else if ( (prefix | mask) < range.lb || prefix > range.ub ) {
        return;
    } else {
        /* Partial */
    }
    if ( !mask ) {
        /* End of the recursion */
        return;
    }

    mask >>= 1;
    /* Left */
    range2mask_rec(range, prefix, mask, b + 1);
    /* Right */
    prefix |= (1 << (15 - b));
    range2mask_rec(range, prefix, mask, b + 1);
}

void
range2mask(acl_port_range_t range)
{
    int b;
    uint16_t x;
    uint16_t y;
    uint16_t prefix;
    uint16_t mask;

    for ( b = 0; b < 16; b++ ) {
        x = range.lb & (1 << (15 - b));
        y = range.ub & (1 << (15 - b));
        if ( x != y ) {
            /* The most significant different bit */
            break;
        }
    }
    mask = (1 << (16 - b)) - 1;
    prefix = range.lb & ~mask;

    range2mask_rec(range, prefix, mask, b);
}

/*
 * Main routine
 */
int
main(int argc, const char *const argv[])
{
    const char *fname;
    FILE *fp;

    acl_port_range_t range;
    range.lb = 5;
    range.ub = 27;
    range2mask(range);

    return 0;

    if ( argc < 2 ) {
        usage(argv[0]);
        return -1;
    }

    /* File name */
    fname = argv[1];

    printf("Opening %s...\n", fname);
    fp = fopen(fname, "r");
    if ( NULL == fp ) {
        usage(argv[0]);
        return -1;
    }

    parse(fp);

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
