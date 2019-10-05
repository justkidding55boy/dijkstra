// 61706613 Eiji Kudo

#include <stdio.h>

// determined input
#define NNODE 6
#define INF 100 // infinity
int cost[NNODE][NNODE] = {
    {  0,  2, 5,  1, INF, INF},
    {  2,  0, 3,  2, INF, INF},
    {  5,  3, 0,  3,   1,   5},
    {  1,  2, 3,  0,   1, INF},
    {INF,INF, 1,  1,   0,   2},
    {INF,INF, 5, INF,  2,   0}
};

int dist[NNODE];
int prev[NNODE];

// determiend input

// From here: Eiji's solution


#define TRUE 1
#define FALSE 0

int minDistance(int *dist, int *sptSet) {
    //this function returns the min number of dist[], using the sptSet

    int min = INF;
    int min_index = INF;

    //search for the min route around the node
    int v;
    for (v = 0; v < NNODE; v++) {
        if (sptSet[v] == FALSE && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}


// get the root node
// return the dist set and prev set
void dijkstra(int root) {
    //sptSet is the set of calculated nodes
    //if it's calculated, the node is TRUE
    int sptSet[NNODE];

    //initialize
    int i;
    for (i = 0; i < NNODE; i++) {
        dist[i] = INF;
        sptSet[i] = FALSE;       
    }

    //route to route is 0
    dist[root] = 0;
    prev[root] = root;

    int cnt;
    for (cnt = 0; cnt < NNODE - 1; cnt++) {

        // u is the index of the min distance
        // return with answered dist, sptSet is the hint to look for the distance
        int u = minDistance(dist, sptSet);
        sptSet[u] = TRUE;

        //update the dist value of the adjacenet v
        int v;
        for (v = 0; v < NNODE; v++) {
            if (!sptSet[v] && dist[u] + cost[u][v] < dist[v]) {
                dist[v] = dist[u] + cost[u][v];
		prev[v] = u;
            }
        }
    }

}

int main() {

    //error handling
    int i;
    int input_judge = 0;
    for (i = 0; i < NNODE; i++) {
	input_judge = 0;
	int j;
	for (j = 0; j < NNODE; j++) {
	    if (cost[i][j] == 0) {
		input_judge++;
	    }
	    if (cost[i][j] > INF) {
		printf("Some of the cost is larger than INF\n");
		return 1;
	    }
	}

	if (input_judge != 1) {
	    printf("This input is incorrect!\n");
	    return 1;
	}
    }

    for (i = 0; i < NNODE; i++) {
        dijkstra(i);
        printf("root node %c:\n", 'A' + i);
        int j;
        for (j = 0; j < NNODE; j++) {
            printf("[%c,%c,%d] ", 'A'+j, 'A'+prev[j], dist[j]);
        }
        printf("\n");
    }

    return 0;
}
