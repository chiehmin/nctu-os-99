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

const char *JUDGE_OUT = "judge-output.shaform";


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

	struct rusage ru;

	FILE *exout, *solout;
	sz_t mem, used, file;


	switch (c_pid = fork()) {
		case -1:
			perror("fork failed!\n");
			return;
		case 0:
			/* child process */
			freopen(in, "r", stdin);
			freopen(JUDGE_OUT, "w", stdout);

			struct rlimit lim;
			// set time limit
			lim.rlim_cur = tl+1;
			lim.rlim_max = 2*lim.rlim_cur;
			setrlimit(RLIMIT_CPU, &lim);
			// set output limit
			lim.rlim_cur = fl;
			lim.rlim_max = 2*lim.rlim_cur;
			setrlimit(RLIMIT_FSIZE, &lim);
			// run !!
			execl(ef, ef, NULL);

			return;
		default:
			/* parent process */

			wait3(&ret, 0, &ru); 


			if (WIFSIGNALED(ret)) {
				switch (WTERMSIG(ret)) {
					case SIGXFSZ:
						res = FLE;
						break;
					case SIGXCPU:
						res = TLE;
						break;
					default:
						res = RE;
				}
			} else {
				exout = fopen(JUDGE_OUT, "r");
				solout = fopen(out, "r");

				mem = ru.ru_minflt*getpagesize()/1024;
				used = ru.ru_utime.tv_sec*1000000 + ru.ru_utime.tv_usec
					+ ru.ru_stime.tv_sec*1000000 + ru.ru_stime.tv_usec;
				used /= 1000;

				file = get_file(JUDGE_OUT);

				if (mem > ml)
					res = MLE;
				else if (file > fl)
					res = FLE;
				else if (cmp(exout, solout))
					res = AC;
				else
					res = WA;

				fclose(solout);
				fclose(exout);
			}

			if (res == AC) {
				printf("CORRECT: %lu %lu %lu\n", used, mem, file);
			} else {
				printf("%s\n", RES[res]);
			}
			fflush(stdout);
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
