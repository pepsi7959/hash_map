#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "hmap.h"
//#include "watch.h"


int main(){
    char *key1[] = {
                    "version",
                    "priority",
                    "ds3politedegree",
                    "expirygraceperiod",
                    "preexpirythr",
                    "postexpirythr",
                    "lifecycleunit",
                    "lifecycleno",
                    "recurringlifecycle",
                    "ds3timeresolution",
                    "counterrollover",
                    "refillstopext",
                    "ds3packagedesc",
                    "ds3offlinenotifpolicycontrol",
                    "ds3recurringpolicycontrol",
                    "ds3costcode",
                    "persistcycle",
                    "offerstate",
                    "sponsorid",
                    "privateidlist",
                    "publicidlist",
                    "implicitregsetid",
                    "regprivateidlist",
                    "registrationstatus",
                    "aliaid",
                    "pfpassword",
                    "ds3pin",
                    "secretquestion",
                    "secretanswer",
                    "accountid",
                    "productnolist",
                    "productownerlist",
                    "mmtelprofileid",
                    "addifexist",
                    "old_number",
                    "new_number",
                    "ds3_polite_degree",
                    "ds3_brandid",
                    "ds3_spname",
                    "ds3_registration_status",
                    "language",
                    "ivr_language",
                    "sms_language",
                    "ussd_language",
                    "email_language",
                    "preferences",
                    "ds3_customer_category",
                    "ds3_customer_subcategory",
                    "ds3_customer_segment",
                    "address",
                    "birthday",
                    "priority1",
                    "version1",
                    "sendsms",
                    "locked",
                    "activation_date",
                    "counter_state",
                    "expiry_time",
                    "expiry_grace_period",
                    "preexpiry_thr",
                    "postexpiry_thr",
                    "expiry_notify_time",
                    "persist_cycle",
                    "lifecycle_unit",
                    "lifecycle_no",
                    "recurring_lifecycle",
                    "ds3_time_resolution",
                    "counter_rollover",
                    "refill_stop_time",
                    "refill_stop_ext",
                    "refill_notify_time",
                    "amf_cost_code",
                    "amf_dynamic_biz_info",
                    "amf_order_date",
                    "amf_recurring_state",
                    "amf_package_refkey",
                    "ds3_package_desc",
                    "ds3_offline_notify_policy_control",
                    "ds3_recurring_notify_policy_control",
                    "ds3_cost_code",
                    "sponsor_id",
                    "offer_state",
                    "last_index_name",
                    "ok1"
                };
    HMAP_DB *my_hmap_db = NULL;   
    //struct timeval elap_tm;
    int i = 0;
    int data_len = 0; 
    char data[1024];
    //char error[1024];
    int ret = 0;
    
    //init_watch(TMR0, error);
    hmap_init(1024, &my_hmap_db);
    //stop_watch(TMR0, &elap_tm, error);
    //printf("\x1b[41m<<<<< init_watch elap time: %lu.%.6lu >>>>>\x1b[0m\n", elap_tm.tv_sec, elap_tm.tv_usec);
     
    //init_watch(TMR0, error);
    for(; i < 84 ; i++){
        data_len = sprintf(data, "my data at %d", i);
        if( (ret = hmap_add(&my_hmap_db, key1[i], strlen(key1[i]), data, data_len, 1)) < 0 ){
            printf("Add error[%d] key[%s]\n", ret, key1[i]);
            return -1; 
        }
    }
    //stop_watch(TMR0, &elap_tm, error);
    //printf("\x1b[41m<<<<< add data elap time: %lu.%.6lu >>>>>\x1b[0m\n", elap_tm.tv_sec, elap_tm.tv_usec);
     
    hmap_print_list(my_hmap_db);
    if( (ret = hmap_delete(&my_hmap_db, key1[75], strlen(key1[75]))) != HMAP_SUCCESS){
        printf("Delete error[%d]\n", ret);
        return -1;
    }
    printf("AFTER DELETE:\n");
    hmap_print_list(my_hmap_db);
    
    getchar();
    data_len = sprintf(data, "my data at %d\n", 75);
    hmap_add(&my_hmap_db, key1[75], strlen(key1[75]), data, data_len, 1);
    printf("ADD AGAIN:\n");
    hmap_print_list(my_hmap_db);
    
    getchar();
    hmap_print_tree(my_hmap_db);
    
    printf( "before database tuple: %d\n", my_hmap_db->tuple_count);
    if( (ret = hmap_delete(&my_hmap_db, key1[4], strlen(key1[4]))) != HMAP_SUCCESS){
        printf("Delete error[%d]\n", ret);
        return -1;
    }
    
    printf( "after database tuple: %d\n", my_hmap_db->tuple_count);
    
    getchar();
    hmap_print_tree(my_hmap_db);

    
    struct my_data{
        int a;
        };
    struct my_data *mdata = (struct my_data*) calloc(1, sizeof(struct my_data));
    mdata->a = 11;
    ret = hmap_add_tuple_with_data(&my_hmap_db, key1[4], strlen(key1[4]), (void*)mdata, 1);
    TUPLE *ptr_tuple = NULL;
    ret = hmap_search(my_hmap_db, key1[4], strlen(key1[4]), &ptr_tuple);
    if( ret == HMAP_SUCCESS )
    printf("Try to search %s : %d\n", key1[4], ((struct my_data*)ptr_tuple->data)->a);
    
    hmap_destroy(&my_hmap_db);
    
    return 0;
}

