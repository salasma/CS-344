#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// Size of the buffer
#define SIZE 10
#define MAX 1000000

// Special marker used to indicate end of the producer data
#define END_MARKER -1

// fgets get input
char input[MAX];
// Buffer, shared resource
int buffer[1000];
int buffer_2[1000];
int buffer_3[1000];

// thread store the characters
char store_char[MAX];
char store_char_2[MAX];
char store_char_3[MAX];
char store_char_4[MAX];
char temp_char[MAX];
char temp_char_2[MAX];

// store the plus index
int plus[MAX];
int plus_index = 0;
int index_1 = 0;
int index_2 = 0;
int index_3 = 0;
int index_4 = 0;
int char_in_line = 0;

// Number of items in the buffer, shared resource
int count = 0;
int count_2 = 0;
int count_3 = 0;
// Index where the producer will put the next item
int prod_idx = 0;
int prod_idx_2 = 0;
int prod_idx_3 = 0;
// Index where the consumer will pick up the next item
int con_idx = 0;
int con_idx_2 = 0;
int con_idx_3 = 0;

// Initialize the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variables
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty_2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty_3 = PTHREAD_COND_INITIALIZER;

/*
get the input from user
*/
void get_characters(){
      int i = 0;
      int j = 0;

      // while loop to get input
      while(1){
            fgets(input, MAX, stdin);
            strcat(store_char, input);
            // check terminal
            if(store_char[strlen(store_char) - 1] == '\n' && store_char[strlen(store_char) - 2] == 'E' &&
               store_char[strlen(store_char) - 3] == 'N' && store_char[strlen(store_char) - 4] == 'O' &&
               store_char[strlen(store_char) - 5] == 'D' && store_char[strlen(store_char) - 6] == '\n'){
                  break;
            }
      }
      // set the terminal sign
      store_char[strlen(store_char) - 5] = '\0';
      return;
}


/*
 Produce an item. Every line separator in the input will be replaced by a space
*/
void produce(int i){
      // space replace line separator
      if(store_char[i] == '\n'){
            buffer[prod_idx] = ' ';
      }
      else{
            buffer[prod_idx] = store_char[i];
      }
      // Increment the index where the next item will be put. Roll over to the start of the buffer
      // if the item was placed in the last slot in the buffer
      prod_idx = (prod_idx + 1) ;
      count++;
      return;
}


/*
 Function that the producer thread will run. Produce an item and put in the buffer only if there
 is space in the buffer. If the buffer is full, then wait until there is space in the buffer.
*/
void *producer(void *args){
      int i;
      for (i = 0; i < strlen(store_char); i++){
            // Lock the mutex before checking where there is space in the buffer
            pthread_mutex_lock(&mutex);
            while (count == 80)
                  // Buffer is full. Wait for the consumer to signal that the buffer has space
                  pthread_cond_wait(&empty, &mutex);
            produce(i);
            // Signal to the consumer that the buffer is no longer empty
            pthread_cond_signal(&full);
            // Unlock the mutex
            pthread_mutex_unlock(&mutex);
      }
      return;
}


/*
 Get the next item from the buffer
*/
void consume(int i){
      store_char_2[i] = buffer[con_idx];
      // Increment the index from which the item will be picked up, rolling over to the start of the buffer if currently at the end of the buffer
      con_idx = (con_idx + 1);
      count--;
      return;
}


/*
Every adjacent pair of plus signs, i.e., "++", is replaced by a "^".
*/
void delete_plus(){
      int i = 0;
      int j = 0;

      while(1){
            // break loop
            if(i == strlen(store_char_2)){
                  break;
            }
            // if there a adjacent pair of plus signs, delete one plus
            if(store_char_2[i] == '+' && store_char_2[i + 1] == '+'){
                  temp_char[j] = store_char_2[i];
                  // maker the delete plus index and will replaced by a "^"
                  plus[plus_index] = j;
                  plus_index += 1;
                  j += 1;
                  i += 2;
            }
            else{
                  temp_char[j] = store_char_2[i];
                  j += 1;
                  i += 1;
            }
      }
      memset(store_char_2, '\0', sizeof(store_char_2));
      strcpy(store_char_2, temp_char);
}


