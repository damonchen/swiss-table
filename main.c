#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "swiss-table.h"

// 用于测试的简单结构体
typedef struct {
    int id;
    char name[32];
} User;

// 辅助函数：创建用户
User* create_user(int id, const char* name) {
    User* user = malloc(sizeof(User));
    user->id = id;
    strncpy(user->name, name, 31);
    user->name[31] = '\0';
    return user;
}

void test_basic_operations() {
    printf("Testing basic operations...\n");
    
    swiss_table_t* table = swiss_table_new();
    assert(table != NULL);
    assert(swiss_table_size(table) == 0);

    // 测试插入
    User* user1 = create_user(1, "Alice");
    assert(swiss_table_insert(table, "user1", user1));
    assert(swiss_table_size(table) == 1);

    // 测试查询
    User* found = swiss_table_get(table, "user1");
    assert(found != NULL);
    assert(found->id == 1);
    assert(strcmp(found->name, "Alice") == 0);

    // 测试更新
    User* user1_new = create_user(1, "Alice Updated");
    assert(swiss_table_insert(table, "user1", user1_new));
    free(user1);

    found = swiss_table_get(table, "user1");
    assert(found != NULL);
    assert(strcmp(found->name, "Alice Updated") == 0);

    // 测试删除
    assert(swiss_table_remove(table, "user1"));
    assert(swiss_table_size(table) == 0);
    assert(swiss_table_get(table, "user1") == NULL);

    free(user1_new);
    swiss_table_free(table);
    printf("Basic operations test passed!\n");
}

void test_multiple_items() {
    printf("Testing multiple items...\n");
    
    swiss_table_t* table = swiss_table_new();
    const int test_size = 100;
    User* users[test_size];
    char keys[test_size][32];

    // 插入多个项目
    for (int i = 0; i < test_size; i++) {
        snprintf(keys[i], sizeof(keys[i]), "user%d", i);
        users[i] = create_user(i, keys[i]);
        assert(swiss_table_insert(table, keys[i], users[i]));
    }

    assert(swiss_table_size(table) == test_size);

    // 验证所有项目
    for (int i = 0; i < test_size; i++) {
        User* found = swiss_table_get(table, keys[i]);
        assert(found != NULL);
        assert(found->id == i);
    }

    // 删除一半的项目
    for (int i = 0; i < test_size / 2; i++) {
        assert(swiss_table_remove(table, keys[i]));
        free(users[i]);
    }

    assert(swiss_table_size(table) == test_size / 2);

    // 清理剩余项目
    for (int i = test_size / 2; i < test_size; i++) {
        free(users[i]);
    }

    swiss_table_free(table);
    printf("Multiple items test passed!\n");
}

void test_edge_cases() {
    printf("Testing edge cases...\n");
    
    swiss_table_t* table = swiss_table_new();

    // 测试空键
    User* user = create_user(1, "Test");
    assert(swiss_table_insert(table, "", user));
    assert(swiss_table_get(table, "") == user);
    assert(swiss_table_remove(table, ""));
    free(user);

    // 测试NULL值
    assert(swiss_table_insert(table, "null_value", NULL));
    assert(swiss_table_get(table, "null_value") == NULL);
    assert(swiss_table_remove(table, "null_value"));

    // 测试不存在的键
    assert(swiss_table_get(table, "nonexistent") == NULL);
    assert(!swiss_table_remove(table, "nonexistent"));

    swiss_table_free(table);
    printf("Edge cases test passed!\n");
}

int main() {
    printf("Starting Swiss Table tests...\n\n");

    test_basic_operations();
    printf("\n");
    
    test_multiple_items();
    printf("\n");
    
    test_edge_cases();
    printf("\n");

    printf("All tests passed successfully!\n");
    return 0;
} 