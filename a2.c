/***********************************************************************************/
//***********************************************************************************
//            *************NOTE**************
// This is a template for the subject of RTOS in University of Technology Sydney(UTS)
// Please complete the code based on the assignment requirement.

//***********************************************************************************
/***********************************************************************************/

/* --- Compile Info --- 

  To compile a2.c ensure that gcc is installed and run the following command:
  gcc a2.c -o a2 -pthread -lrt -Wall
*/

/* --- Code usage --- */

/*	Edit the Data file and Source file names below in the "Defines" section to
		align with the files to be read and written to, respectively.

		The marker for the end of the header section is defined as HEADER_END.
		Edit this to match the Data file specifications.

		The MAXCHAR define can be changed to reflect the maximum length 
		of each line in the data file 
*/

/* --- Included Libraries --- */
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

/* --- Defines --- */

#define MAXCHAR 100						// Maximum characters read from file
#define ENDSTRING '\0'
#define DATA_FILE "data.txt"
#define SRC_FILE "src.txt"
#define HEADER_END "end_header"

/* --- Global Variables --- */

// while loops condition
int z = 1;

// pipe pointers array
int fd[2];

// Pipe read data storage string
char PipeReadData[MAXCHAR+1];

// length of string in pipe
int len;

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

  // Thread creation variables
  int err;
  pthread_t tid[3];
  pthread_attr_t attr;
  ThreadParams params;

  // Initialization
  initializeData(&params);
  pthread_attr_init(&attr);

  // Signal first semaphore for Thread A to begin
  sem_post(&params.sem_write);


  /* -------- Thread Creation -------- */
  if((err = pthread_create(&(tid[0]), &attr, &ThreadA, (void*)(&params))))
  {
	  perror("Error creating Thread A");
      exit(-1);
  }
  // TODO: add your code
 	
  if((err = pthread_create(&(tid[1]), &attr, &ThreadB, (void*)(&params))))
  {
    perror("Error creating Thread B");
      exit(-1);
  }

  if((err = pthread_create(&(tid[2]), &attr, &ThreadC, (void*)(&params))))
  {
    perror("Error creating Thread C");
      exit(-1);
  }


  /* -------- Waiting for Thread Completion -------- */
  if (pthread_join(tid[0], NULL) != 0)
  {
  	perror("Thread A did not terminate correctly");
    exit(-1);
  } 
  
  if (pthread_join(tid[1], NULL) != 0)
  {
  	perror("Thread B did not terminate correctly");
    exit(-1);
  }
  
  if (pthread_join(tid[2], NULL) != 0)
  {
  	perror("Thread C did not terminate correctly");
    exit(-1);
  }
  
  // Indicate Successful Completion
  printf("YA DUN M8!!\n");

  return 0;
}

void initializeData(ThreadParams *params) 
{
  // Initialize Sempahores
  sem_init(&(params->sem_write), 0, 0);
  sem_init(&(params->sem_read), 0, 0);
  sem_init(&(params->sem_justify), 0, 0);
 
  // TODO: add your code

	// create a pipe: fd[0] is pointer for reading and fd[1] for writing
  if ( pipe(fd) < 0 ) 
    perror("pipe creation error");

  printf("Semaphore and pipe init successful \n");

  return;
}

void* ThreadA(void *params) 
{
  //TODO: add your code
  ThreadParams* p = (ThreadParams*)params;

	// Initialise file variables
	FILE *fp;
	char str[MAXCHAR+1];
	char* filename = DATA_FILE;


	// Open data file
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
	    printf("Could not open file %s",filename);
	    return 1;
	}

	printf("\n ------ File Read Begin ------ \n");

	// Read from file
	while (z)
	{
		// wait for pipe write signal from Thread C
    sem_wait(&p->sem_write);

    printf("\n\t Thread A running\n");

    if (fgets(str, MAXCHAR, fp) != NULL)
    {
    	printf("File read: %s \n", str);

    	// record string length, including null terminator
			len = strlen(str) + 1;

			// write string to pipe
	  	write(fd[1], str, len);
		}
		else
		{
			// Indicate EOF reached
			z = 0;

			// Closing Pipe pointers
			close(fd[0]);
			close(fd[1]);

			printf("\n ------ Reading Complete ------ \n");
		}

		// Signal Thread B to proceed
		sem_post(&p->sem_read);
	}  

    // Close file
    fclose(fp);
    printf("\n");

	  
	 return 0;
}

void* ThreadB(void *params) 
{ 
  //TODO: add your code
  ThreadParams* p = (ThreadParams*)params;

  while(1)
  {
  	// wait for pipe read signal from Thread A
    sem_wait(&p->sem_read);

    printf("\n\t Thread B running\n");

    // Checks if Thread A at EOF 
    if (z == 0)
    {
    	// Thread C signalled then break from while loop
    	printf("\t*complete*\n");
    	sem_post(&p->sem_justify);
    	break;
    }

    // Read from pipe
    read(fd[0], PipeReadData, len);
		printf("Pipe read: %s \n", PipeReadData);

    // Signal Thread C to proceed
    sem_post(&p->sem_justify);

    
  }
  return 0;
}

void* ThreadC(void *params) 
{
  //TODO: add your code
  ThreadParams* p = (ThreadParams*)params;

  // initialise file variables
	FILE *fp;
	char* filename = SRC_FILE;

	// Open data file
	fp = fopen(filename, "w");
	if (fp == NULL)
	{
	    printf("Could not open file %s",filename);
	    return 1;
	}

	// Indicates whe
	int headerEnd = 0;

  while(1)
  {
  	sem_wait(&p->sem_justify);
	  printf("\n\t Thread C running\n");

	  if (z == 0)
    {
    	// EOF reached, break from while loop
    	printf("\t*complete*\n");
    	break;
    }

    // Check if data file content has been reached
	  if (headerEnd)
	  {
	  	// write string to source file
	  	fprintf(fp, "%s", PipeReadData);
	  	printf("File write: %s\n\n", PipeReadData);
	  }
	  else
	  {
	  	printf("\t still in header\n");
	  }

	  // Check if end of header line has been reached
	  if (strstr(PipeReadData, HEADER_END) != NULL)
	  {
  		headerEnd = 1;
  		printf("end of header recognised\n");
	  }

	  // Signal Thread A to proceed
	  sem_post(&p->sem_write);

	}

  return 0;
}
