#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include <stdint.h>

#define LOAD_FACTOR 0.75

typedef struct HashTableEntry HashTableEntry;

typedef struct HashTableEntry {
  char *key;
  char *value;
  uint32_t hash;
  HashTableEntry *next;
} HashTableEntry;

typedef struct {
  HashTableEntry **entries; // points to head of linked list.
  size_t capacity;
  size_t count;
} HashTable;

HashTable *ht_create();
int ht_set(HashTable *ht, const char *key, const char *value);
char *ht_get(HashTable *ht, const char *key);
void ht_destroy(HashTable *ht);
void ht_print(HashTable *ht);

#endif
