/*
	Name: Mahmoud Natsheh
	ID: 1001860023
*/

// The MIT License (MIT)
// 
// Copyright (c) 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// // 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

//Enum that will be used to declare if a spot in memory is free or used 
enum TYPE
{
	FREE = 0,
	USED
};

//linked list that will hold our arena allocations, it will tell us how large
//the requested space is, is it used or not, its start, its pointer to the next
// node, and the pointer to the previous node
struct Node {
	size_t size;
	enum TYPE type;
	void * arena;
	struct Node * next;
	struct Node * prev;
};

//arena head pointer
struct Node *alloc_list;
struct Node *previous_node;


void * arena;

//initalization of the algorith enum, this will allow the program to know which
//type of algorithm to run on the allocation of memory, in this case we will
//run the first fit algorithm
enum ALGORITHM allocation_algorithm = FIRST_FIT;

//Function that will allow us to allocate a pool of memory size bytes big  
int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
	//if the size requested is less than zero then we cannot allocate a pool of
	//memory that is not a valid size
	if( size < 0 )
	{
		return -1;
	}
	//if the of memory size requested is valid, we will allocate the pool of
	//memroy using malloc
	arena = malloc( ALIGN4( size ) );
	//this will allow us to allocate memory in the algorithm style requested
	allocation_algorithm = algorithm;

	alloc_list = ( struct Node * )malloc( sizeof( struct Node ));
	//initialize the linked list (head, size, and if the node in linked list
	//is free or used)
	alloc_list -> arena = arena;
	alloc_list -> size  = ALIGN4(size);
	alloc_list -> type  = FREE;
	alloc_list -> next  = NULL;
	alloc_list -> prev  = NULL;

	previous_node  = alloc_list;

	return 0;
}

//this function will free the allocated arena pool and empty the linked list
void mavalloc_destroy( )
{
	//freeing the arena will release the unused memroy allocated
	free( arena );
	arena = NULL;

	// iterate over the linked list and free the nodes
	while (alloc_list)
	{
		free(alloc_list);
		alloc_list = alloc_list -> next;
	}
	alloc_list = NULL;

	return;
}

