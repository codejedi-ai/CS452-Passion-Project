#include <stdio.h>
#include <stdlib.h>
#include "track_node.h"
#include "track_data_new.h"
#include "C-heap/heap.h"
// make a heap consists of the following datastructure

struct priorityQueueElement
{
	int *distance;
	track_node *current_node;
};

int compare(void *a, void *b)
{
	struct priorityQueueElement *a1 = (struct priorityQueueElement *)a;
	struct priorityQueueElement *b1 = (struct priorityQueueElement *)b;
	int distance_a = *(a1->distance);
	int distance_b = *(b1->distance);
	if (distance_a > distance_b)
	{
		return -1;
	}
	if (distance_a < distance_b)
	{
		return 1;
	}
	return 0;
}
void printHeap(struct heap *h)
{
	for (int i = 0; i < h->size; i++)
	{
		struct priorityQueueElement *p = (struct priorityQueueElement *)h->heapData[i];
		printf("%s ", p->current_node->name);
	}
	
	printf("\nHeap Size %d\n", h->size);
	// print the memory address of the elements in the heap
	for (int i = 0; i < h->size; i++)
	{
		struct priorityQueueElement *p = (struct priorityQueueElement *)h->heapData[i];
		printf("%p ", p);
	}
	printf("\n");
}
int track_dijkstra(track_node *start, track_node *end)
{
	// make a heap
	struct heap* h = (struct heap*)malloc(sizeof(struct heap));
	initHeap(h, compare);
	// make a visited array
	int visited[NODE_EXIT + 1][TRACK_MAX];
	for (int i = 0; i < NODE_EXIT; i++)
	{
		for (int j = 0; j < TRACK_MAX; j++)
		{
			visited[i][j] = 0;
		}
	}
	visited[start->type][start->num] = 1;
	// make a distance array
	int distance[NODE_EXIT + 1][TRACK_MAX];
	for (int i = 0; i < NODE_EXIT; i++)
	{
		for (int j = 0; j < TRACK_MAX; j++)
		{
			distance[i][j] = 1000000;
		}
	}
	distance[start->type][start->num] = 0;
	// make previous array
	track_node *previous[NODE_EXIT + 1][TRACK_MAX];
	// make a priorityQueueElement
	struct priorityQueueElement* p = (struct priorityQueueElement*)malloc(sizeof(struct priorityQueueElement));
	p->distance = &distance[start->type][start->num];
	p->current_node = start;
	insert(h, p);
	// print the start path from the start to the end
	printf("start %s to end %s\n", start->name, end->name);
	printf("start %s\n", start->name);
	while (!isEmpty(h))
	{
		// print the name of the current node
		struct priorityQueueElement *current = (struct priorityQueueElement *)removeMax(h);

		
		track_node *current_node = current->current_node;
		int current_distance = *(current->distance);
		free(current);
		visited[current_node->type][current_node->num] = 1;
		// there are three cases
		// check straight
		// if the track tup is a branch then check the DIR_CURVED
		track_node *rev_node = current_node->reverse;
		// add the nodes with the distance to the heap
		if (visited[rev_node->type][rev_node->num] == 0 && current_distance <= distance[rev_node->type][rev_node->num])
		{
			
			distance[rev_node->type][rev_node->num] = current_distance;
			previous[rev_node->type][rev_node->num] = current_node;
			struct priorityQueueElement* p1 = (struct priorityQueueElement*)malloc(sizeof(struct priorityQueueElement));
			p1->distance = &distance[rev_node->type][rev_node->num];
			p1->current_node = rev_node;
			printf("Reversed %s %s distance from start %d\n", current_node->name, rev_node->name, distance[rev_node->type][rev_node->num]);
			insert(h, p1);
			printHeap(h);
		}
		if (current_node->type != NODE_EXIT)
		{
			track_edge *edge1 = &current_node->edge[DIR_AHEAD];
			track_node *next_node = edge1->dest;
			if (visited[next_node->type][next_node->num] == 0 && current_distance + edge1->dist <= distance[next_node->type][next_node->num])
			{
				
				distance[next_node->type][next_node->num] = current_distance + edge1->dist;
				previous[next_node->type][next_node->num] = current_node;
				struct priorityQueueElement* p2 = (struct priorityQueueElement*)malloc(sizeof(struct priorityQueueElement));
				p2->distance = &distance[next_node->type][next_node->num];
				p2->current_node = next_node;
				// print in the format of the go straight, start end distance from start
				printf("Straight %s %s distance from start %d\n", current_node->name, next_node->name, distance[next_node->type][next_node->num]);
				insert(h, p2);
				printHeap(h);
			}
		}
		if (current_node->type == NODE_BRANCH)
		{
			track_edge *edge2 = &current_node->edge[DIR_CURVED];
			track_node *curved_node = edge2->dest;
			if (visited[curved_node->type][curved_node->num] == 0 && current_distance + edge2->dist <= distance[curved_node->type][curved_node->num])
			{
				
				distance[curved_node->type][curved_node->num] = current_distance + edge2->dist;
				previous[curved_node->type][curved_node->num] = current_node;
				struct priorityQueueElement* p3 = (struct priorityQueueElement*)malloc(sizeof(struct priorityQueueElement));
				p3->distance = &distance[curved_node->type][curved_node->num];
				p3->current_node = curved_node;
				printf("Curved %s %s distance from start %d\n", current_node->name, curved_node->name, distance[curved_node->type][curved_node->num]);
				insert(h, p3);
				printHeap(h);
			}
		}
	}
	free(h);
	// now we need to print the path
	printf("Print the path\n");
	track_node *current = end;
	while (current != start)
	{
		if (current == NULL)
		{
			printf("No path\n");
			break;
		}
		printf("%s\n", current->name);
		current = previous[current->type][current->num];
	}
	
	return distance[end->type][end->num];
}
int main()
{
	track_node track[TRACK_MAX];
	init_tracka(track);
	track_node *start = &track[0];
	track_node *end = &track[40];
	track_dijkstra(start, end);
	return 0;
}