#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

int  main(int  argc, char *argv[]) {
		int ShmID;
		int *ShmPTR;
		pid_t  pid;
// 		int    status;
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
	
		pid = fork();
		
		if (pid > 0) { // parent process
			int loops = 0;

			while (loops < 25) {
				while (*turn == 0);
				printf("i'm dear old dad bank acc = $%d\n", *bankAccount);
				*bankAccount += 10;
				*turn = 0;
				loops++;
				printf("just added 10 to bank acc = $%d\n", *bankAccount);
			}
		} else {
			int loops = 0;

			while (loops < 25) {
				while (*turn == 1);
				printf("i'm poor student :( bank acc = $%d\n", ShmPTR[0]);
				*turn = 1;
				loops++;
				*bankAccount-=1;
				printf("just subtracted 1 bank acc = $%d\n", ShmPTR[0]);
			}
		}

	 printf("Process has detected the completion of its child...\n");
	 shmdt((void *) ShmPTR);
	 printf("Process has detached its shared memory...\n");
	 shmctl(ShmID, IPC_RMID, NULL);
	 printf("Process has removed its shared memory...\n");
	 printf("Process exits...\n");
	 exit(0);
}

