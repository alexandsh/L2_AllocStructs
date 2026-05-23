#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "allocator.h"
#include <stddef.h>

typedef struct HashNode {
  const char *key;
  void *value;
  struct HashNode *next;
} HashNode;

typedef struct {
  IAllocator *alloc;
  HashNode **buckets;
  size_t num_buckets;
} HashTable;

void hash_table_init(HashTable *table, IAllocator *alloc, size_t num_buckets);
int hash_table_insert(HashTable *table, const char *key, void *value);
void *hash_table_get(HashTable *table, const char *key);
int hash_table_remove(HashTable *table, const char *key);
void hash_table_destroy(HashTable *table);

#endif
