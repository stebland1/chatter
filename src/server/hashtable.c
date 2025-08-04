#include "server/hashtable.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t hash_string(const char *key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

void ht_destroy(HashTable *ht) {
  for (size_t i = 0; i < ht->capacity; i++) {
    HashTableEntry *cur = ht->entries[i];

    while (cur) {
      HashTableEntry *next = cur->next;

      free(cur->key);
      free(cur->value);
      free(cur);

      cur = next;
    }

    ht->entries[i] = NULL;
  }

  free(ht->entries);
  free(ht);
}

static int ht_resize(HashTable *ht) {
  size_t new_capacity = ht->capacity > 0 ? ht->capacity * 2 : 8;
  HashTableEntry **new_entries = calloc(new_capacity, sizeof(HashTableEntry *));
  if (!new_entries) {
    ht_destroy(ht);
    return -1;
  }

  for (size_t i = 0; i < ht->capacity; i++) {
    HashTableEntry *cur = ht->entries[i];
    if (!cur) {
      continue;
    }

    while (cur) {
      HashTableEntry *next = cur->next;
      size_t index = cur->hash % new_capacity;
      cur->next = new_entries[index];
      new_entries[index] = cur;
      cur = next;
    }
  }

  free(ht->entries);
  ht->entries = new_entries;
  ht->capacity = new_capacity;
  return 0;
}

HashTable *ht_create() {
  HashTable *ht = malloc(sizeof(HashTable));
  if (!ht) {
    return NULL;
  }

  ht->count = 0;
  ht->capacity = 0;
  ht->entries = NULL;
  return ht;
}

static HashTableEntry *ht_create_entry(const char *key, const char *value,
                                       const uint32_t hash) {
  HashTableEntry *entry = malloc(sizeof(HashTableEntry));
  entry->key = strdup(key);
  if (!entry->key) {
    return NULL;
  }

  // TODO: add struct storage.
  // I think to do this I'd need to use memcpy to copy the raw bytes.
  // I'd need to accept value as a void ptr.
  // and accept the size of the value being stored as part of the API.
  entry->value = strdup(value);
  if (!entry->value) {
    free(entry->key);
    return NULL;
  }

  entry->hash = hash;
  return entry;
}

int ht_set(HashTable *ht, const char *key, const char *value) {
  if ((ht->capacity == 0 ||
       (double)(ht->count + 1) / (double)ht->capacity > LOAD_FACTOR) &&
      ht_resize(ht) == -1) {
    return -1;
  }

  // TODO: add integer hashing support
  uint32_t hash = hash_string(key, strlen(key));
  size_t index = hash % ht->capacity;
  HashTableEntry *cur = ht->entries[index];
  if (!cur) {
    HashTableEntry *entry = ht_create_entry(key, value, hash);
    if (!entry) {
      return -1;
    }
    ht->entries[index] = entry;
    ht->count++;
    return 0;
  }

  while (cur) {
    if (strcmp(cur->key, key) == 0) {
      free(cur->value);
      cur->value = strdup(value);
      return cur->value ? 0 : -1;
    }

    if (!cur->next) {
      break;
    }
    cur = cur->next;
  }

  cur->next = ht_create_entry(key, value, hash);
  if (!cur->next) {
    return -1;
  }

  ht->count++;
  return 0;
}

char *ht_get(HashTable *ht, const char *key) {
  uint32_t hash = hash_string(key, strlen(key));
  int index = hash % ht->capacity;
  HashTableEntry *cur = ht->entries[index];

  if (!cur) {
    return NULL;
  }

  while (cur) {
    if (strcmp(cur->key, key) == 0) {
      return cur->value;
    }

    cur = cur->next;
  }

  return NULL;
}

void ht_print(HashTable *ht) {
  printf("============================\n");
  printf("Num items: %zu\n", ht->count);

  for (size_t i = 0; i < ht->capacity; i++) {
    HashTableEntry *cur = ht->entries[i];
    printf("[%zu]: ", i);
    if (!cur) {
      printf("NULL\n");
    } else {
      int first = 1;
      while (cur) {
        if (!first) {
          printf(" -> ");
        }
        printf("[%s, %s]", cur->key, cur->value);
        cur = cur->next;
        first = 0;
      }
      printf("\n");
    }
  }
  printf("============================\n");
}
