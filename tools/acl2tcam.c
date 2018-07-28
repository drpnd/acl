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
    ACL_PERMIT,
    ACL_DENY,
} acl_action_t;

typedef enum {
    ACL_IP,
    ACL_ICMP,
    ACL_TCP,
    ACL_UDP,
} acl_proto_t;

#define ACL_ESTABLISHED     (1 << 0)

typedef struct {
    uint16_t fin:1;
    uint16_t syn:1;
    uint16_t rst:1;
    uint16_t psh:1;
    uint16_t ack:1;
    uint16_t urg:1;
    uint16_t ece:1;
    uint16_t cwr:1;
    uint16_t ns:1;
    uint16_t rsvd:7;
} tcp_flag;

typedef struct {
    uint8_t proto;
    uint32_t saddr;
    uint32_t daddr;
    uint16_t sport;
    uint16_t dport;
    uint16_t flags;
} __attribute__ ((packed)) acl_ipv4_entry_t;
typedef struct {
    uint8_t data[16];
    uint8_t mask[16];
} acl_tcam_entry_t;

typedef struct {
    int count;
    uint16_t data[32];
    uint16_t mask[32];
} acl_port_range_masks_t;
typedef struct {
    uint32_t prefix;
    uint32_t mask;
} acl_ipv4_mask_t;
typedef struct {
    uint16_t lb;
    uint16_t ub;
} acl_port_range_t;

typedef struct {
    acl_action_t action;
    acl_proto_t proto;
    union {
        acl_ipv4_mask_t ip4;
    } saddr;
    acl_port_range_t sport;
    union {
        acl_ipv4_mask_t ip4;
    } daddr;
    acl_port_range_t dport;
    int flags;
    int priority;
} acl_t;

int
range2mask(acl_port_range_masks_t *, acl_port_range_t);

void
usage(const char *prog)
{
    fprintf(stderr, "Usage: %s filename\n", prog);
}

/*
 * Parse IPv4 address
 */
static int
_parse_ipv4addr(acl_ipv4_mask_t *mask, char **tok, const char *sep,
                char **saveptr)
{
    int octets[4];
    int prefixlen;

    if ( NULL == *tok ) {
        *tok = strtok_r(NULL, sep, saveptr);
        if ( NULL == *tok ) {
            return -1;
        }
    }
    sscanf(*tok, "%d.%d.%d.%d/%d", &octets[0], &octets[1], &octets[2],
           &octets[3], &prefixlen);

    mask->prefix = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8)
        | (octets[3]);
    mask->mask = ((1ULL << (32 - prefixlen)) - 1);

    *tok = NULL;

    return 0;
}

/*
 * Parse port
 */
static int
_parse_port(acl_port_range_t *range, char **tok, const char *sep,
            char **saveptr)
{
    int port;

    if ( NULL == *tok ) {
        *tok = strtok_r(NULL, sep, saveptr);
        if ( NULL == *tok ) {
            range->lb = 0;
            range->ub = 65535;
            return 0;
        }
    }
    if ( 0 == strcasecmp(*tok, "eq") ) {
        /* Port */
        *tok = strtok_r(NULL, sep, saveptr);
        port = atoi(*tok);
        range->lb = port;
        range->ub = port;
        *tok = NULL;
    } else if ( 0 == strcasecmp(*tok, "lt") ) {
        /* Port */
        *tok = strtok_r(NULL, sep, saveptr);
        port = atoi(*tok);
        range->lb = 0;
        range->ub = port - 1;
        *tok = NULL;
    } else if ( 0 == strcasecmp(*tok, "gt") ) {
        /* Port */
        *tok = strtok_r(NULL, sep, saveptr);
        port = atoi(*tok);
        range->lb = port + 1;
        range->ub = 65535;
        *tok = NULL;
    } else {
        range->lb = 0;
        range->ub = 65535;
    }

    return 0;
}

/*
 * Parse an ACL line
 */
