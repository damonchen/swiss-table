#ifndef SWISS_TABLE_H
#define SWISS_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct swiss_table swiss_table_t;

// 创建新的哈希表
swiss_table_t* swiss_table_new(void);

// 插入键值对
bool swiss_table_insert(swiss_table_t* table, const char* key, void* value);

// 查找键对应的值
void* swiss_table_get(swiss_table_t* table, const char* key);

// 删除键值对
bool swiss_table_remove(swiss_table_t* table, const char* key);

// 释放哈希表
void swiss_table_free(swiss_table_t* table);

// 获取当前元素数量
size_t swiss_table_size(const swiss_table_t* table);

#endif // SWISS_TABLE_H 