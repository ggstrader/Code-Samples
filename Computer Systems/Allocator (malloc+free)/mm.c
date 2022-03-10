/* Garin Strader CS4400 Fall 2018

NOTES:

// This allocator relies on structs being of size ALIGNMENT. It is able to use
    either first or best fit and has a doubly linked free list for
    constant time insertion/deletion when allocating.

// Footer/free macros can only be used when the payload is deallocated. Footers
    do not exist when a block is allocated.

*/

#include "mm.h"

#define DBG               0
#define ALIGNMENT         16
#define ALGORITHM         FIRST
#define ALLOC_OVERHEAD    (sizeof(block_header))
#define DEALLOC_OVERHEAD  (sizeof(block_header) + sizeof(block_footer))

// using low 4 bits of header size since they are always 0 if aligned by 16
#define _PAKVAL_(bp) (ASHDRP(HDRP(bp)))->size
typedef struct {
  size_t size;
  void *prev_free;
} block_header;
typedef enum {
  ALLOCATED = 0x1,
  PREV_ALLOCATED = 0x2,
  UNUSED1 = 0x4, // could likely remove the need for terminating block by
  UNUSED2 = 0x8 // using one of these as a flag
} Bit;
// no packing used on footers
//(still available if needed, macros would need adjusting)
typedef struct {
  size_t size;
  void *next_free;
} block_footer;

void mm_init(void *, size_t);
void *mm_malloc(size_t);
void mm_free(void *);

static void *get_bestfit(size_t);
static void *get_firstfit(size_t);

static void set_allocated(void *, size_t);
static void split_block(void *, size_t, size_t);
static void coalesce(void *);
static void *remove_free_node(void *const);
static void *add_free_node(void *const);

static int get_bit(void *, Bit);
static void *flag_allocated(void *const);
static void *flag_deallocated(void *const);
static void *init_header(void *const, size_t, size_t);
static void *init_footer(void *const);

typedef enum { FIRST, BEST } ALG;
typedef void *(*alg_ptr)(size_t);
static alg_ptr get_memory;
static void set_algorithm() {
  switch (ALGORITHM) {
  case BEST: get_memory = &get_bestfit; break;
  case FIRST: get_memory = &get_firstfit; break;
  default: get_memory = NULL;
  }
}

// casting
#define ASBYTEP(p)  ((char *)(p))
#define ASHDRP(p)   ((block_header *)(p))
#define ASFTRP(p)   ((block_footer *)(p))
// casting

#define ALIGN(size)       (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

#define HDRP(bp)              (ASBYTEP(bp) - sizeof(block_header))
#define FTRP(bp)              (ASBYTEP(bp) + GET_SIZE_H(bp) - DEALLOC_OVERHEAD)

#define GET_SIZE_H(bp)        (_PAKVAL_(bp) & ~0xF)
#define SET_SIZE_H(bp, val)   (_PAKVAL_(bp) = (_PAKVAL_(bp) & 0xF) | ((val) & ~0xF))
#define SIZE_F(bp)            (ASFTRP(FTRP(bp)))->size

#define GET_BIT(bp, bit)      (_PAKVAL_(bp) & (bit))
#define SET_BIT(bp, mask)     (_PAKVAL_(bp) = (_PAKVAL_(bp) | (mask)))
#define CLR_BIT(bp, mask)     (_PAKVAL_(bp) = (_PAKVAL_(bp) & ~(mask)))

#define NEXT_BLKP(bp)         (ASBYTEP(bp) + GET_SIZE_H(bp))
#define PREV_BLKP(bp)         (ASBYTEP(bp) - ASFTRP((ASBYTEP(bp) - DEALLOC_OVERHEAD))->size)

#define NEXT_FREE(bp)         (ASFTRP(FTRP(bp))->next_free)
#define PREV_FREE(bp)         (ASHDRP(HDRP(bp))->prev_free)

// TESTING
enum { NOPRNT, PRNT };
#define OPT_PRNT(print, ...) if (print) printf(__VA_ARGS__)
#define PRNT(...)        OPT_PRNT(DBG, __VA_ARGS__)
#define EXITED()         PRNT("Exited function: %s\n", __func__)
#define ENTERED()        PRNT("Entered function: %s\n", __func__)
#define HERE(i)          PRNT("HERE %d\n", i)
#define FROM_START(p)    (p == NULL ? 0 : DIST(p))
#define DIST(p)          ((size_t)((void *)(p) - ((first_bp) - sizeof(block_header))))
// TESTING

static void *first_bp;
static void *free_list;

