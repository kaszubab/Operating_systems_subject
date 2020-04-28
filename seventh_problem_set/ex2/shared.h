#ifndef shared
#define shared


#define MAXIMUM_ORDERS 15
#define ACCESS_FLAGS 0666

#define SHARED_MEMORY "/shared"
#define OPERATION_LOCK "/locker"
#define SIZE_CONTROL "/size_lock"
#define WAITING_TO_BE_PROCESSED "/process_lock"
#define WAITING_FOR_SENDING "/send_lock"

#define TRUE 1
#define FALSE 0

typedef struct company_orders
{
    int orders [MAXIMUM_ORDERS];
    int new_order_index;
    int prepare_order_index;
    int send_order_index;


} company_orders;

#endif

