#include "hmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEUBUG(...) do {printf( __VA_ARGS__ );}while(0)

#define VALIDATE_DB(_hdb, _key, _klen) do{  \
    if( _hdb == NULL ){                     \
        return HMAP_DB_EMPTY;               \
    }                                       \
    if( _key == NULL || _klen <= 0){        \
        return HMAP_KEY_EMPTY;              \
    }                                       \
}while(0)


#define HDB_LIST_APPEND(_first,_item)                                 \
{                                                                     \
   if ((_first) == NULL)                                              \
   {                                                                  \
      (_first) = (_item)->prev = (_item)->next = (_item);             \
   }                                                                  \
   else                                                               \
   {                                                                  \
      (_item)->prev = (_first)->prev;                                 \
      (_item)->next = (_first);                                       \
      (_first)->prev->next = (_item);                                 \
      (_first)->prev = (_item);                                       \
   }                                                                  \
}
#define HDB_LIST_REMOVE(_first,_item)                                 \
{                                                                     \
   if ((_first) == (_item))                                           \
   {                                                                  \
      if ((_first)->next == (_first))                                 \
         (_first) = NULL;                                             \
      else                                                            \
      {                                                               \
         (_first) = (_item)->next;                                    \
         (_item)->next->prev = (_item)->prev;                         \
         (_item)->prev->next = (_item)->next;                         \
      }                                                               \
   }                                                                  \
   else                                                               \
   {                                                                  \
      (_item)->next->prev = (_item)->prev;                            \
      (_item)->prev->next = (_item)->next;                            \
   }                                                                  \
   (_item)->prev = (_item)->next = NULL;                              \
}

#define HDB_HASH_APPEND(_first,_item)                                 \
{                                                                     \
   if ((_first) == NULL)                                              \
   {                                                                  \
      (_first) = (_item)->hash_prev = (_item)->hash_next = (_item);   \
   }                                                                  \
   else                                                               \
   {                                                                  \
      (_item)->hash_prev = (_first)->hash_prev;                       \
      (_item)->hash_next = (_first);                                  \
      (_first)->hash_prev->hash_next = (_item);                       \
      (_first)->hash_prev = (_item);                                  \
   }                                                                  \
}

#define HDB_HASH_REMOVE(_first,_item)                                 \
{                                                                     \
   if ((_first) == (_item))                                           \
   {                                                                  \
      if ((_first)->hash_next == (_first))                            \
         (_first) = NULL;                                             \
      else                                                            \
      {                                                               \
         (_first) = (_item)->hash_next;                               \
         (_item)->hash_next->hash_prev = (_item)->hash_prev;          \
         (_item)->hash_prev->hash_next = (_item)->hash_next;          \
      }                                                               \
   }                                                                  \
   else                                                               \
   {                                                                  \
      (_item)->hash_next->hash_prev = (_item)->hash_prev;             \
      (_item)->hash_prev->hash_next = (_item)->hash_next;             \
   }                                                                  \
   (_item)->hash_prev = (_item)->hash_next = NULL;                    \
}

#define IS_NEXT_HASH(_first, _curr){        \
    if( _curr->hash_next == _first ){       \
        break;                              \
    }                                       \
    _curr = _curr->hash_next;               \
}

#define IS_NEXT_LIST(_first, _curr){        \
    if( _curr->next == _first ){            \
        break;                              \
    }                                       \
    _curr = _curr->next;                    \
}


static int
hmap_cmp(void *v1, int v1_len, void *v2, int v2_len){
    if(v1_len != v2_len){
        return -1;
    }
    return memcmp(v1, v2, v1_len);
}

static int
hash (void *key, int len){
   unsigned int hash, i;

   for (hash = i = 0; i < len; ++i)
   {
      hash += ((unsigned char *)key)[i];
      hash += (hash << 10);
      hash ^= (hash >> 6);
   }
   hash += (hash << 3);
   hash ^= (hash >> 11);
   hash += (hash << 15);
   return hash & 0x7FFFFFFF;
}

