#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

int main()
{
	pid_t c_pid;
	int i;
	for (i=0; i<10; ++i) {
		switch (c_pid = fork()) {
			case -1:
				perror("fork failed!\n");
				return 0;
			case 0:
				/* child process */
				//execl("/bin/ls", "ls", NULL);
				return 0;

				break;
			default:
				/* parent process */
				printf("fork %d!\n", i+1);
				break;
		}
			
	}
	while (1);
	return 0;
}
