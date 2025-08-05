#include "server/hashtable.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * TODO: Return an appropriate error code to the caller. So they can determine
 * the correct course of action.
 *
 * Allow caller to specify hashing fn.
 */

static uint32_t hash_int(int key) { return (uint32_t)(key * 2654435761u); }

static uint32_t hash_string(const char *key) {
  uint32_t hash = 2166136261u;
  size_t len = strlen((char *)key);
  for (size_t i = 0; i < len; i++) {
    hash ^= (uint8_t)((char *)key)[i];
    hash *= 16777619;
  }
  return hash;
}

static uint32_t make_hash(HashTableKeyType key_type, const void *key) {
  switch (key_type) {
  case HT_KEY_STRING:
    return hash_string((char *)key);
  case HT_KEY_INT:
    return hash_int(*(int *)key);
  default:
    assert(0 && "Unsupported key type");
  }
}

static int compare(HashTableKeyType key_type, const void *p1, const void *p2) {
  switch (key_type) {
  case HT_KEY_STRING:
    return strcmp((const char *)p1, (const char *)p2) == 0;
  case HT_KEY_INT:
    return *(int *)p1 == *(int *)p2;
  default:
    assert(0 && "Unsupported key type");
  }
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

HashTable *ht_create(HashTableKeyType key_type) {
  HashTable *ht = malloc(sizeof(HashTable));
  if (!ht) {
    return NULL;
  }

  ht->count = 0;
  ht->capacity = 0;
  ht->entries = NULL;
  ht->key_type = key_type;
  return ht;
}

static HashTableEntry *ht_create_entry(HashTable *ht, const void *key,
                                       const HashTableValue value,
                                       const uint32_t hash) {
  HashTableEntry *entry = malloc(sizeof(HashTableEntry));
  if (!entry) {
    return NULL;
  }

  switch (ht->key_type) {
  case HT_KEY_INT:
    entry->key = malloc(sizeof(int));
    if (!entry->key) {
      free(entry);
      return NULL;
    }
    memcpy(entry->key, key, sizeof(int));
    break;
  case HT_KEY_STRING:
    entry->key = strdup(key);
    if (!entry->key) {
      free(entry);
      return NULL;
    }
    break;
  }

  entry->value = malloc(value.size);
  if (!entry->value) {
    free(entry->key);
    free(entry);
    return NULL;
  }
  memcpy(entry->value, value.value, value.size);
  entry->hash = hash;
  entry->next = NULL;
  return entry;
}

int ht_set(HashTable *ht, const void *key, const HashTableValue value) {
  if ((ht->capacity == 0 ||
       (double)(ht->count + 1) / (double)ht->capacity > LOAD_FACTOR) &&
      ht_resize(ht) == -1) {
    return -1;
  }

  uint32_t hash = make_hash(ht->key_type, key);
  size_t index = hash % ht->capacity;
  HashTableEntry *cur = ht->entries[index];
  if (!cur) {
    HashTableEntry *entry = ht_create_entry(ht, key, value, hash);
    if (!entry) {
      return -1;
    }
    ht->entries[index] = entry;
    ht->count++;
    return 0;
  }

  while (cur) {
    if (compare(ht->key_type, cur->key, key)) {
      free(cur->value);
      cur->value = malloc(value.size);
      if (!cur->value) {
        return -1;
      }
      memcpy(cur->value, value.value, value.size);
      return 0;
    }

    if (!cur->next) {
      break;
    }
    cur = cur->next;
  }

  cur->next = ht_create_entry(ht, key, value, hash);
  if (!cur->next) {
    return -1;
  }

  ht->count++;
  return 0;
}

void *ht_get(HashTable *ht, const void *key) {
  uint32_t hash = make_hash(ht->key_type, key);
  size_t index = hash % ht->capacity;
  HashTableEntry *cur = ht->entries[index];

  while (cur) {
    if (compare(ht->key_type, cur->key, key)) {
      return cur->value;
    }

    cur = cur->next;
  }

  return NULL;
}

void ht_print(HashTable *ht, PrintCallback print) {
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
        print(cur);
        cur = cur->next;
        first = 0;
      }
      printf("\n");
    }
  }
  printf("============================\n");
}
