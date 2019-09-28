#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "lib.h"

long long TAKE(node_parameter* param) {
   pthread_mutex_lock(param->read_mutex);

   node* n = (node*) *param->read_head;

   while((*param->stop != 1) && (n->next == 0)) { } //spin wait

   if(*param->stop == 1) {
      pthread_mutex_unlock(param->read_mutex);

      pthread_exit(0);
   }
   else {
      long long result = n->next->data;
      node* next = n->next;

      free(*param->read_head);

      *param->read_head = next;

      pthread_mutex_unlock(param->read_mutex);

      return result;
   }
}

void write_to_one(write_head* param, long long data) {
   pthread_mutex_lock(param->write_mutex);

   node* new_node = (node*) malloc(sizeof(node));
   node* previous = (node*) *param->write_head;

   new_node->next = 0;
   new_node->data = data;

   previous->next = new_node;

   *param->write_head = new_node;

   pthread_mutex_unlock(param->write_mutex);  
}

void PUSH(node_parameter* param, long long data) {
//   write_to_one(param->write_heads, data);
   for(int i = 0; i < param->write_head_count; ++i) {
      write_to_one(param->write_heads + i, data);
   }

}