int
parse_line(acl_t *acl, char *buf, int lineno)
{
    char *saveptr;
    char *action;
    char *proto;
    char *tok;
    const char *sep = " \t";
    int ret;

    /* Trim the tail CR/LF */
    while ( '\r' == buf[strlen(buf) - 1] || '\n' == buf[strlen(buf) - 1]) {
        buf[strlen(buf) - 1] = '\0';
    }

    acl->priority = (1 << 30) - lineno;

    /* Action */
    action = strtok_r(buf, sep, &saveptr);
    if ( NULL == action ) {
        return -1;
    } else if ( 0 == strcasecmp(action, "permit") ) {
        acl->action = ACL_PERMIT;
    } else if ( 0 == strcasecmp(action, "deny") ) {
        acl->action = ACL_DENY;
    } else {
        fprintf(stderr, "Unknown action: %s\n", action);
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
    } else if ( 0 == strcasecmp(proto, "icmp") ) {
        acl->proto = ACL_ICMP;
    } else {
        fprintf(stderr, "Unknown protocol: %s\n", proto);
        return -1;
    }

    /* Source IP address */
    tok = NULL;
    ret = _parse_ipv4addr(&acl->saddr.ip4, &tok, sep, &saveptr);
    if ( ret < 0 ) {
        return -1;
    }

    /* Source port */
    ret = _parse_port(&acl->sport, &tok, sep, &saveptr);
    if ( ret < 0 ) {
        return -1;
    }

    /* Destination IP address */
    ret = _parse_ipv4addr(&acl->daddr.ip4, &tok, sep, &saveptr);
    if ( ret < 0 ) {
        return -1;
    }

    /* Destination port */
    ret = _parse_port(&acl->dport, &tok, sep, &saveptr);
    if ( ret < 0 ) {
        return -1;
    }

    /* flags */
    if ( NULL == tok ) {
        tok = strtok_r(NULL, sep, &saveptr);
    }
    acl->flags = 0;
    if ( NULL != tok ) {
        if ( 0 == strcasecmp(tok, "established") ) {
            acl->flags = ACL_ESTABLISHED;
            tok = NULL;
        } else if ( '#' == *tok )  {
            /* Comment */
            acl->flags = 0;
            return 0;
        } else {
            fprintf(stderr, "Unknown flag: %s\n", tok);
            return -1;
        }
    }

    /* Rest of the tokens */
    if ( NULL == tok ) {
        tok = strtok_r(NULL, sep, &saveptr);
    }
    if ( NULL == tok ) {
        return 0;
    } else if ( '#' == *tok ) {
        /* Comment */
        return 0;
    } else {
        return -1;
    }
}

/*
 * Parse the specified ACL
 */
