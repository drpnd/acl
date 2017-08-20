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

#include "sl.h"
#include "tests.h"

/*
 * test
 */
static int
test_true(void)
{
    struct sorted_list sl;
    int ret;
    key_t keys[] = {
        {{0x5, 0, 0, 0, 0, 0}}, {{0, 0, 0, 0, 0, 0}},
        {{0x3, 0, 0, 0, 0, 0}}, {{0, 0, 0, 0, 0, 0}},
        {{0x0, 0, 0, 0, 0, 0}}, {{1, 0, 0, 0, 0, 0}},
        {{0x0, 0, 0, 0, 0, 0}}, {{0xffffffffULL, 0, 0, 0, 0, 0}},
        {{0xcbb20c00ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb22100ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb22400ULL, 0, 0, 0, 0, 0}}, {{0x3ff, 0, 0, 0, 0, 0}},
        {{0xcbb22c00ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb24000ULL, 0, 0, 0, 0, 0}}, {{0x3fff, 0, 0, 0, 0, 0}},
        {{0xcbb24e00ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb25600ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb25b00ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb25c00ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb27c00ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb27f00ULL, 0, 0, 0, 0, 0}}, {{0xff, 0, 0, 0, 0, 0}},
        {{0xcbb28000ULL, 0, 0, 0, 0, 0}}, {{0x7fff, 0, 0, 0, 0, 0}},
    };
    key_t tests[] = {
        {{0xcbb24001ULL, 0, 0, 0, 0, 0}},
        {{0xcbb26001ULL, 0, 0, 0, 0, 0}},
        {{0xcbb28701ULL, 0, 0, 0, 0, 0}},
        {{0x3, 0, 0, 0, 0, 0}},
        {{0x5, 0, 0, 0, 0, 0}},
        {{0x2, 0, 0, 0, 0, 0}},
        {{0x1, 0, 0, 0, 0, 0}},
        {{0x0, 0, 0, 0, 0, 0}},
    };

    sl_init(&sl);

    ret = sl_add_data(&sl, keys[0], keys[1], 2, (void *)5);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[2], keys[3], 2, (void *)3);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[4], keys[5], 1, (void *)2);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[6], keys[7], 0, (void *)9);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[8], keys[9], 24, (void *)11);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[10], keys[11], 24, (void *)12);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[12], keys[13], 22, (void *)13);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[14], keys[15], 24, (void *)14);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[16], keys[17], 18, (void *)15);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[18], keys[19], 24, (void *)16);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[20], keys[21], 24, (void *)17);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[22], keys[23], 24, (void *)18);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[24], keys[25], 24, (void *)19);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[26], keys[27], 24, (void *)20);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[28], keys[29], 24, (void *)21);
    if ( ret < 0 ) {
        return -1;
    }
    ret = sl_add_data(&sl, keys[30], keys[31], 17, (void *)22);
    if ( ret < 0 ) {
        return -1;
    }

    if ( 15 != (uint64_t)sl_lookup(&sl, tests[0]) ) {
        return -1;
    }
    if ( 15 != (uint64_t)sl_lookup(&sl, tests[1]) ) {
        return -1;
    }
    if ( 22 != (uint64_t)sl_lookup(&sl, tests[2]) ) {
        return -1;
    }
    if ( 3 != (uint64_t)sl_lookup(&sl, tests[3]) ) {
        return -1;
    }
    if ( 5 != (uint64_t)sl_lookup(&sl, tests[4]) ) {
        return -1;
    }
    if ( 9 != (uint64_t)sl_lookup(&sl, tests[5]) ) {
        return -1;
    }
    if ( 2 != (uint64_t)sl_lookup(&sl, tests[6]) ) {
        return -1;
    }
    if ( 2 != (uint64_t)sl_lookup(&sl, tests[7]) ) {
        return -1;
    }

    return 0;
}

int
main(int argc, const char *const argv[])
{
    int ret;

    ret = 0;

    /* Run tests */
    TEST_FUNC("true", test_true, ret);

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
