#ifndef _HMAP_H
 #define _HMAP_H
 
#define MAP_BUCKET          1024
#define MAX_SIZE_KEY        1024

enum hmap_error_enum{
    HMAP_SUCCESS = 0,
    HMAP_FAILED = -1,
    HMAP_ALLOCATED_ERROR = -100,
    HMAP_CREATE_DB_ERROR = -101,
    HMAP_CREATE_TUPUL_ERROR = -102,
    HMAP_DB_EMPTY = -401,
    HMAP_TUPUL_EMPTY = -403,
    HMAP_KEY_EMPTY = -404,
    HMAP_TUPLE_NOT_FOUND = -405,
    HMAP_TUPLE_DUPLICATE = -406,
    HMAP_TUPLE_ALREADY_EXIST = -407,

    HMAP_ALL
};

enum hmap_tuple_type{
    HMAP_TUPLE_PRIMARY = 0,
    HMAP_TUPLE_SECONDARY
};

typedef struct tuple{
    struct tuple *next;
    struct tuple *prev;
    struct tuple *hash_next;
    struct tuple *hash_prev;
    
    int key_len;
    int data_len;
    int index;
    
    char key[MAX_SIZE_KEY];
    void *data;
    
    int type;
}TUPLE;

typedef struct hmap_db{
    int tuple_count;
    int secondary_tuple_count;
    int bucket_size;
    TUPLE *tuple;
    TUPLE *list_tuple;
}HMAP_DB;

int hmap_init(int bucket_size, HMAP_DB **hmap_db);
int hmap_is_locate(HMAP_DB *hmap_db, void *key, int k_len);
int hmap_add(HMAP_DB **hmap_db, void *key, int k_len, void *data, int d_len, int dup_flag);
int hmap_add_tuple_with_data(HMAP_DB **hmap_db, void *key, int k_len, void *data, int dup_flag);
int hmap_delete(HMAP_DB **hmap_db, void *key, int k_len);
int hmap_set(HMAP_DB *hmap_db, void *key, int k_len, void *data, int d_len, int alway_set);
int hmap_print_table(HMAP_DB *hmap_db);
int hmap_print_list(HMAP_DB *hmap_db);
int hmap_print_tree( HMAP_DB *my_hmap_db );
int hmap_search(HMAP_DB *hmap_db, void *key, int k_len, TUPLE **data);
int hmap_destroy(HMAP_DB **hmap_db);
int hmap_truncate(HMAP_DB **hamp_db);

#endif