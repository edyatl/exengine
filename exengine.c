/* exengine -- exchange engine for one good

   Written by Yevgeny Dyatlov (@edyatl)

   <https://github.com/edyatl/>             */


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/* Constant for order stack capacity */

#define CAPACITY 1024


/* Imitation Python try except */

#define try bool __KeyError = false;
#define except(x) ExitJmp: if(x)
#define raise(x) x = true; goto ExitJmp;


/* Counter for trade deals */

static int tid_counter = 0;


/* Interface for order - 12 bytes */

struct order {
    int oid;
    int qty;
    float price;
};


/* Interface for order stack */

struct order_stack {
    int top;
    struct order orders[CAPACITY];
};


/* Constructor of order stack */

static struct order_stack order_stack_init() {
    struct order_stack stack; 
    stack.top = -1;
    return stack;
}


/* Add new order to stack */

void add_order(struct order_stack *orderstk, int oid, int qty, float price) {

    orderstk->orders[++orderstk->top].oid = oid;
    orderstk->orders[orderstk->top].qty = qty;
    orderstk->orders[orderstk->top].price = price;
}


/* Find order_stack index by oid */

int find_i(struct order_stack *orderstk, int oid) {
    int i = 0;

    (oid < orderstk->top) ? (i = oid - 1) : (i = orderstk->top); 
    while((i != -1) && (orderstk->orders[i].oid != oid)) i--;
    return i;
}


/* Remove order from stack */

bool rm_order(struct order_stack *orderstk, int oid) {
    int i = 0;
    bool __KeyError = false;

    i = find_i(orderstk, oid);
    i > -1 ? i : (__KeyError = true);

    if((i > -1) && (i == orderstk->top)) {
        orderstk->top--;

    } else if((i > -1) && (i < orderstk->top)) {
        for(i; i != orderstk->top; ++i) {

            orderstk->orders[i].oid = orderstk->orders[i+1].oid;
            orderstk->orders[i].qty = orderstk->orders[i+1].qty;
            orderstk->orders[i].price = orderstk->orders[i+1].price;
        } 
        orderstk->top--;
    }
    return __KeyError;
}


/* Cancel order - delete it from trading stack */

void cancel_order(int oid,
                  struct order_stack *buy_orders, 
                  struct order_stack *sel_orders) {

    try {
/* 
        Check removing order id in buy_orders stack and remove it if exists.
        In other case raise exception and go to check sel_orders.
*/ 
        if(rm_order(buy_orders, oid)) {
            raise(__KeyError);
        }
        printf("X,%d\n", oid);
    } 
    except(__KeyError) {
        try {
            if(!rm_order(sel_orders, oid))
                printf("X,%d\n", oid);
        }
    }
}


/* Split input string by comma and get items */

char ** get_cmd_item(char *line, char **items) {
    int i = 0;
    char * item = strtok(line, ",");

    while(item != NULL) {
        items[i++] = item;
        item = strtok(NULL, ",");
    }
    return items;
}


/* Find max element */

struct order find_max(struct order *ord, int size) {
    struct order mxo = ord[0];
    int i = 0;

    for(i = 1; i != size; ++i)
        (mxo.price < ord[i].price ? mxo = ord[i] : mxo);  /* Find only by price, by oid not needed
                                                           * because loop from begin to end elem.
                                                           */
    return mxo;
}


/* Find min element */

struct order find_min(struct order *ord, int size) {
    struct order mno = ord[0];
    int i = 0;

    for(i = 1; i != size; ++i)
        (mno.price > ord[i].price ? mno = ord[i] : mno);  /* Find only by price, by oid not needed 
                                                           * because loop from begin to end elem.
                                                           */
    return mno;
}


/* Crutch to print float in Python style __.0 */

char* pyprint_float(float fp, char* fs) {
    const char *pz = ".0";

    sprintf(fs,"%g", fp);
    return (strchr(fs, '.') ? fs : strcat(fs, pz));
}


/* Makes trade deals between buy and sell orders in recursion */

