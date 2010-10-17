#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
/*
void get_mem(int c_pid, unsigned long *vsize)
{
	char filename[NAME_MAX+12];
	sprintf(filename, "/proc/%d/statm", c_pid);
	FILE *pst = fopen(filename, "r");

	fscanf(pst, "%lu", vsize);

	*vsize *= sysconf(_SC_PAGE_SIZE);

	fclose(pst);
}
void get_mem(int c_pid, unsigned long *vsize)
{
	struct rusage buf;
	getrusage(RUSAGE_CHILDREN, &buf);
	printf("%ld\n", buf.ru_minflt);
	*vsize = buf.ru_minflt*sysconf(_SC_PAGESIZE);
}
*/

int main(int argc, char *argv[])
{
	int ret;
	pid_t c_pid;
	unsigned long vsize = 0;
	char exefile[NAME_MAX+3];
	if (argc != 2) {
		perror("argument error! usage: maxmem *FILENAME*\n");
		return 1;
	}

	struct rusage ru;

	switch (c_pid = fork()) {
		case -1:
			perror("fork failed!\n");
			break;
		case 0:
			/* child process */
			sprintf(exefile, "./%s", argv[1]);
			execl(exefile, exefile+2, NULL);

			break;
		default:
			/* parent process */
			wait3(NULL, WUNTRACED, &ru); 
			fprintf(stderr, "Maximum memory usage: %luKB\n", (long int)ru.ru_maxrss);
			
	}
	return 0;
}