/*
The “80 character line” to be written to standard output is defined as 80 non-line separator
characters plus a line separator.
*/
void line_separa(){
      int i;
      for(i = 0; i < strlen(store_char_2); i ++){
            // if full 80 characters, plus one line separator
            if(char_in_line == 80){
                  char_in_line = 0;
                  // one line separator
                  temp_char_2[index_2] = '\n';
                  temp_char_2[index_2 + 1] = store_char_2[i];
                  index_2 += 2;
                  char_in_line += 1;
            }
            // nornal copy characters
            else{
                  temp_char_2[index_2] = store_char_2[i];
                  index_2 += 1;
                  char_in_line += 1;
            }
      }
      // get the new characters
      memset(store_char_2, '\0', sizeof(store_char_2));
      strcpy(store_char_2, temp_char_2);
      return;
}


/*
 Function that the consumer thread will run. Get  an item from the buffer if the buffer is not empty. If the buffer is empty then wait until there is data in the buffer.
*/
void *consumer(void *args){
      // Continue consuming until the END_MARKER is seen
      while (index_1 != strlen(store_char)){
            // Lock the mutex before checking if the buffer has data
            pthread_mutex_lock(&mutex);
            while (count == 0)
                  // Buffer is empty. Wait for the producer to signal that the buffer has data
                  pthread_cond_wait(&full, &mutex);
            consume(index_1);
            // Signal to the producer that the buffer has space
            pthread_cond_signal(&empty);
            // Unlock the mutex
            pthread_mutex_unlock(&mutex);
            index_1 += 1;
      }

      delete_plus();
      line_separa();
      return;
}


/*
 Produce an item. From the last Producer-consumer
*/
void produce_2(int i){
      buffer_2[prod_idx_2] = store_char_2[i];

      // Increment the index where the next item will be put. Roll over to the start of the buffer if the item was placed in the last slot in the buffer
      prod_idx_2 = (prod_idx_2 + 1) ;
      count_2++;
      return;
}


/*
 Function that the producer thread will run. Produce an item and put in the buffer only if there is space in the buffer. If the buffer is full, then wait until there is space in the buffer.
*/
void *producer_2(void *args){
      int i;
      for (i = 0; i < strlen(store_char_2); i++){
            // Lock the mutex before checking where there is space in the buffer
            pthread_mutex_lock(&mutex_2);
            while (count_2 == 80)
                  // Buffer is full. Wait for the consumer to signal that the buffer has space
                  pthread_cond_wait(&empty_2, &mutex_2);
            produce_2(i);
            // Signal to the consumer that the buffer is no longer empty
            pthread_cond_signal(&full_2);
            // Unlock the mutex
            pthread_mutex_unlock(&mutex_2);
      }
      return;
}


/*
 Get the next item from the buffer
*/
void consume_2(int i){
      store_char_3[i] = buffer_2[con_idx_2];

      // Increment the index from which the item will be picked up, rolling over to the start of the buffer if currently at the end of the buffer
      con_idx_2 = (con_idx_2 + 1);
      count_2--;
      return;
}


/*
Every adjacent pair of plus signs, i.e., "++", is replaced by a "^".
*/
void change_plus(){
      int i;
      for(i = 0; i < plus_index; i++){
            store_char_3[plus[i]] = '^';
      }
      return;
}


/*
 Function that the consumer thread will run. Get  an item from the buffer if the buffer is not empty. If the buffer is empty then wait until there is data in the buffer.
*/
void *consumer_2(void *args){
      // Continue consuming until the END_MARKER is seen
      while (index_3 != strlen(store_char_2)){
            // Lock the mutex before checking if the buffer has data
            pthread_mutex_lock(&mutex_2);
            while (count_2 == 0)
                  // Buffer is empty. Wait for the producer to signal that the buffer has data
                  pthread_cond_wait(&full_2, &mutex_2);
            consume_2(index_3);
            // Signal to the producer that the buffer has space
            pthread_cond_signal(&empty_2);
            // Unlock the mutex
            pthread_mutex_unlock(&mutex_2);
            index_3 += 1;
      }
      change_plus();
      return;
}