void mm_init(void *heap, size_t heap_size) {
  set_algorithm();
  first_bp = free_list = NULL;
  first_bp = heap + sizeof(block_header); // need room for header before bp
  heap_size -= sizeof(block_header); // account for terminating block
  add_free_node(init_footer(init_header(first_bp, heap_size, PREV_ALLOCATED)));
  init_header(NEXT_BLKP(first_bp), 0, ALLOCATED);
}

void *mm_malloc(size_t request) {
  if (!free_list)
    return NULL;
  size_t total = ALIGN(request + ALLOC_OVERHEAD);
  return get_memory(total);
}

void mm_free(void *bp) {
  coalesce(init_footer(flag_deallocated(bp)));
}



static void *get_bestfit(size_t request) {
  void *current = free_list;
  void *best = NULL;
  while (current) {
    if (request <= GET_SIZE_H(current))
      if (!best || GET_SIZE_H(current) < GET_SIZE_H(best))
        best = current;
    current = NEXT_FREE(current);
  }
  if (best)
    set_allocated(best, request);
  return best;
}

static void *get_firstfit(size_t request) {
  void *current = free_list;
  while (current) {
    if (request <= GET_SIZE_H(current))
      break;
    current = NEXT_FREE(current);
  }
  if(current)
    set_allocated(current, request);
  return current;
}

static void set_allocated(void *bp, size_t size) {
  flag_allocated(remove_free_node(bp));
  size_t extra_size = GET_SIZE_H(bp) - size;
  if (extra_size > ALIGN(1 + ALLOC_OVERHEAD))
    split_block(bp, size, extra_size);
}

static void split_block(void *bp, size_t size, size_t extra_size) {
  SET_SIZE_H(bp, size);
  void *split = NEXT_BLKP(bp);
  init_header(split, extra_size, PREV_ALLOCATED);
  mm_free(split);
}
/*
  If the next is empty, it must always be removed from the free list since
    it will be coalesced
  If the previous is empty, it is in the free list so we don't add anything
  Coallescing with a previous block means the footer changes locations, so the
    old footer data must be copied
*/
static void coalesce(void *bp) {
  void *next_bp = NEXT_BLKP(bp);
  int nalloc = get_bit(next_bp, ALLOCATED);
  int palloc = get_bit(bp, PREV_ALLOCATED);
  void *prev_bp = palloc ? NULL : PREV_BLKP(bp);

  size_t coalesced_size = GET_SIZE_H(bp)
  + (palloc ? 0 : GET_SIZE_H(prev_bp))
  + (nalloc ? 0 : GET_SIZE_H(next_bp));

  if (!nalloc) remove_free_node(next_bp);
  void *old_next;
  if (!palloc) {
    bp = prev_bp;
    old_next = NEXT_FREE(prev_bp);
  }

  SET_SIZE_H(bp, coalesced_size);
  init_footer(bp);

  if (palloc) add_free_node(bp);
  else NEXT_FREE(bp) = old_next; //moving footer means we need to reset next
  return;
}
// inserts a node at the start (not the end) of the doubly linked list
static void *add_free_node(void *const node) {
  if (free_list) PREV_FREE(free_list) = node;
  NEXT_FREE(node) = free_list;
  PREV_FREE(node) = NULL;
  free_list = node;
  return node;
}

static void *remove_free_node(void *const node) {
  void *prev = PREV_FREE(node);
  void *next = NEXT_FREE(node);
  if (next) PREV_FREE(next) = PREV_FREE(node);
  if (prev) NEXT_FREE(prev) = NEXT_FREE(node);
  else free_list = next; // no prev means we're deleting the start of the list
  return node;
}



/*
Initializes header with size which must be a multiple of 16
Ensures that lowest 4 bits are zero unless otherwise set with flags
*/
static void *init_header(void *const bp, size_t size, size_t flags) {
  PREV_FREE(bp) = NULL;
  _PAKVAL_(bp) = 0; // make sure lower four bits are zeroed
  SET_SIZE_H(bp, size);
  SET_BIT(bp, flags);
  return bp;
}

static void *init_footer(void *const bp) {
  NEXT_FREE(bp) = NULL;
  SIZE_F(bp) = GET_SIZE_H(bp);
  return bp;
}

static int get_bit(void *bp, Bit which) {
  return GET_BIT(bp, which) > 0 ? 1 : 0;
}

static void *flag_allocated(void *const bp) {
  SET_BIT(bp, ALLOCATED);
  SET_BIT(NEXT_BLKP(bp), PREV_ALLOCATED);
  return bp;
}

static void *flag_deallocated(void *const bp) {
  CLR_BIT(bp, ALLOCATED);
  CLR_BIT(NEXT_BLKP(bp), PREV_ALLOCATED);
  return bp;
}
