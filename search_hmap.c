#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "hmap.h"


int revese_search( char *find , HMAP_DB *db, char *value){
    char *p = NULL;
    int ret = 0;
    TUPLE *ptr_tuple = NULL;
    
    p = find;

    while( p != NULL ){
        printf("token : %s\n", p);
        if( hmap_search(db, p, strlen(p), &ptr_tuple) == HMAP_SUCCESS){
            memcpy(value, ptr_tuple->vals.val_chars, ptr_tuple->data_len);
            value[ptr_tuple->data_len] = 0;
            return 0;
        }
        p = strstr(p, ",");
        p++;
    }
    value[0] = NULL;
    return -1;
}


int main(){
    HMAP_DB *my_hmap_db = NULL;   
    char data[1024];

    /* initialize database */
    hmap_init(1024, &my_hmap_db);
    
    hmap_add(&my_hmap_db, "msisdn=#1,dc=MSISDN,dc=C-NTDB", strlen("msisdn=#1,dc=MSISDN,dc=C-NTDB"), 1, HMAP_DATA_TYPE_CHARS, "subdata=services,msisdn=#1,dc=MSISDN,dc=C-NTDB", strlen("subdata=services,msisdn=#1,dc=MSISDN,dc=C-NTDB"));
    hmap_add(&my_hmap_db, "imsi=#1,dc=IMSI,dc=C-NTDB", strlen("imsi=#1,dc=IMSI,dc=C-NTDB"), 1, HMAP_DATA_TYPE_CHARS, "ds=ds3,imsi=#1,dc=IMSI,dc=C-NTDB", strlen("ds=ds3,imsi=#1,dc=IMSI,dc=C-NTDB"));

    /* display database */
    hmap_print_list(my_hmap_db);


    data[0] = 0;
    revese_search("ds=ds3,user=toro,msisdn=#1,dc=MSISDN,dc=C-NTDB", my_hmap_db, data);
    printf("data : %s\n", data);

    data[0] = 0;
    revese_search("user=toro,imsi=#1,dc=IMSI,dc=C-NTDB", my_hmap_db, data);
    printf("data : %s\n", data);
    /* destroy database */
    hmap_destroy(&my_hmap_db);
    
    return 0;
}

