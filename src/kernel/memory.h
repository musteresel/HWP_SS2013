/** kernel memory abstraction
 * */
#ifndef __KERNEL__MEMORY_H__
#define __KERNEL__MEMORY_H__ 1
#include <stdint.h>




//-----------------------------------------------------------------------------
/** alloc a bunch of memory
 * */
extern uint8_t * Memory_alloc(uint8_t);
/** free a previously allocated bunch of memory
 * */
extern void Memory_free(uint8_t *);
/** init kernel memory management
 * */
extern void Memory_init(void);



#endif

