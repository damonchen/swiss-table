#include "swiss-table.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8
#define LOAD_FACTOR_THRESHOLD 0.7
#define EMPTY_HASH 0
#define DELETED_HASH 1

typedef struct entry {
    uint64_t hash;  // 存储哈希值，0表示空槽，1表示已删除
    char* key;
    void* value;
} entry_t;

struct swiss_table {
    entry_t* entries;
    size_t capacity;
    size_t size;
    size_t deleted;
};

// FNV-1a 哈希函数
static uint64_t hash_key(const char* key) {
    uint64_t hash = 14695981039346656037ULL; // FNV偏移基数
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= 1099511628211ULL; // FNV质数
    }
    return hash;
}

swiss_table_t* swiss_table_new(void) {
    swiss_table_t* table = malloc(sizeof(swiss_table_t));
    if (!table) return NULL;

    table->capacity = INITIAL_CAPACITY;
    table->size = 0;
    table->deleted = 0;
    table->entries = calloc(table->capacity, sizeof(entry_t));
    
    if (!table->entries) {
        free(table);
        return NULL;
    }
    
    return table;
}

static bool swiss_table_resize(swiss_table_t* table, size_t new_capacity) {
    entry_t* new_entries = calloc(new_capacity, sizeof(entry_t));
    if (!new_entries) return false;

    // 重新哈希所有现有条目
    for (size_t i = 0; i < table->capacity; i++) {
        entry_t* entry = &table->entries[i];
        if (entry->hash <= DELETED_HASH) continue;

        size_t index = entry->hash % new_capacity;
        while (new_entries[index].hash != EMPTY_HASH) {
            index = (index + 1) % new_capacity;
        }
        new_entries[index] = *entry;
    }

    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    table->deleted = 0;
    return true;
}

bool swiss_table_insert(swiss_table_t* table, const char* key, void* value) {
    if ((table->size + table->deleted + 1) > table->capacity * LOAD_FACTOR_THRESHOLD) {
        if (!swiss_table_resize(table, table->capacity * 2)) {
            return false;
        }
    }

    uint64_t hash = hash_key(key);
    if (hash <= DELETED_HASH) hash += 2;
    
    size_t index = hash % table->capacity;
    while (true) {
        entry_t* entry = &table->entries[index];
        
        if (entry->hash == EMPTY_HASH || entry->hash == DELETED_HASH) {
            entry->hash = hash;
            entry->key = strdup(key);
            entry->value = value;
            table->size++;
            return true;
        }
        
        if (entry->hash == hash && strcmp(entry->key, key) == 0) {
            entry->value = value;
            return true;
        }
        
        index = (index + 1) % table->capacity;
    }
}

void* swiss_table_get(swiss_table_t* table, const char* key) {
    uint64_t hash = hash_key(key);
    if (hash <= DELETED_HASH) hash += 2;
    
    size_t index = hash % table->capacity;
    while (true) {
        entry_t* entry = &table->entries[index];
        
        if (entry->hash == EMPTY_HASH) {
            return NULL;
        }
        
        if (entry->hash == hash && strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        
        if (entry->hash == DELETED_HASH) {
            index = (index + 1) % table->capacity;
            continue;
        }
        
        index = (index + 1) % table->capacity;
    }
}

bool swiss_table_remove(swiss_table_t* table, const char* key) {
    uint64_t hash = hash_key(key);
    if (hash <= DELETED_HASH) hash += 2;
    
    size_t index = hash % table->capacity;
    while (true) {
        entry_t* entry = &table->entries[index];
        
        if (entry->hash == EMPTY_HASH) {
            return false;
        }
        
        if (entry->hash == hash && strcmp(entry->key, key) == 0) {
            free(entry->key);
            entry->hash = DELETED_HASH;
            entry->key = NULL;
            entry->value = NULL;
            table->size--;
            table->deleted++;
            return true;
        }
        
        index = (index + 1) % table->capacity;
    }
}

void swiss_table_free(swiss_table_t* table) {
    if (!table) return;
    
    for (size_t i = 0; i < table->capacity; i++) {
        if (table->entries[i].hash > DELETED_HASH) {
            free(table->entries[i].key);
        }
    }
    
    free(table->entries);
    free(table);
}

size_t swiss_table_size(const swiss_table_t* table) {
    return table->size;
} 

