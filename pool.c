#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "errc.h"
#include "pool.h"

#ifndef NVALGRIND
# include <valgrind/memcheck.h>
#endif

/* header of each free element */
struct pool_freelist_elt {
	void *next;
};

/* XXX alignement not enforced */
int
pool_init(struct pool *p, uint32_t elt_count, uint32_t elt_size)
{
	uint32_t i;
	uint32_t area_len;
	struct pool_freelist_elt *elt;

	assert(p != NULL);
	assert(elt_size > sizeof(struct pool_freelist_elt));

	area_len = elt_count * elt_size;
	p->area = malloc(area_len);
	if (p->area == NULL) {
		return POOL_MALLOC_ERROR;
	}
	p->area_len = area_len;

	/* Construct freelist */
	p->free = p->area;
	p->free_count = elt_count;

	elt = p->area;
	for (i = 0; i < elt_count - 1; i++) {
		elt->next = (void *)((size_t)elt + elt_size); /* XXX Alignement */
		elt = elt->next;
	}
	elt->next = NULL;

#ifndef NVALGRIND
	VALGRIND_CREATE_MEMPOOL(p, 0, 1);
	VALGRIND_MAKE_MEM_NOACCESS(p->area, p->area_len);
#endif

	p->elt_count = elt_count;
	p->elt_size = elt_size;

	return SUCCESS;
}

void
pool_clean(struct pool *p)
{
	assert(p != NULL);

#ifndef NVALGRIND
	VALGRIND_DO_LEAK_CHECK;
	VALGRIND_DESTROY_MEMPOOL(p);
#endif

	free(p->area);

}

void *
pool_alloc(struct pool *p)
{
	void *elt;

	if (pool_is_empty(p)) {
		return NULL;
	}

#ifndef NVALGRIND
	VALGRIND_MEMPOOL_ALLOC(p, p->free, p->elt_size);
#endif

	elt = p->free;
	p->free = ((struct pool_freelist_elt *)(p->free))->next;
	(p->free_count)--;

	return elt;
}

void
pool_free(struct pool *p, void *elt)
{
	struct pool_freelist_elt *free_elt;

	/* if all elements are already in free list */
	if (pool_is_full(p)) {
		return;
	}

	free_elt = (struct pool_freelist_elt *)elt;

	free_elt->next = p->free;

	p->free = free_elt;
	(p->free_count)++;

#ifndef NVALGRIND
	VALGRIND_MEMPOOL_FREE(p, elt);
#endif

}


int
pool_safe_memcpy(struct pool	*p,
	 void						*dest,
	 void						*src,
	 uint32_t					src_len)
{

	assert(p != NULL);
	assert(dest != NULL);
	assert(src != NULL);
	assert(src_len > 0);

	if (src_len > p->elt_size) {
		return ERROR;
	}

	memcpy(dest, src, p->elt_size);

	return SUCCESS;
}
