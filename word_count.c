#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "word_count.h"
#include "hashmap.h"
#include "bounded_buffer.h"
#include <unistd.h>


int print_result(char*, int);

struct buffered_queue *queue;

struct mapper_arg{
	int r;
	char* string;
};

struct reducer_arg{
	int m;
	int id;
};

void * mapper(void*args)
{
	struct mapper_arg* arguments = (struct mapper_arg*) args;
        int i, reducer_num;
	int * mapper_num_ptr;
        unsigned long hash;
        char * wrd = NULL;
	char * txt = NULL;
	const char delimiter[2] = " \0";
	char *ptr = NULL;
	char * special = NULL;


	special = "0";
	txt = arguments->string;
	reducer_num = arguments->r;
	
	wrd = strtok_r(txt, " ", &ptr);

	if(wrd == NULL)
	printf("Word is null");
	
	while(wrd != NULL)
	{
		char* tmp = malloc(sizeof(char)*strlen(wrd)+1);
		strcpy(tmp, wrd);
		hash = crc32(wrd, strlen(wrd));
		buffered_queue_push(&(queue[hash%reducer_num]), tmp);
		wrd = strtok_r(NULL, delimiter, &ptr);
      	}

	for(i = 0; i<reducer_num; i++){
		buffered_queue_push(&queue[i], special);
	}
	free(arguments);

}

void * reducer(void*args)
{
        map_t mymap;
	int m, id;
	int finishedmappers, error, count;
	char* popped;
	struct reducer_arg* arguments = (struct reducer_arg*) args;
        
	mymap = hashmap_new();

	m = arguments->m;
	id = arguments->id;
		
	finishedmappers = 0;
        count = 0;
	
	while(1)
	{	
		popped = (char*)(buffered_queue_pop(&queue[id]));
		if(strcmp(popped, "0") != 0)
		{
			error = hashmap_get(mymap, popped, &count);
			if(error==MAP_OK){
			
			hashmap_put(mymap, popped, count+1);
			}
			else{
			hashmap_put(mymap, popped, 1);
			}
			
		}
		else
		{
			finishedmappers = finishedmappers + 1;
		}

		if(finishedmappers == m)
		{	
			hashmap_iterate(mymap, print_result);
			hashmap_free(mymap);
			free(arguments);
			break;
		}

	}	
}

int print_result(char *key, int value){
    printf("count of %s = %d\n", key, value);
    free(key);
    return 0;
}


void word_count(int m, int r, char **text){

    struct mapper_arg* mapper_args;
    struct reducer_arg* reducer_args;
    pthread_t* reducer_threads;
    pthread_t* mapper_threads;
    int* mapper_num_ptr = &m; 

    int i = 0;

    mapper_threads = malloc(sizeof(pthread_t)*m);
    reducer_threads = malloc(sizeof(pthread_t)*r);

    queue = malloc(sizeof(struct buffered_queue)*r);

    for(i = 0; i < r;i++)
    {
	buffered_queue_init(&queue[i], 10);
    }
   	
    for(i = 0; i < r; i++)
    {
	reducer_args = malloc(sizeof(struct reducer_arg));
	reducer_args->m = m;
	reducer_args->id = i;
	pthread_create(&reducer_threads[i], NULL, reducer, (void*)(reducer_args));
    }


    for(i = 0; i < m; i++)
    {
	mapper_args = malloc(sizeof(struct mapper_arg));
	mapper_args->r = r;
	mapper_args->string = text[i];
	pthread_create(&mapper_threads[i], NULL, mapper, (void*)(mapper_args));
    }

    
    for(i = 0; i < m; i++)
    {
	pthread_join(mapper_threads[i], NULL);
    }

    for(i = 0; i < r; i++)
    {
	pthread_join(reducer_threads[i], NULL);
    }

    for(i = 0; i < r; i++)
    { 
    buffered_queue_destroy(&queue[i]);
    }

    
    free(mapper_threads);
    free(reducer_threads);
    free(queue);
    
}

