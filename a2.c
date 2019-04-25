/***********************************************************************************/
//***********************************************************************************
//            *************NOTE**************
// This is a template for the subject of RTOS in University of Technology Sydney(UTS)
// Please complete the code based on the assignment requirement.

//***********************************************************************************
/***********************************************************************************/

/*
  To compile prog_1 ensure that gcc is installed and run the following command:
  gcc prog_1.c -o prog_1 -lpthread -lrt -Wall

*/
#include  <pthread.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <stdio.h>
#include  <sys/types.h>
#include  <fcntl.h>
#include  <string.h>
#include  <sys/stat.h>
#include  <semaphore.h>
#include  <sys/time.h>

/* --- Structs --- */

typedef struct ThreadParams {
  int pipeFile[2];
  sem_t sem_read, sem_justify, sem_write;
  char message[255];
  pthread_mutex_t lock;
} ThreadParams;

/* --- Prototypes --- */

/* Initializes data and utilities used in thread params */
void initializeData(ThreadParams *params);

/* This thread reads data from data.txt and writes each line to a pipe */
void* ThreadA(void *params);

/* This thread reads data from pipe used in ThreadA and writes it to a shared variable */
void* ThreadB(void *params);

/* This thread reads from shared variable and outputs non-header text to src.txt */
void* ThreadC(void *params);

/* --- Main Code --- */
int main(int argc, char const *argv[]) 
{
  printf("START\n");
  //struct timeval t1, t2;
  //gettimeofday(&t1, NULL);  // Start Timer
  int err;
  pthread_t tid[3];
  pthread_attr_t attr;
  ThreadParams params;

  // Initialization
  initializeData(&params);
  pthread_attr_init(&attr);

  //Starting Semaphore
  sem_post(&params.sem_write);

  // Create Threads
  if((err = pthread_create(&(tid[0]), &attr, &ThreadA, (void*)(&params))))
  {
	  perror("Error creating threads: ");
      exit(-1);
  }
  //TODO: add your code
  
  if((err = pthread_create(&(tid[1]), &attr, &ThreadB, (void*)(&params))))
  {
    perror("Error creating threads: ");
      exit(-1);
  }

  if((err = pthread_create(&(tid[2]), &attr, &ThreadC, (void*)(&params))))
  {
    perror("Error creating threads: ");
      exit(-1);
  }



  // Wait on threads to finish
  pthread_join(tid[0], NULL); 
  pthread_join(tid[1], NULL);
  pthread_join(tid[2], NULL);
  //TODO: add your code
  printf("YA DUN M8!!\n");

  return 0;
}

void initializeData(ThreadParams *params) {
  // Initialize Sempahores
  sem_init(&(params->sem_write), 0, 0);
  sem_init(&(params->sem_read), 0, 0);
  sem_init(&(params->sem_justify), 0, 0);
  //TODO: add your code
  printf("data init \n");

  return;
}

void* ThreadA(void *params) {
  //TODO: add your code
  ThreadParams* p = (ThreadParams*)params;
  while(1)
  {
    sem_wait(&p->sem_write);
    printf("Thread A running\n");
    //sleep(1);
    sem_post(&p->sem_read);
  }
  
  return 0;
}

void* ThreadB(void *params) { 
  //TODO: add your code
  ThreadParams* p = (ThreadParams*)params;
  while(1)
  {
    sem_wait(&p->sem_read);
    printf("Thread B running\n");
    //sleep(1);
    sem_post(&p->sem_justify); 
  }
  return 0;
}

void* ThreadC(void *params) {
  //TODO: add your code
  ThreadParams* p = (ThreadParams*)params;
  while(1)
  {
    sem_wait(&p->sem_justify);
    printf("Thread C running\n\n");
    sleep(1);
    sem_post(&p->sem_write);
  }
  return 0;
}
