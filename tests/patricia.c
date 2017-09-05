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

#include "patricia.h"
#include "tests.h"

/*
 * Performance test with a routing table
 */
#include <arpa/inet.h>
#include <netinet/in.h>
static int
test_lookup_performance(void)
{
    struct patricia pat;
    FILE *fp;
    char buf[4096];
    int ret;
    acl_key_t addr1 = {{0, 0, 0, 0, 0, 0}};
    uint64_t addr2;
    uint64_t i;
    char buf1[256];
    char buf2[256];
    struct in6_addr prefix;
    int prefixlen;
    struct in6_addr nexthop;

    /* Initialize */
    patricia_init(&pat);

    /* Load from the linx file */
    fp = fopen("tests/rviews-inet6-wide-p7-rib.20170820.1200.txt", "r");
    if ( NULL == fp ) {
        return -1;
    }

    /* Load the full route */
    i = 0;
    while ( !feof(fp) ) {
        if ( !fgets(buf, sizeof(buf), fp) ) {
            continue;
        }

        ret = sscanf(buf, "%255[^/]/%d %255s", buf1, &prefixlen, buf2);
        if ( ret < 0 ) {
            return -1;
        }

        /* Prefix */
        ret = inet_pton(AF_INET6, buf1, &prefix);
        if ( ret != 1 ) {
            return -1;
        }
        /* Nexthop */
        ret = inet_pton(AF_INET6, buf2, &nexthop);
        if ( ret != 1 ) {
            return -1;
        }

        /* Convert to uint64_t */
        addr1.a[0] = ((uint64_t)prefix.s6_addr[0] << 56)
            + ((uint64_t)prefix.s6_addr[1] << 48)
            + ((uint64_t)prefix.s6_addr[2] << 40)
            + ((uint64_t)prefix.s6_addr[3] << 32)
            + ((uint64_t)prefix.s6_addr[4] << 24)
            + ((uint64_t)prefix.s6_addr[5] << 16)
            + ((uint64_t)prefix.s6_addr[6] << 8)
            + (uint64_t)prefix.s6_addr[7];
        addr1.a[1] = ((uint64_t)prefix.s6_addr[8] << 56)
            + ((uint64_t)prefix.s6_addr[9] << 48)
            + ((uint64_t)prefix.s6_addr[10] << 40)
            + ((uint64_t)prefix.s6_addr[11] << 32)
            + ((uint64_t)prefix.s6_addr[12] << 24)
            + ((uint64_t)prefix.s6_addr[13] << 16)
            + ((uint64_t)prefix.s6_addr[14] << 8)
            + (uint64_t)prefix.s6_addr[15];
        addr2 = ((uint64_t)nexthop.s6_addr[8] << 56)
            + ((uint64_t)nexthop.s6_addr[9] << 48)
            + ((uint64_t)nexthop.s6_addr[10] << 40)
            + ((uint64_t)nexthop.s6_addr[11] << 32)
            + ((uint64_t)nexthop.s6_addr[12] << 24)
            + ((uint64_t)nexthop.s6_addr[13] << 16)
            + ((uint64_t)nexthop.s6_addr[14] << 8)
            + (uint64_t)nexthop.s6_addr[15];

        /* Add an entry */
        ret = patricia_add_data(&pat, addr1, prefixlen, (void *)addr2);
        if ( ret < 0 ) {
            return -1;
        }
        i++;
    }

    /* Close */
    fclose(fp);

    double t0, t1;
    uint64_t x = 0;
    acl_key_t tmp = {{0, 0, 0, 0, 0, 0}};
    t0 = getmicrotime();
    for ( i = 0; i < 0x10000ULL; i++ ) {
        tmp.a[0] = (0x2000ULL<<48)
            | (((uint64_t)xor128() << 32) & 0x0fffffffffffffffULL)
            | xor128();
        x ^= (uint64_t)patricia_lookup(&pat, tmp);
    }
    t1 = getmicrotime();
    printf("@@@ Performance: %.6lf sec: %.3lf lookup/sec, %llx) @@@",
           t1 - t0, i / (t1 - t0), x);

    return 0;
}

/*
 * Main routine
 */
int
main(int argc, const char *const argv[])
{
    int ret;

    ret = 0;

    /* Run tests */
    TEST_FUNC("performance", test_lookup_performance, ret);

    return ret;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
