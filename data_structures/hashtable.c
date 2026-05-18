#include "hash_table.h"
#include "helpers.h"
#include <string.h>

static unsigned int default_hash(const char *key, size_t buckets) {
  unsigned int hash = 5381;
  while (*key) {
    hash = ((hash << 5) + hash) + *key++;
  }
  return hash % buckets;
}

void hash_table_init(HashTable *table, IAllocator *alloc, size_t num_buckets) {
  table->alloc = alloc;
  table->num_buckets = num_buckets > 0 ? num_buckets : 16;
  table->buckets =
      (HashNode **)i_alloc(alloc, table->num_buckets * sizeof(HashNode *));
  if (table->buckets) {
    for (size_t i = 0; i < table->num_buckets; i++) {
      table->buckets[i] = NULL;
    }
  }
}

int hash_table_insert(HashTable *table, const char *key, void *value) {
  if (!table->buckets)
    return 0;

  unsigned int index = default_hash(key, table->num_buckets);
  HashNode *curr = table->buckets[index];

  while (curr != NULL) {
    if (strcmp(curr->key, key) == 0) {
      curr->value = value;
      return 1;
    }
    curr = curr->next;
  }

  HashNode *new_node = (HashNode *)i_alloc(table->alloc, sizeof(HashNode));
  if (!new_node)
    return 0;

  new_node->key = key;
  new_node->value = value;
  new_node->next = table->buckets[index];
  table->buckets[index] = new_node;

  return 1;
}

void *hash_table_get(HashTable *table, const char *key) {
  if (!table->buckets)
    return NULL;

  unsigned int index = default_hash(key, table->num_buckets);
  HashNode *curr = table->buckets[index];

  while (curr != NULL) {
    if (strcmp(curr->key, key) == 0) {
      return curr->value;
    }
    curr = curr->next;
  }
  return NULL;
}

int hash_table_remove(HashTable *table, const char *key) {
  if (!table->buckets)
    return 0;

  unsigned int index = default_hash(key, table->num_buckets);
  HashNode *curr = table->buckets[index];
  HashNode *prev = NULL;

  while (curr != NULL) {
    if (strcmp(curr->key, key) == 0) {
      if (prev == NULL) {
        table->buckets[index] = curr->next;
      } else {
        prev->next = curr->next;
      }
      i_free(table->alloc, curr);
      return 1;
    }
    prev = curr;
    curr = curr->next;
  }
  return 0;
}

void hash_table_destroy(HashTable *table) {
  if (!table->buckets)
    return;

  for (size_t i = 0; i < table->num_buckets; i++) {
    HashNode *curr = table->buckets[i];
    while (curr != NULL) {
      HashNode *temp = curr;
      curr = curr->next;
      i_free(table->alloc, temp);
    }
  }
  i_free(table->alloc, table->buckets);
  table->buckets = NULL;
  table->num_buckets = 0;
}