int hmap_init(int bucket_size, HMAP_DB **hmap_db){

     HMAP_DB *new_hmap_db = (HMAP_DB *)calloc(1, sizeof(HMAP_DB));
     if(new_hmap_db == NULL){
        return HMAP_CREATE_DB_ERROR;
    }
    
    new_hmap_db->bucket_size = bucket_size;
    new_hmap_db->tuple_count = 0;
    new_hmap_db->list_tuple = NULL;
    new_hmap_db->tuple = (TUPLE *)calloc(bucket_size, sizeof(TUPLE));
    
    if( new_hmap_db->tuple == NULL){
        return HMAP_CREATE_TUPUL_ERROR;
    }   
    
    *hmap_db = new_hmap_db;
    
    return HMAP_SUCCESS;
}

int hmap_search(HMAP_DB *hmap_db, void *key, int k_len, TUPLE **data){

    VALIDATE_DB(hmap_db, key, k_len);
    
    int index = hash(key, k_len)%hmap_db->bucket_size;
    TUPLE *ptr_tuple = (hmap_db->tuple+index);
    TUPLE *ptr_hash_tuple = NULL;
    
    if( ptr_tuple == NULL){
        return HMAP_TUPUL_EMPTY;
    }
    
    TUPLE *itr_tuple = ptr_tuple;
    while( itr_tuple ){
        ptr_hash_tuple = itr_tuple;
        while( ptr_hash_tuple ){
        
            if( hmap_cmp(ptr_hash_tuple->key, ptr_hash_tuple->key_len, key, k_len) == 0){
                if( data != NULL){
                    *data = ptr_hash_tuple;
                }
                return HMAP_SUCCESS;
            }
            
            if( ptr_hash_tuple->hash_next == itr_tuple ){
                break;
            }
            ptr_hash_tuple = ptr_hash_tuple->hash_next;
        }
        
        if( itr_tuple->hash_next == ptr_tuple ){
            break;
        }
        itr_tuple = itr_tuple->hash_next;
    }
    
    return HMAP_TUPLE_NOT_FOUND;
}

int hmap_is_locate(HMAP_DB *hmap_db, void *key, int k_len){
    return hmap_search(hmap_db, key, k_len, NULL);
}

int hmap_add(HMAP_DB **hmap_db, void *key, int k_len, void *data, int d_len, int dup_flag){
    
    VALIDATE_DB(*hmap_db, key, k_len);
    
    TUPLE *new_tuple = NULL, *ptr_tuple = NULL, *root_tuple = NULL ;
    int r = 0;
    int index = hash(key, k_len) % (*hmap_db)->bucket_size;
    
    r = hmap_search(*hmap_db, key, k_len, &ptr_tuple);
    
    if ( r == HMAP_SUCCESS ){
        return HMAP_TUPLE_ALREADY_EXIST;
    }
    
    if ( r  == HMAP_TUPLE_NOT_FOUND ){
    
        root_tuple = ((*hmap_db)->tuple + index);
        
        if( root_tuple == NULL ){
            return HMAP_TUPUL_EMPTY;
        }
        
        if(root_tuple->key_len == 0){
        
            root_tuple->data = (char *)calloc(1, d_len+1);
            
            if( root_tuple->data == NULL){
                return HMAP_ALLOCATED_ERROR;
            }
            
            if ( memcpy( root_tuple->data, data, d_len ) == NULL){
                free(root_tuple->data);
                return  HMAP_FAILED;
            }
            
            ((char *)root_tuple->data)[d_len] = 0;
            root_tuple->data_len = d_len;
            
            DEUBUG("add[%d] primary data[%s]\n", index, (char *)root_tuple->data);
            memcpy(root_tuple->key, key, k_len);
            root_tuple->key[k_len] = 0;
            root_tuple->key_len = k_len;
            root_tuple->index = index;
            root_tuple->type = HMAP_TUPLE_PRIMARY;
            
            if( root_tuple->hash_next == NULL && root_tuple->hash_prev == NULL){
                root_tuple->hash_next = root_tuple->hash_prev = root_tuple;
            }else{
                HDB_HASH_APPEND(root_tuple, root_tuple);
            }
            HDB_LIST_APPEND((*hmap_db)->list_tuple, root_tuple);
           
            
        }else{
            new_tuple = (TUPLE *) calloc(1, sizeof(TUPLE));

            if( new_tuple == NULL ){
                return HMAP_ALLOCATED_ERROR;
            } 
            
            new_tuple->data = (char *)calloc(1, d_len+1);
            
            if( new_tuple->data == NULL){
                free(new_tuple);
                return HMAP_ALLOCATED_ERROR;
            }
            
            if ( memcpy( new_tuple->data, data, d_len ) == NULL ){
                free(new_tuple->data);
                free(new_tuple);
                return  HMAP_FAILED;
            }
            
            ((char *)new_tuple->data)[d_len] = 0;
            new_tuple->data_len = d_len;
            
            DEUBUG("add[%d] secondary data[%s]\n", index, (char *)new_tuple->data);
            memcpy(new_tuple->key, key, k_len);
            new_tuple->key[k_len] = 0;
            new_tuple->key_len = k_len;
            new_tuple->index = index;
            new_tuple->hash_next = new_tuple->hash_prev = NULL;
            new_tuple->next = new_tuple->prev = NULL;
            new_tuple->type = HMAP_TUPLE_SECONDARY;
            (*hmap_db)->secondary_tuple_count++;
            HDB_LIST_APPEND((*hmap_db)->list_tuple, new_tuple);
            HDB_HASH_APPEND(root_tuple, new_tuple);
        }
        
    }else{
        return r;
    }
    
    (*hmap_db)->tuple_count++;

    return index;
}

