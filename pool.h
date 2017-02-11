#ifndef POOL_H_
#define POOL_H_

#include <stdint.h>

struct pool {
	void *area;
	uint32_t area_len;

	uint32_t elt_count;
	uint32_t elt_size;

	uint32_t free_count;
	void *free;  /* Free list head */
};

/* NOTES:
 *  - alignment not enforced
 *  - if data size is larger than the returned element size,
 *    next chunk will be corrupted and the free list will be corrupted.
 *    To prevent this, pool_safe_memcpy has been added.
 *
 * alignement macro :
 *    #define ALIGN(x,a) (typeof(x))((((size_t)(x) + ((size_t)(a) - 1)) & ~((size_t)(a) - 1)))
 */

int pool_init(struct pool	*p,
     uint32_t				elt_count,
     uint32_t				elt_size);
void pool_clean(struct pool *p);

void * pool_alloc(struct pool *p);
void pool_free(struct pool	*p,
     void					*elt);

/* XXX may cause partial copy of data */
int pool_safe_memcpy(struct pool	*p,
     void							*dest,
     void							*src,
     uint32_t						src_len);

#define pool_is_empty(p) ((p)->free_count == 0)
#define pool_is_full(p) ((p)->free_count == (p)->elt_count)

#endif /* POOL_H_ */
