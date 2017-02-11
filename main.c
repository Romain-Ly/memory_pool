#include <assert.h>
#include <stdio.h>
#include <string.h>
//XXX
#include <stdlib.h>

#include "err.h"
#include "pool.h"
#include "test_tools/test.h"

#define POOL_SIZE 100
#define TEST_SIZE 16
struct test_elt {
	char a[TEST_SIZE];
};
#define WRITE_TEST_ELT(e) memset((e), 'q', sizeof(struct test_elt))

int test_alloc_all(struct test_suite *ts);
int test_overflow_element(struct test_suite *ts);
int test_alloc_random_free(struct test_suite *ts);
int test_double_free(struct test_suite *ts); /* Valgrind memory pool test */
int test_write_freed_block(struct test_suite *ts); /* Valgrind memory pool test */

int test_not_freed_block(struct test_suite *ts); /* Valgrind memory pool test */

int
test_alloc_all(struct test_suite *ts)
{
	struct pool p;
	void *ret[POOL_SIZE]; /* store each element adress */
	unsigned int i;

	dbg_print("Init pool...\n");
	pool_init(&p, POOL_SIZE, sizeof(struct test_elt));

	test_ok(ts, pool_is_empty(&p) == 0);
	test_ok(ts, pool_is_full(&p) == 1);

	dbg_print("Alloc and write in each memory block...\n");
	for (i = 0; i < POOL_SIZE; i++) {
		ret[i] = pool_alloc(&p);
		test_ok(ts, ret[i] != NULL);
		test_ok(ts, pool_is_full(&p) == 0);
		WRITE_TEST_ELT(ret[i]);
	}

	test_ok(ts, pool_is_empty(&p) == 1);
	test_ok(ts, pool_alloc(&p) == NULL);

	dbg_print("Free all blocks...\n");
	for (i = 0; i < POOL_SIZE; i++) {
		pool_free(&p, ret[i]);
	}

	test_ok(ts, pool_is_empty(&p) == 0);
	test_ok(ts, pool_is_full(&p) == 1);

	dbg_print("Clean pool...\n");
	pool_clean(&p);

	return 0;
}

int
test_overflow_element(struct test_suite *ts)
{
	struct pool p;
	void *ret[2];
	void *ret_1_address;
	void *ret_1_realloc_address;
	pool_init(&p, POOL_SIZE, sizeof(struct test_elt));

	/* take first two addresses */
	ret[0] = pool_alloc(&p);
	test_ok(ts, ret[0] != NULL);

	/* release second element and write in first element with overflow */
	ret[1] = pool_alloc(&p);
	test_ok(ts, ret[1] != NULL);
	ret_1_address = ret[1];
	pool_free(&p, ret[1]);

	/* if you replace pool_safe_memcpy call by the memset below, the
	 * next free element adress will be overwritten.
	 */
	/* memset(ret[0], 'a', 17); */
	pool_safe_memcpy(&p, ret[0], "aaaaaaaaaaaaaaaaa", 17);

	/* If the freelist is corrupted, next block address will be wrong */
	ret_1_realloc_address = pool_alloc(&p);
	test_ok(ts, ret_1_realloc_address != NULL);

	/* Test if pointer corrupted*/
	test_ok(ts, ret_1_realloc_address == ret_1_address);

	pool_free(&p, ret_1_realloc_address);
	pool_free(&p, ret[0]);

	pool_clean(&p);
	return 0;
}

int
test_alloc_random_free(struct test_suite *ts)
{
	struct pool p;
	void *ret[POOL_SIZE]; /* store each element adress */
	unsigned int indexes[] = {10, 0, 50, 6, 90};
	unsigned int indexes_len = 5;
	unsigned int i;

	dbg_print("Init pool...\n");
	pool_init(&p, POOL_SIZE, sizeof(struct test_elt));
	assert(pool_is_full(&p));

	dbg_print("Alloc and write in each memory block...\n");
	for (i = 0; i < POOL_SIZE; i++) {
		ret[i] = pool_alloc(&p);
		WRITE_TEST_ELT(ret[i]);
	}

	dbg_print("Free some choosen blocks...\n");
	for (i = 0; i < indexes_len; i++) {
		pool_free(&p, ret[indexes[i]]);
	}

	dbg_print("Re-alloc previously freed blocks and write in it...\n");
	for (i = 0; i < indexes_len; i++) {
		ret[indexes[i]] = pool_alloc(&p);
		WRITE_TEST_ELT(ret[indexes[i]]);
	}

	dbg_print("Free all blocks...\n");
	for (i = 0; i < POOL_SIZE; i++) {
		pool_free(&p, ret[i]);
	}

	dbg_print("Clean pool...\n");
	pool_clean(&p);

	return 0;
}

int
test_double_free(struct test_suite *ts)
{
	struct pool p;
	void *ret;

	pool_init(&p, POOL_SIZE, sizeof(struct test_elt));

	/* take first two addresses */
	ret = pool_alloc(&p);
	test_ok(ts, ret != NULL);

	pool_free(&p, ret);
	pool_free(&p, ret);

	pool_clean(&p);
	return 0;
}

INIT_ERR();

int
main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	struct test_suite ts;

	SET_ERR(stderr);

	test_init(&ts);
	test_alloc_all(&ts);
	test_display(&ts);
	tests_reset(&ts);

	test_overflow_element(&ts);
	test_display(&ts);
	tests_reset(&ts);

	test_alloc_random_free(&ts);
	test_display(&ts);
	tests_reset(&ts);

	test_double_free(&ts);
	test_display(&ts);
	tests_reset(&ts);

	test_display(&ts);
	return 0;
}