int hmap_add_tuple_with_data(HMAP_DB **hmap_db, void *key, int k_len, void *data, int dup_flag){
    
    VALIDATE_DB(*hmap_db, key, k_len);
    
    TUPLE *new_tuple = NULL, *ptr_tuple = NULL, *root_tuple = NULL ;
    int r = 0;
    int index = hash(key, k_len) % (*hmap_db)->bucket_size;
    
    r = hmap_search(*hmap_db, key, k_len, &ptr_tuple);
    
    if ( r == HMAP_SUCCESS ){
        return HMAP_TUPLE_ALREADY_EXIST;
    }
    
    if ( r  == HMAP_TUPLE_NOT_FOUND ){
    
        root_tuple = ((*hmap_db)->tuple + index);
        
        if( root_tuple == NULL ){
            return HMAP_TUPUL_EMPTY;
        }
        /* free slot */
        if(root_tuple->key_len == 0){
        
            root_tuple->data = data;
            root_tuple->data_len = -1;
            
            DEUBUG("add[%d] primary data[%s]\n", index, (char *)root_tuple->data);
            memcpy(root_tuple->key, key, k_len);
            root_tuple->key[k_len] = 0;
            root_tuple->key_len = k_len;
            root_tuple->index = index;
            root_tuple->type = HMAP_TUPLE_PRIMARY;
            
            if( root_tuple->hash_next == NULL && root_tuple->hash_prev == NULL){
                root_tuple->hash_next = root_tuple->hash_prev = root_tuple;
            }else{
                HDB_HASH_APPEND(root_tuple, root_tuple);
            }
            HDB_LIST_APPEND((*hmap_db)->list_tuple, root_tuple);
           
            
        }else{
            new_tuple = (TUPLE *) calloc(1, sizeof(TUPLE));

            if( new_tuple == NULL ){
                return HMAP_ALLOCATED_ERROR;
            } 
            
            new_tuple->data = data;
            new_tuple->data_len = -1;
            
            DEUBUG("add[%d] secondary data[%s]\n", index, (char *)new_tuple->data);
            memcpy(new_tuple->key, key, k_len);
            new_tuple->key[k_len] = 0;
            new_tuple->key_len = k_len;
            new_tuple->index = index;
            new_tuple->hash_next = new_tuple->hash_prev = NULL;
            new_tuple->next = new_tuple->prev = NULL;
            new_tuple->type = HMAP_TUPLE_SECONDARY;
            (*hmap_db)->secondary_tuple_count++;
            HDB_LIST_APPEND((*hmap_db)->list_tuple, new_tuple);
            HDB_HASH_APPEND(root_tuple, new_tuple);
        }
        
    }else{
        return r;
    }
    
    (*hmap_db)->tuple_count++;

    return index;
}

