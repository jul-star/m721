#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h> //sleep

void *mtxf(void *val);
void *spnf(void *val);
void *rwf(void *val);
void *wwf(void *val);
// scope of mutex & vars the same!!!
pthread_mutex_t mt1 = PTHREAD_MUTEX_INITIALIZER;
int counter=0;
pthread_spinlock_t lock;
pthread_rwlock_t rl = PTHREAD_RWLOCK_INITIALIZER, wl=PTHREAD_RWLOCK_INITIALIZER;


int main (int c, char** v)
{
	pthread_t mtx, spn, rw, ww;
	int rt;
	pthread_attr_t *attr;	
	int shr;
	shr=1;
	void *res;  
	FILE *fs = fopen("home/box/main.pid","w");
	fprintf(fs,"%d",getpid());
	fclose(fs); 

	rt = pthread_create(&mtx,NULL, mtxf,(void*)&shr);
	if (rt != 0) perror("create mtx");
	

	rt = pthread_create(&spn, NULL, spnf, (void*)&shr);
	if (rt != 0) perror("create spn");

	rt = pthread_create(&rw,NULL, rwf, (void*)&shr);
	if (rt != 0) perror("create rw");

	rt = pthread_create(&ww, NULL, wwf,(void*)&shr);
	if (rt != 0) perror("create ww");
//  change shr
	shr += 1;
	counter += 1;

    printf("base:c=%d,s=%d\n",counter, shr);

//  wait for results
	rt = pthread_join(mtx,&res);
	if (rt != 0) perror("join mtx");
	if((int*)res == PTHREAD_CANCELED)
	{ printf ("mtx was closed before join");}
    printf("base:mtf return=%d\n", (int *)res);
    printf("base:c=%d,s=%d\n",counter, shr);

	return 0;
}

void *mtxf(void *val){
	int *sh;
	sh = (int*)val;
	pthread_mutex_trylock(&mt1);
	int i=0;
	while(1){
	if (i == 0) {
	counter+=10;
	(*sh)=10;
	printf("mtxf:c=%d,s=%d\n",counter,*sh);}
	i=1;
//	sleep(2);
//	pthread_mutex_unlock(&mt1);
	}
	pthread_exit( (void*)0);
}
// spin lock thread
void *spnf(void *val){
	int *sh;
    sh = (int*)val;
	pthread_spin_init(&lock, PTHREAD_PROCESS_SHARED);
	pthread_spin_lock(&lock);
	int i =0;
	while(1)
	{
	if (i ==0){
		*sh = 100;
		counter+=100;	printf("spnf:c=%d,s=%d\n",counter,*sh);
	}
	i=1;
	}
	pthread_exit(0);
}
void *rwf(void *val){
	int *sh;
	sh = (int*)val;
	pthread_rwlock_rdlock(&rl);
	int i=0;
	while(1){
	if (i==0){
		*sh = 1000;
		counter+=1000;	printf("rwf:c=%d,s=%d\n",counter,*sh);
	}
	i=1;
	}
	pthread_exit(0);
}

void *wwf(void *val){
	int *sh;
	sh = (int*)val;
	pthread_rwlock_trywrlock(&wl);
	int i=0;
	while(1){
	if(i==0){
		*sh = 10000;
		counter+=10000;printf("wwf:c=%d,s=%d\n",counter,*sh);

	}
	i=1;
	}
	
	pthread_exit(val);
}

	