void mktrade(struct order_stack *buy_orders, 
             struct order_stack *sel_orders) {

    struct order max_price_buy_order;
    struct order min_price_sel_order;
/*
    If buy_orders stack and sel_orders exists then define max and min orders by price
*/
    if((buy_orders->top > -1) && (sel_orders->top > -1)) {
        max_price_buy_order = find_max(buy_orders->orders, buy_orders->top+1);
        min_price_sel_order = find_min(sel_orders->orders, sel_orders->top+1);
    }
/* 
    If max and min orders by price defined then check is their prices suitable for trade deal
*/
    if((max_price_buy_order.oid > 0) && (min_price_sel_order.oid > 0)) {
        if(min_price_sel_order.price <= max_price_buy_order.price) {
            char side;
            int oid1 = -1;
            int oid2 = -1;
            float tprice = 0;

            struct order *cleared_order = NULL;  /* pointer to closing order */
            struct order *keep_order = NULL;     /* pointer to keep on stack order */
/* 
            Define cleared_order and keep_order by quantity field.
            If max price oredr quantity < min price oreder quantity then 
            they are respectively cleared and keep.
            And if they >= vice versa.
*/
            if(max_price_buy_order.qty < min_price_sel_order.qty) {
                cleared_order = &max_price_buy_order;
                keep_order = &min_price_sel_order;

            } else {
               cleared_order = &min_price_sel_order;
               keep_order = &max_price_buy_order;
            } 
/*
            Define min id of tow orders in deal and vars for trade record
*/
            if(min_price_sel_order.oid < max_price_buy_order.oid) {
                side = 'S';
                oid1 = min_price_sel_order.oid;
                oid2 = max_price_buy_order.oid;
                tprice = min_price_sel_order.price;

            } else {
                side = 'B';
                oid1 = max_price_buy_order.oid;
                oid2 = min_price_sel_order.oid;
                tprice = max_price_buy_order.price;
            }

            int new_qty = abs(keep_order->qty - cleared_order->qty);
            char stprice[32]; /* buffer for tprice float to py style print */
/* 
            Make trade deal and record. 
*/ 
            try {
/* 
                Check cleared_order in buy_orders stack and remove it if exists.
                In other case raise exception and goto check sel_orders.
*/ 
                if(rm_order(buy_orders, cleared_order->oid)) {
                    raise(__KeyError);
                }
/*
                Check if quantity trade orders == then remove keep_order if so,
                or record new quantity to keep_order.
*/
                new_qty == 0 ? rm_order(sel_orders, keep_order->oid) : 
                (sel_orders->orders[find_i(sel_orders, keep_order->oid)].qty = new_qty);

                printf("T,%d,%c,%d,%d,%d,%s\n", ++tid_counter, side, oid1, oid2, cleared_order->qty, (pyprint_float(tprice, stprice)));
            } 
            except(__KeyError) {
                try {
/*
                    Make as above try block for sel_orders stack
*/
                    if(!rm_order(sel_orders, cleared_order->oid))
                        new_qty == 0 ? rm_order(buy_orders, keep_order->oid) : 
                        (buy_orders->orders[find_i(buy_orders, keep_order->oid)].qty = new_qty);

                    printf("T,%d,%c,%d,%d,%d,%s\n", ++tid_counter, side, oid1, oid2, cleared_order->qty, (pyprint_float(tprice, stprice)));
                }
            }
/*
            Go recurcion
*/
            mktrade(buy_orders, sel_orders);
        }
    }
}


int main(void) {
    char *line = NULL;  /* Input string */
    size_t len = 0;
    ssize_t nread = 0;

    char *items[5];  /* Tokens in input */
/*
    Make two new stacks for buy_orders and sel_orders
*/
    struct order_stack buy_orders = order_stack_init();
    struct order_stack sel_orders = order_stack_init();
/*
    Read input and split tokens to an array
*/
    while ((nread = getline(&line, &len, stdin)) != -1) {
        line[nread - 1] = '\0';
        char **cmd = get_cmd_item(line, items);
/*
        Check command for Cancel
*/
        if(*cmd[0] == 'C') {

            cancel_order(atoi(cmd[1]), &buy_orders, &sel_orders);
/*
        Check command for new Order
*/
        } else if(*cmd[0] == 'O') {
/*
            Check order if Buy then add it on buy_orders stack and try trade
*/
            if(*cmd[2] == 'B') {
            
                add_order(&buy_orders, atoi(cmd[1]), atoi(cmd[3]), atof(cmd[4]));
                mktrade(&buy_orders, &sel_orders);

/*
            Check order if Sell then add it on sel_orders stack and try trade
*/
            } else if(*cmd[2] == 'S') {

                add_order(&sel_orders, atoi(cmd[1]), atoi(cmd[3]), atof(cmd[4]));
                mktrade(&buy_orders, &sel_orders);

            } else {
            printf("ERROR Bad operation\n");
            }


        } else {
        printf("ERROR Bad command\n");
        }
    }
    return 0;
}
