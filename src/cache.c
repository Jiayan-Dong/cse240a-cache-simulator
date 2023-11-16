//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"
#include "stdio.h"

//
// TODO:Student Information
//
const char *studentName = "Jiayan Dong";
const char *studentID = "A16593051";
const char *email = "jid001@ucsd.edu";

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;    // Number of sets in the I$
uint32_t icacheAssoc;   // Associativity of the I$
uint32_t icacheHitTime; // Hit Time of the I$

uint32_t dcacheSets;    // Number of sets in the D$
uint32_t dcacheAssoc;   // Associativity of the D$
uint32_t dcacheHitTime; // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive
uint32_t prefetch;

uint32_t blocksize; // Block/Line size
uint32_t memspeed;  // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;      // I$ references
uint64_t icacheMisses;    // I$ misses
uint64_t icachePenalties; // I$ penalties

uint64_t dcacheRefs;      // D$ references
uint64_t dcacheMisses;    // D$ misses
uint64_t dcachePenalties; // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

uint64_t compulsory_miss;
uint64_t other_miss;

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

struct cache_line
{
  uint32_t tag;
  struct cache_line *next;
  struct cache_line *prev;
};

struct cache_line *icache;
struct cache_line *dcache;
struct cache_line *l2cache;

uint32_t icache_index_mask;
uint32_t dcache_index_mask;
uint32_t l2cache_index_mask;

uint8_t icache_index_bit;
uint8_t dcache_index_bit;
uint8_t l2cache_index_bit;

uint8_t block_offset_bit;

uint8_t
get_msb_index(uint32_t n)
{
  uint8_t index = 0;
  do
  {
    n = n >> 1;
    index++;
  } while (n != 0);

  return index - 1;
}

uint8_t
access_cache(uint32_t addr, struct cache_line *cache, uint32_t index_mask, uint8_t index_bit, uint32_t assoc, uint32_t update, uint32_t pre)
{
  uint32_t index = (addr >> block_offset_bit) & index_mask;
  uint32_t tag = addr >> (block_offset_bit + index_bit);

  struct cache_line *p = cache[index].next;
  struct cache_line *p_prev = NULL;
  uint32_t cnt = 0;

  while (p != NULL)
  {
    cnt++;
    if (p->tag == tag)
    {
      if (p->next != NULL)
      {
        p->next->prev = p->prev;
      }

      p->prev->next = p->next;
      p->next = cache[index].next;
      if (cache[index].next)
      {
        cache[index].next->prev = p;
      }
      p->prev = &cache[index];
      cache[index].next = p;

      return TRUE;
    }
    p_prev = p;
    p = p->next;
  }

  if (pre == 0)
  {
    if (p_prev == NULL)
      compulsory_miss++;
    else
      other_miss++;
  }

  if (update == 0)
  {
    return FALSE;
  }

  if (cnt == assoc)
  {
    p_prev->prev->next = NULL;
    free(p_prev);
  }

  p = malloc(sizeof(struct cache_line));
  p->tag = tag;
  p->next = cache[index].next;
  if (cache[index].next)
  {
    cache[index].next->prev = p;
  }
  p->prev = &cache[index];
  cache[index].next = p;

  return FALSE;
}

struct cache_line *i_stream_buffer;
struct cache_line *d_stream_buffer;
struct cache_line *l2_stream_buffer;

uint32_t stream_buffer_size = 4;

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Initialize the Cache Hierarchy
//
void init_cache()
{
  // Initialize cache stats
  icacheRefs = 0;
  icacheMisses = 0;
  icachePenalties = 0;
  dcacheRefs = 0;
  dcacheMisses = 0;
  dcachePenalties = 0;
  l2cacheRefs = 0;
  l2cacheMisses = 0;
  l2cachePenalties = 0;

  compulsory_miss = 0;
  other_miss = 0;

  icache_index_mask = icacheSets - 1;
  dcache_index_mask = dcacheSets - 1;
  l2cache_index_mask = l2cacheSets - 1;

  icache_index_bit = get_msb_index(icacheSets);
  dcache_index_bit = get_msb_index(dcacheSets);
  l2cache_index_bit = get_msb_index(l2cacheSets);
  block_offset_bit = get_msb_index(blocksize);

  icache = calloc(icacheSets, sizeof(struct cache_line));
  dcache = calloc(dcacheSets, sizeof(struct cache_line));
  l2cache = calloc(l2cacheSets, sizeof(struct cache_line));

  i_stream_buffer = calloc(1, sizeof(struct cache_line));
  d_stream_buffer = calloc(1, sizeof(struct cache_line));
  l2_stream_buffer = calloc(1, sizeof(struct cache_line));
}

// Clean Up the Cache Hierarchy
//
void clean_cache()
{
  //
  // TODO: Clean Up Cache Simulator Data Structures
  //
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{
  if (icacheSets == 0)
  {
    return l2cache_access(addr);
  }
  icacheRefs++;
  if (access_cache(addr, i_stream_buffer, 0, 0, stream_buffer_size, 0, 1) == TRUE)
  {
    return icacheHitTime;
  }
  if (access_cache(addr, icache, icache_index_mask, icache_index_bit, icacheAssoc, 1, 0) == TRUE)
  {
    return icacheHitTime;
  }
  else
  {
    icacheMisses++;
    uint32_t access_time = l2cache_access(addr);
    icachePenalties += access_time;
    return access_time + icacheHitTime;
  }
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  if (dcacheSets == 0)
  {
    return l2cache_access(addr);
  }
  dcacheRefs++;
  if (access_cache(addr, d_stream_buffer, 0, 0, stream_buffer_size, 0, 1) == TRUE)
  {
    return dcacheHitTime;
  }
  if (access_cache(addr, dcache, dcache_index_mask, dcache_index_bit, dcacheAssoc, 1, 0) == TRUE)
  {
    return dcacheHitTime;
  }
  else
  {
    dcacheMisses++;
    uint32_t access_time = l2cache_access(addr);
    dcachePenalties += access_time;
    return access_time + dcacheHitTime;
  }
}

void l2cache_prefetch(uint32_t addr)
{
  if (l2cacheSets == 0)
    return;
  access_cache(addr + blocksize, l2_stream_buffer, 0, 0, stream_buffer_size, 1, 1);
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  if (l2cacheSets == 0)
  {
    return memspeed;
  }
  l2cacheRefs++;
  if (access_cache(addr, l2_stream_buffer, 0, 0, stream_buffer_size, 0, 1) == TRUE)
  {
    l2cache_prefetch(addr + blocksize);
    return l2cacheHitTime;
  }
  if (access_cache(addr, l2cache, l2cache_index_mask, l2cache_index_bit, l2cacheAssoc, 1, 0) == TRUE)
  {
    l2cache_prefetch(addr + blocksize);
    return l2cacheHitTime;
  }
  else
  {
    l2cache_prefetch(addr + blocksize);
    l2cacheMisses++;
    l2cachePenalties += memspeed;
    return memspeed + l2cacheHitTime;
  }
}

void icache_prefetch(uint32_t addr)
{
  if (icacheSets == 0)
  {
    return;
  }

  access_cache(addr + blocksize, i_stream_buffer, 0, 0, stream_buffer_size, 1, 1);
}

void dcache_prefetch(uint32_t addr)
{
  if (dcacheSets == 0)
  {
    return;
  }

  access_cache(addr + blocksize, d_stream_buffer, 0, 0, stream_buffer_size, 1, 1);
}
