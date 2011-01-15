#include "mythread.h"
#include <stdio.h>

extern int curr;

int beg=0, end=0;
int arr[100];

void *runner1(void *param); /* the thread */
void *runner2(void *param); /* the thread */

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr,"usage: a.out \n");
		/*exit(1);*/
		return -1;
	}

	if (atoi(argv[1]) < 0) {
		fprintf(stderr,"Argument %d must be non-negative\n",atoi(argv[1]));
		/*exit(1);*/
		return -1;
	}
	int t = atoi(argv[1]);

	th_t *ths = malloc(t*sizeof(th_t));

	/* get the default attributes */
	setpreemptive();// the scheduler will be preemptive

	int i;
	for (i=0; i<t; ++i) {
		th_init(&ths[i]);
		if (i%2)
			th_fork(&ths[i], runner1, argv[1]);
		else
			th_fork(&ths[i], runner2, argv[1]);
	}

	/* now wait for the thread to exit */
	int ret;
	for (i=0; i<t; ++i) {
		th_wait(&ths[i], &ret);
		printf("thread #%d, ret:%d\n", i, ret);
	}
	printf("test...\n");
	free(ths);
	return 0;
}

void *runner1(void *param)
{
	int i, upper = atoi(param);
	static int test=0;
	i=0;
	int j,k,l;
	while (++i <= upper) {
		while ((end+1) % 100 == beg);
		arr[end++] = i;
		printf("runner 1 %d at %d produced %d\n", upper, curr, i);
		for (k=0; k< 1000000; ++k) ++l;
	}

	int t = 1;
	return &t;
}
void *runner2(void *param)
{
	int i, upper = atoi(param);
	static int test=0;
	i=0;
	int j,k,l;
	while (++i <= upper) {
		while (end == beg);
		j = arr[beg++];
		printf("runner 2 %d at %d consumed %d\n", upper, curr, j);
		for (k=0; k< 1000000; ++k) ++l;
	}

	int t = 2;
	return &t;
}