int hmap_delete(HMAP_DB **hmap_db, void *key, int k_len){

    VALIDATE_DB(*hmap_db, key, k_len);

    TUPLE *root_tuple = NULL, *ptr_tuple = NULL;
    int r = hmap_search(*hmap_db, key, k_len, &ptr_tuple);
    
    if( r == HMAP_SUCCESS ){
        HDB_LIST_REMOVE((*hmap_db)->list_tuple, ptr_tuple);
        root_tuple = (*hmap_db)->tuple+ptr_tuple->index;
        if(root_tuple == ptr_tuple){
            root_tuple->key[0] = 0;
            root_tuple->key_len = 0;
            if( root_tuple->data != NULL){
                free(root_tuple->data);
            }
            root_tuple->data = NULL;
            root_tuple->data_len = 0;
        }else{
            HDB_HASH_REMOVE(root_tuple, ptr_tuple);
            if( ptr_tuple->data != NULL){
                free(ptr_tuple->data);
            }
            free(ptr_tuple);
            (*hmap_db)->secondary_tuple_count--;
        }
        (*hmap_db)->tuple_count--;
    }
        
    return r;
}

int hmap_set(HMAP_DB *hmap_db, void *key, int k_len, void *data, int d_len, int alway_set);

int hmap_truncate(HMAP_DB **hmap_db){
    TUPLE *ptr_list_tuple = (*hmap_db)->list_tuple;
    while( ptr_list_tuple ){
        HDB_LIST_REMOVE((*hmap_db)->list_tuple, ptr_list_tuple);
        if( ptr_list_tuple->data ){
            free(ptr_list_tuple->data);
            ptr_list_tuple->data = NULL;
            ptr_list_tuple->key[0] = 0;
            ptr_list_tuple->key_len = 0;
        }
        if( ptr_list_tuple->type == HMAP_TUPLE_SECONDARY ){
            free(ptr_list_tuple);
        }
        ptr_list_tuple = (*hmap_db)->list_tuple;
    }
    return 0;
}

int hmap_destroy(HMAP_DB **hmap_db){
    hmap_truncate(hmap_db);
    free((*hmap_db)->tuple);
    free((*hmap_db));
    *hmap_db = NULL;
    return 0;
}

static void hmap_print( HMAP_DB *my_hmap_db ){
    int i = 0;
    TUPLE * my_tuple = NULL;
    my_tuple = my_hmap_db->tuple;
    for(; i < my_hmap_db->bucket_size; i++){
        printf("index[%d][%p] key[%s], data[%s]\n", i,my_tuple , my_tuple->key, (char *)my_tuple->data);
        my_tuple++;
    }
}

static void hmap_print_list_tuple( HMAP_DB *my_hmap_db ){
    TUPLE *ptr_list_tuple = my_hmap_db->list_tuple;
    while( ptr_list_tuple ){
        
        printf("index[%d][%p] key[%s], data[%s]\n", ptr_list_tuple->index, ptr_list_tuple,  ptr_list_tuple->key, (char *)ptr_list_tuple->data);

        if( ptr_list_tuple->next == my_hmap_db->list_tuple){
            break;
        }
        ptr_list_tuple = ptr_list_tuple->next;
    }
}

static void hmap_print_tree_tuple( HMAP_DB *my_hmap_db ){
    int i = 0;
    TUPLE *root_tuple, *ptr_tuple;
    for( ; i < my_hmap_db->bucket_size ; i++){
        ptr_tuple = root_tuple = (my_hmap_db->tuple+i);
        if( root_tuple == NULL  ){
            continue;
        }
        printf("index[%d]\n", ptr_tuple->index);
        while( ptr_tuple ){
            if ( ptr_tuple->key_len > 0){
                printf("\t[%p] key[%s], data[%s]\n", ptr_tuple,  ptr_tuple->key, (char *)ptr_tuple->data);
            }
            if( ptr_tuple->hash_next == root_tuple ){
                break;
            }
            ptr_tuple = ptr_tuple->hash_next;
        }
    }
}

int hmap_print_table( HMAP_DB *my_hmap_db ){
    if( my_hmap_db == NULL ) 
        return HMAP_DB_EMPTY;
    hmap_print(my_hmap_db);
    return HMAP_SUCCESS;
}

int hmap_print_list( HMAP_DB *my_hmap_db ){
    if( my_hmap_db == NULL ) 
        return HMAP_DB_EMPTY;
    hmap_print_list_tuple(my_hmap_db);
    return HMAP_SUCCESS;
}

int hmap_print_tree( HMAP_DB *my_hmap_db ){
    if( my_hmap_db == NULL ) 
        return HMAP_DB_EMPTY;
    hmap_print_tree_tuple(my_hmap_db);
    return HMAP_SUCCESS;
}