//this function will allocate memory from our preallocated pool of memory 
//that we already allocated, this function will input data into memory
void * mavalloc_alloc( size_t size )
{
	struct Node * node;
	//The next fit algorithm won't always start looking for free memory from
	//the head of the memory, but it will continue from where it left off
	//all the other algorithms will start from the head of the memory
	if( allocation_algorithm != NEXT_FIT )
	{
		node = alloc_list;
	}
	else if ( allocation_algorithm == NEXT_FIT )
	{
		node = previous_node;
	}
	else
	{
		printf("ERROR: Unknown allocation algorithm!\n");
		exit(0);
	}

	//standardize the block size to 4 bytes aligned to allow the requested size
	//to be standardized when allocating more memory
	size_t aligned_size = ALIGN4( size );

	//Implement First Fit
	if( allocation_algorithm == FIRST_FIT )
	{
		//iterate through the pool of memroy finding free spots to put data into
		//until you reach the end of the pool
		while( node )
		{
			//if the spot in memory is greater than or equal to the size of memory
			//needed and the is spot free to put memory into, then use the spot
			if( node -> size >= aligned_size  && node -> type == FREE )
			{
				int leftover_size = 0;

				//when using the spot you must mark it as used and set the new size
				//available in the spot to the size reqused minus the original size
				node -> type  = USED;
				leftover_size = node -> size - aligned_size;
				node -> size =  aligned_size;

				//if when we use the memory for our data and there is still left over
				//space in the spot, we need to split the space and make it available
				//to other data to use
				if( leftover_size > 0 )
				{
					struct Node * previous_next = node -> next;
					struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

					leftover_node -> arena = node -> arena + size;
					leftover_node -> type  = FREE;
					leftover_node -> size  = leftover_size;
					leftover_node -> next  = previous_next;

					node -> next = leftover_node;
				}
				previous_node = node;
				return ( void * ) node -> arena;
			}
			node = node -> next;
		}
	}
	// Implement Next Fit
	struct Node * previous;
	if( allocation_algorithm == NEXT_FIT )
	{
		//when we start next fit, it must start from the end of the previous node,
		//if it is NULL that means we must start from the head of the linked list
		if(previous == NULL)
		{
			previous = alloc_list;
		}
		struct Node * node_ptr = previous;
		//iterate through the pool of memroy finding free spots to put data into
		//until you reach the end of the pool
		while( node )
		{
			//if the spot in memory is greater than or equal to the size of memory
			//needed and the spot is free to put memory into, then use the spot
			if( node -> size >= aligned_size  && node -> type == FREE )
			{
				int leftover_size = 0;

				//when using the spot you must mark it as used and set the new size
				//available in the spot to the size reqused minus the original size
				node -> type  = USED;
				leftover_size = node -> size - aligned_size;
				node -> size =  aligned_size;

				//if when we use the memory for our data and there is still left over
				//space in the spot, we need to split the space and make it available
				//to other data to use
				if( leftover_size > 0 )
				{
					struct Node * previous_next = node -> next;
					struct Node * leftover_node = ( struct Node * ) malloc ( sizeof( struct Node ));

					leftover_node -> arena = node -> arena + size;
					leftover_node -> type  = FREE;
					leftover_node -> size  = leftover_size;
					leftover_node -> next  = previous_next;

					node -> next = leftover_node;
				}
				previous_node = node;
				return ( void * ) node -> arena;
			}
			node = node -> next;
			//we will keep iterating the list of memory until our node is the previous
			//node we used, this will indicate that we reached the end of our list
			//and must break/exit
			if(node == previous)
			{
				break;
			}
			//if we are pointing at nothing in our linked list, we must loop back to
			//the beginning and see what spot are available in the list before
			if(node == NULL)
			{
				node = alloc_list;
			}
		}
	}
	// Implement Best Fit
	struct Node * winner;
	if( allocation_algorithm == BEST_FIT )
	{
		//keep looping over the linked list until you find the minimum space available
		//that will fit the data we need, best fit finds just the smallest amount
		//of space needed to fit the data set then inserts data into there
		int winning_size = INT_MAX;
		while( node )
		{
			//if the size of the node/space in memory is less than the previous size
			//found and it is free and fits the data set, then save that spot to later
			//add our data into it
			if((node -> size - aligned_size) < winning_size  && node -> type == FREE )
			{
				winner = node;
				winning_size = node -> size - aligned_size;
				return winner;
			}
			node = node -> next;
		}
	}
	// Implement Worst Fit
	if( allocation_algorithm == WORST_FIT )
	{
		//keep looping over the linked list until you find the maximum space available
		//that will fit the data we need, worst fit finds just the biggest amount
		//of space needed to fit the data set then inserts data into there
		int wWinning_size = INT_MIN;
		while( node )
		{
			//if the size of the node/space in memory is greater than the previous size
			//found and it is free and fits the data set, then save that spot to later
			//add our data into it
			if((node -> size - aligned_size) < wWinning_size  && node -> type == FREE)
			{
				winner = node;
				wWinning_size = node -> size - aligned_size;
				return winner;
			}
			node = node -> next;
		}
	}

	//if failed to find size in memory to fit data return NULL
	return NULL;
}

//this function will free any two free consecutive blocks of memory and 
//combine them
void mavalloc_free( void * ptr )
{
	//loop to the end of the linked list and if there are two consecutive
	//empty free memeory spots next to each other combine them
	struct Node * node = alloc_list;
	while (node)
	{
		//check if the current node is the same the previous node, then free
		//them and combine
		if(node -> arena == ptr)
		{
			node -> type = FREE;
			break;
		}
		node = node -> next;
	}
	return;
}

//this function calculates how many nodes/spaces we have in our memory and 
//return the amount of nodes available in memory
int mavalloc_size( )
{ 
	//start from the beginning of the linked list
	int number_of_nodes = 0;
	struct Node * ptr = alloc_list;
	//iterate through the whole linked list and add one for every node found
	while( ptr )
	{
		number_of_nodes ++;
		ptr = ptr -> next;
	}
	//return the counted nodes in the linked list
	return number_of_nodes;
}

