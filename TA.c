#include <stdio.h>			
#include <pthread.h>		//for creating the threads
#include <semaphore.h>		//for creating semaphores
int CC = 0;
int CI = 0;
pthread_t S[5];		//5 threads as a student
pthread_t T;				//1 thread for TA.
sem_t Tsleep;
sem_t St;
sem_t C[3];
pthread_mutex_t CA;
void *Teacher()
{
	while(1)
	{
		sem_wait(&Tsleep);		//Teacher  is currently sleeping.
		printf("Student awakened the Teacher.\n");
		while(1)
		{
			// locking the occupied chair in Teacher room 
			pthread_mutex_lock(&CA);
			if(CC == 0) 
			{//if chairs are empty, break the loop.
				pthread_mutex_unlock(&CA);
				break;
			}
			//teacher gets next student on chair.
			sem_post(&C[CI]);
			CC--;
			printf("Student left the chair. Remaining Chairs %d\n", 3 - CC);
			CI = (CI + 1) % 3;
			pthread_mutex_unlock(&CA);
			// unlocking the occupied chair in Teacher room
            printf("\t Teacher is currently helping the student.\n");
			sleep(5);
			sem_post(&St);
		}
	}
}
void *Student(void *tID) 
{
	int t;
	while(1)
	{
		printf("Student %ld is doing project .\n", (long)tID);
		t = 7;
		sleep(t);	
		printf("Student %ld needs help from the Teacher\n", (long)tID);
		pthread_mutex_lock(&CA);
		int count = CC;
		pthread_mutex_unlock(&CA);
		if(count < 3)		//Student is trying to sit on a chair
		{
			if(count == 0)		//If all the chair in vacant , student wakeup the teacher 
				sem_post(&Tsleep);
			else
				printf("Student %ld sat on a chair waiting for the teacher to finish. \n", (long)tID);
			// lock
			pthread_mutex_lock(&CA);
			int index = (CI + CC) % 3;
			CC++;
			printf("Student sat on chair.Chairs Remaining: %d\n", 3 - CC);
			pthread_mutex_unlock(&CA);
			// unlock
			sem_wait(&C[index]);		//Student leaves the chair.
			printf("\t Student %ld is getting help from the teacher. \n", (long)tID);
			sem_wait(&St);		//Student is waiting.
			printf("Student %ld left Teacher room.\n",(long)tID);
		}
		else 
			printf("Student %ld will return at another time. \n", (long)tID);
			//when all the chairs are already occupied
	}
}
int main(int argc, char* argv[])
{
	int num_s;		//	by Default its value is 5
	int id;
	sem_init(&Tsleep, 0, 0);
	sem_init(&St, 0, 0);
	for(id = 0; id < 3; ++id)			//Chairs array of 3 semaphores.
		sem_init(&C[id], 0, 0);
	pthread_mutex_init(&CA, NULL);
	
		if(argc<2)
	{
		printf("Number of student= 5 .\n");
		num_s = 5;
	}
	else
	{
		printf("Number of Students specified. Creating %d threads.\n", num_s);
		num_s = atoi(argv[1]);
	}
    //Creating 1 teacher thread and 5 Student threads.
	pthread_create(&T, NULL, Teacher, NULL);	
	for(id = 0; id < num_s; id++)
		pthread_create(&S[id], NULL, Student,(void*) (long)id);
	pthread_join(T, NULL);
	for(id = 0; id < num_s; id++)
		pthread_join(S[id], NULL);
	//Free allocated memory
	free(S); 
	return 0;
	}
