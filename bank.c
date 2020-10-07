#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <time.h> 

void  deposit(int* bankAccount);
void  request(int* bankAccount);

int  main(int  argc, char *argv[]) {
		int ShmID;
		int *ShmPTR;
		pid_t  pid;
		int* bankAccount;
		int* turn;

		ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
		if (ShmID < 0) {
			printf("*** shmget error ***\n");
			exit(1);
		}
		printf("Process has recieved shared memory of 2 integers...\n");

		ShmPTR = (int *) shmat(ShmID, NULL, 0);

		if (*ShmPTR == -1) {
			printf("*** shmat error (server) ***\n");
			exit(1);
		}
		printf("Process has attached the shared memory...\n");

		ShmPTR[0] = 0;
		ShmPTR[1] = 0;
		
		bankAccount = &ShmPTR[0];
		turn = &ShmPTR[1];

		srand(time(0));

		pid = fork();
		
		if (pid > 0) { // parent process
			int loops = 0;

			while (loops < 25) {
				int randSleep = (rand() % 1);
				sleep(randSleep);

				while (*turn == 0); 
				if (*bankAccount <= 100) { // critical section
					deposit(bankAccount);
				} else {
					printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", *bankAccount);
				}
				*turn = 0; // change turn
				loops++; // non-critical section
			}
				wait(&pid);
				printf("Process has detected the completion of its child...\n");
				shmdt((void *) ShmPTR);
				printf("Process has detached its shared memory...\n");
				shmctl(ShmID, IPC_RMID, NULL);
				printf("Process has removed its shared memory...\n");
				printf("Process exits...\n");
				exit(0);
		} else {
			int loops = 0;

			while (loops < 25) {
				int randSleep = (rand() % 1);
				sleep(randSleep);
				while (*turn == 1);
				request(bankAccount); // critical section
				*turn = 1; // change turn
				loops++; // non-critical section
			}
			exit(0);
		}
}

void deposit(int* bankAccount) {
	int randMoney = (rand() % 100);
	if ((randMoney % 2) == 0) {
		*bankAccount += randMoney;
		printf("Dear old Dad: Deposits $%d / Balance = $%d\n", randMoney, *bankAccount);
	} else {
		printf("Dear old Dad: Doesn't have any money to give\n");
	}
}

void request(int* bankAccount) {
	int requested = (rand() % 50);
	printf("Poor Student needs $%d\n", requested);
	if (requested <= *bankAccount) {
		*bankAccount -= requested;
		printf("Poor Student: Withdraws $%d / Balance = $%d\n", requested, *bankAccount);
	} else {
		printf("Poor Student: Not Enough Cash ($%d)\n", *bankAccount);
	}
}
