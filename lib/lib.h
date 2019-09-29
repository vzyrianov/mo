#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

typedef struct node {
   long long data;
   struct node* next;
} node;

typedef struct write_head {
   void** write_head;
   pthread_mutex_t* write_mutex;
} write_head;

typedef struct node_parameter {
   void** read_head;
   pthread_mutex_t* read_mutex;

   int* stop;
   
   write_head* write_heads;
   int write_head_count; 
} node_parameter;

long long TAKE(node_parameter* param);

void write_to_one(write_head* param, long long data);

void PUSH(node_parameter* param, long long data);
