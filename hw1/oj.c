#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


typedef unsigned long sz_t;


const char *RES[6] = {"CORRECT", "WRONG-ANSWER", "RUN-ERROR", "TIME-LIMIT", "MEMORY-LIMIT", "FILE-LIMIT"};
enum {AC, WA, RE, TLE, MLE, FLE};

const char *JUDGE_OUT = "/tmp/judge-output.shaform";


int cmp(FILE *a, FILE *b)
{
	int t;
	while ((t=fgetc(a)) == fgetc(b))
		if (t == EOF)
			return 1;
	return 0;
}
sz_t get_file(const char *ef)
{
        struct stat st;

	if (stat(ef, &st) == -1)
		return 0;
        return (sz_t ) st.st_size;
}

void solve(const char *ef, const char *in, const char *out, int tl, int ml, int fl)
{
	int ret, res;
	pid_t c_pid;
	unsigned long vsize = 0;

	struct rusage ru;

	/*int fd[2];*/
	FILE *exout, *solout;
	sz_t mem, used, file;

	/*
	if (pipe(fd) == -1) {
		perror("Pipe failed\n");
		return;
	}
	*/

	switch (c_pid = fork()) {
		case -1:
			perror("fork failed!\n");
			break;
		case 0:
			/* child process */
			freopen(in, "r", stdin);
			freopen(JUDGE_OUT, "w", stdout);
			execl(ef, ef, NULL);

			break;
		default:
			/* parent process */
			/* close wirte end */
			/*close(fd[1]);*/


			wait3(&ret, WUNTRACED, &ru); 

			mem = ru.ru_minflt*sysconf(_SC_PAGESIZE)/1024;
			used = ru.ru_utime.tv_sec*1000000 + ru.ru_utime.tv_usec
				+ ru.ru_stime.tv_sec*1000000 + ru.ru_stime.tv_usec;
			used /= 1000000;

			file = get_file(JUDGE_OUT);

			/*exout = fdopen(fd[0], "r");*/
			exout = fopen(JUDGE_OUT, "r");
			solout = fopen(out, "r");

			if (WIFEXITED(ret)) {
				if (used > tl)
					res = TLE;
				else if (mem > ml)
					res = MLE;
				else if (file > fl)
					res = FLE;
				else if (cmp(exout, solout))
					res = AC;
				else
					res = WA;

			} else
				res = RE;

			fclose(solout);
			fclose(exout);

			printf("%s: %lu %lu %lu\n", RES[res], used, mem, file);

	}
}

const size_t BSZ = NAME_MAX+1;
int main()
{
	char ef[BSZ], in[BSZ], out[BSZ];
	int tl, ml, fl;
	while (scanf("%s %s %s %d %d %d", ef, in, out, &tl, &ml, &fl) == 6) {
		solve(ef, in, out, tl, ml, fl);
	}
	return 0;
}
