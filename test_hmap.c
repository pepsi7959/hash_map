#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "hmap.h"

int main(){
    HMAP_DB *my_hmap_db = NULL;   
    TUPLE *ptr_tuple    = NULL;
    int data_len        = 0; 
    int ret             = 0;
    char data[1024];

    struct my_data{
    int a;
    };

    char *key[2]        = {"key:value1","key:value2"};

    /* initialize database */
    hmap_init(1024, &my_hmap_db);

    /* add key="key:value2" value="kv2" */
    data_len = sprintf(data, "kv2");
    ret = hmap_add(&my_hmap_db, key[1], strlen(key[1]), 1, HMAP_DATA_TYPE_CHARS, data, data_len);

    /* display database */
    hmap_print_list(my_hmap_db);

    /* add key="key:value2" value=@my_data */
    struct my_data *mdata = (struct my_data*) calloc(1, sizeof(struct my_data));
    mdata->a = 11;
    ret = hmap_add(&my_hmap_db, key[0], strlen(key[0]), 1, HMAP_DATA_TYPE_CUSTOM, (void*)mdata,0);

    /* search key="key:value1" expect=1  */
    ret = hmap_search(my_hmap_db, key[0], strlen(key[0]), &ptr_tuple);
    printf("Try to search %s : %d\n", key[0], ((struct my_data*)ptr_tuple->vals.val_custom)->a);

    
    hmap_add(&my_hmap_db, "age", strlen("age"), 1, HMAP_DATA_TYPE_UINT, 27, 0);
    hmap_add(&my_hmap_db, "FirstName", strlen("FirstName"), 1, HMAP_DATA_TYPE_CHARS, "Narongsak", strlen("Narongsak"));
    hmap_add(&my_hmap_db, "LastName", strlen("LastName"), 1, HMAP_DATA_TYPE_CHARS, "Mala", strlen("Mala"));
    hmap_add(&my_hmap_db, "Weight", strlen("Weight"), 1, HMAP_DATA_TYPE_DOUBLE, 62.239);

    /* display database */
    hmap_print_list(my_hmap_db);

    hmap_search(my_hmap_db, "FirstName", strlen("FirstName"), &ptr_tuple);
    hmap_add(&my_hmap_db, "LinkName", strlen("LinkName"), 1, HMAP_DATA_TYPE_CUSTOM, ptr_tuple);

    printf("First Name is   : %s\n", ptr_tuple->vals.val_chars);

    hmap_search(my_hmap_db, "LastName", strlen("LastName"), &ptr_tuple);
    printf("Last Name is    : %s\n", ptr_tuple->vals.val_chars);

    hmap_search(my_hmap_db, "age", strlen("age"), &ptr_tuple);
    printf("Ags             : %u years old\n", ptr_tuple->vals.val_uint);

    hmap_search(my_hmap_db, "Weight", strlen("Weight"), &ptr_tuple);
    printf("Weight          : %f kgs\n", ptr_tuple->vals.val_double);

    hmap_search(my_hmap_db, "LinkName", strlen("LinkName"), &ptr_tuple);
    printf("First Name is   : %s\n", ((TUPLE*)ptr_tuple->vals.val_custom)->vals.val_chars);

    /* destroy database */
    hmap_destroy(&my_hmap_db);
    
    return 0;
}