int
parse(FILE *fp)
{
    char buf[1024];
    int lineno;
    acl_t acl;
    int ret;
    acl_port_range_masks_t sports;
    acl_port_range_masks_t dports;
    int i;
    int j;
    int k;
    acl_tcam_entry_t e;
    acl_ipv4_entry_t *data;
    acl_ipv4_entry_t *mask;
    int flagc;
    tcp_flag flagd[2];
    tcp_flag flagm[2];

    lineno = 1;
    while ( NULL != fgets(buf, sizeof(buf), fp) ) {
        ret = parse_line(&acl, buf, lineno);
        if ( ret < 0 ) {
            fprintf(stderr, "Error at line %d\n", lineno);
            exit(EXIT_FAILURE);
        }
        /* Source and destination ports */
        ret = range2mask(&sports, acl.sport);
        if ( ret < 0 ) {
            return -1;
        }
        ret = range2mask(&dports, acl.dport);
        if ( ret < 0 ) {
            return -1;
        }
        /* Flags */
        memset(&flagd[0], 0, sizeof(tcp_flag));
        memset(&flagm[0], 0xff, sizeof(tcp_flag));
        memset(&flagd[1], 0, sizeof(tcp_flag));
        memset(&flagm[1], 0xff, sizeof(tcp_flag));
        if ( ACL_ESTABLISHED & acl.flags ) {
            /* Syn */
            flagc = 2;
            flagd[0].ack = 1;
            flagm[0].ack = 0;
            flagd[1].rst = 1;
            flagm[1].rst = 0;
        } else {
            flagc = 1;
        }

        /* Build TCAM */
        for ( i = 0; i < sports.count; i++ ) {
            for ( j = 0; j < sports.count; j++ ) {
                for ( k = 0; k < flagc; k++ ) {
                    memset(&e, 0x0, sizeof(acl_tcam_entry_t));
                    data = (acl_ipv4_entry_t *)e.data;
                    mask = (acl_ipv4_entry_t *)e.mask;
                    memset(mask, 0xff, 16);
                    switch ( acl.proto ) {
                    case ACL_IP:
                        data->proto = 0;
                        mask->proto = 0xff;
                        break;
                    case ACL_ICMP:
                        data->proto = 1;
                        mask->proto = 0;
                        break;
                    case ACL_TCP:
                        data->proto = 6;
                        mask->proto = 0;
                        break;
                    case ACL_UDP:
                        data->proto = 17;
                        mask->proto = 0;
                        break;
                    }
                    data->saddr = htonl(acl.saddr.ip4.prefix);
                    mask->saddr = htonl(acl.saddr.ip4.mask);
                    data->daddr = htonl(acl.daddr.ip4.prefix);
                    mask->daddr = htonl(acl.daddr.ip4.mask);
                    data->sport = htons(sports.data[i]);
                    mask->sport = htons(sports.mask[i]);
                    data->dport = htons(dports.data[j]);
                    mask->dport = htons(dports.mask[j]);
                    data->flags = *(uint16_t *)&flagd[k];
                    mask->flags = *(uint16_t *)&flagm[k];

                    int x;
                    for ( x = 0; x < 16; x++ ) {
                        printf("%02x", e.data[x]);
                    }
                    printf(" ");
                    for ( x = 0; x < 16; x++ ) {
                        printf("%02x", e.mask[x]);
                    }
                    printf(" %d %d", lineno, acl.action);
                    printf("\n");
                }
            }
        }

        lineno++;
    }

    return 0;
}

int
range2mask_rec(acl_port_range_masks_t *masks, acl_port_range_t range,
               uint16_t prefix, uint16_t mask, int b)
{
    int ret;

    if ( prefix >= range.lb && (prefix | mask) <= range.ub ) {
        if ( masks->count >= 32 ) {
            return -1;
        }
        masks->data[masks->count] = prefix;
        masks->mask[masks->count] = mask;
        masks->count++;
        return 0;
    } else if ( (prefix | mask) < range.lb || prefix > range.ub ) {
        return 0;
    } else {
        /* Partial */
    }
    if ( !mask ) {
        /* End of the recursion */
        return 0;
    }

    mask >>= 1;
    /* Left */
    ret = range2mask_rec(masks, range, prefix, mask, b + 1);
    if ( ret < 0 ) {
        return ret;
    }
    /* Right */
    prefix |= (1 << (15 - b));
    ret = range2mask_rec(masks, range, prefix, mask, b + 1);
    if ( ret < 0 ) {
        return ret;
    }
    return 0;
}

int
range2mask(acl_port_range_masks_t *masks, acl_port_range_t range)
{
    int b;
    uint16_t x;
    uint16_t y;
    uint16_t prefix;
    uint16_t mask;

    masks->count = 0;
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

    return range2mask_rec(masks, range, prefix, mask, b);
}

/*
 * Main routine
 */
int
main(int argc, const char *const argv[])
{
    const char *fname;
    FILE *fp;

    if ( argc < 2 ) {
        usage(argv[0]);
        return -1;
    }

    /* File name */
    fname = argv[1];

    fprintf(stderr, "Opening %s...\n", fname);
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
