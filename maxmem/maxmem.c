#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

void get_mem(int c_pid, unsigned long *vsize)
{
	char filename[NAME_MAX+12];
	sprintf(filename, "/proc/%d/statm", c_pid);
	FILE *pst = fopen(filename, "r");

	int i;
	for (i=0; i<6; ++i)
		fscanf(pst, "%lu", vsize);

	*vsize *= sysconf(_SC_PAGE_SIZE);

	fclose(pst);
}


int main(int argc, char *argv[])
{
	int ret;
	pid_t c_pid;
	unsigned long vsize = 0, vs;
	int test[100000];
	char exefile[NAME_MAX+3];
	if (argc != 2)
		perror("argument errro! usage: maxmem *FILENAME*\n");
	switch (c_pid = fork()) {
		case -1:
			perror("fork failed!\n");
			break;
		case 0:
			/* child process */
			sprintf(exefile, "./%s", argv[1]);

			ptrace(PTRACE_TRACEME, 0, 0, 0);
			execl(exefile, exefile+2, NULL);

			break;
		default:
			/* parent process */
			wait(&ret); 
			while (WIFSTOPPED(ret)) {

				get_mem(c_pid, &vs);
				if (vs > vsize)
					vsize = vs;

				if (ptrace(PTRACE_SYSCALL, c_pid, 0, 0) != 0)
					perror("ptrace failed!\n");

				wait(&ret);
			}
			fprintf(stderr, "Maximum memory usage: %luKB\n", vsize/1024);
	}
	return 0;
}