/*
 Produce an item. From the last Producer-consumer
*/
void produce_3(int i){
      buffer_3[prod_idx_3] = store_char_3[i];

      // Increment the index where the next item will be put. Roll over to the start of the buffer if the item was placed in the last slot in the buffer
      prod_idx_3 = (prod_idx_3 + 1) ;
      count_3++;
      return;
}


/*
 Function that the producer thread will run. Produce an item and put in the buffer only if there is space in the buffer. If the buffer is full, then wait until there is space in the buffer.
*/
void *producer_3(void *args){
      int i;
      for (i = 0; i < strlen(store_char_3); i++){
            // Lock the mutex before checking where there is space in the buffer
            pthread_mutex_lock(&mutex_3);
            while (count_3 == 80)
                  // Buffer is full. Wait for the consumer to signal that the buffer has space
                  pthread_cond_wait(&empty_3, &mutex_3);
            produce_3(i);
            // Signal to the consumer that the buffer is no longer empty
            pthread_cond_signal(&full_3);
            // Unlock the mutex
            pthread_mutex_unlock(&mutex_3);
      }
      return;
}


/*
 Get the next item from the buffer
*/
int consume_3(int i){
      store_char_4[i] = buffer_3[con_idx_3];

      // Increment the index from which the item will be picked up, rolling over to the start of the buffer if currently at the end of the buffer
      con_idx_3 = (con_idx_3 + 1);
      count_3--;
}

/*
delete the line shorter than 80
*/
void print_char(){
      char* token = strtok(store_char_4, "\n");

      // Keep printing tokens while one of the
      // delimiters present in str[].
      while (token != NULL) {
            // check if the line euqal to 80
            if(strlen(token) == 80){
                  printf("%s\n", token);
                  token = strtok(NULL, "\n");
            }
            else{
                  return;
            }
      }
      return;
}


/*
 Function that the consumer thread will run. Get  an item from the buffer if the buffer is not empty. If the buffer is empty then wait until there is data in the buffer.
*/
void *consumer_3(void *args){
      // Continue consuming until the END_MARKER is seen
      while (index_4 != strlen(store_char_3)){
            // Lock the mutex before checking if the buffer has data
            pthread_mutex_lock(&mutex_3);
            while (count_3 == 0)
                  // Buffer is empty. Wait for the producer to signal that the buffer has data
                  pthread_cond_wait(&full_3, &mutex_3);
            consume_3(index_4);
            // Signal to the producer that the buffer has space
            pthread_cond_signal(&empty_3);
            // Unlock the mutex
            pthread_mutex_unlock(&mutex_3);
            index_4 += 1;
      }
      print_char();
      //printf("%s\n", store_char_4);
      return;
}


int main(){
      get_characters();

      // Create the producer and consumer threads
      pthread_t p, c;
      pthread_create(&p, NULL, producer, NULL);
      // Sleep for a few seconds to allow the producer to fill up the buffer. This has been put in to demonstrate the the producer blocks when the buffer is full. Real-world systems won't have this sleep
      sleep(5);
      pthread_create(&c, NULL, consumer, NULL);

      pthread_join(p, NULL);
      pthread_join(c, NULL);

      pthread_t p2, c2;
      pthread_create(&p2, NULL, producer_2, NULL);
      // Sleep for a few seconds to allow the producer to fill up the buffer. This has been put in to demonstrate the the producer blocks when the buffer is full. Real-world systems won't have this sleep
      sleep(5);
      pthread_create(&c2, NULL, consumer_2, NULL);

      pthread_join(p2, NULL);
      pthread_join(c2, NULL);

      pthread_t p3, c3;
      pthread_create(&p3, NULL, producer_3, NULL);
      // Sleep for a few seconds to allow the producer to fill up the buffer. This has been put in to demonstrate the the producer blocks when the buffer is full. Real-world systems won't have this sleep
      sleep(5);
      pthread_create(&c3, NULL, consumer_3, NULL);

      pthread_join(p3, NULL);
      pthread_join(c3, NULL);

      return 0;
}
