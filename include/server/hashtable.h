#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include <stdint.h>

#define LOAD_FACTOR 0.75

typedef struct HashTableEntry HashTableEntry;

typedef struct HashTableEntry {
  char *key;
  void *value;
  uint32_t hash;
  HashTableEntry *next;
} HashTableEntry;

typedef struct {
  void *value;
  size_t size;
} HashTableValue;

typedef uint32_t (*HashCallback)(void *item);
typedef void (*PrintCallback)(HashTableEntry *item);

typedef enum {
  HT_KEY_STRING,
  HT_KEY_INT,
} HashTableKeyType;

typedef struct {
  HashTableEntry **entries; // points to head of linked list.
  size_t capacity;
  size_t count;
  HashTableKeyType key_type;
} HashTable;

HashTable *ht_create(HashTableKeyType key_type);
int ht_set(HashTable *ht, const void *key, const HashTableValue value);
void *ht_get(HashTable *ht, const void *key);
void ht_destroy(HashTable *ht);
void ht_print(HashTable *ht, PrintCallback print);

#endif
