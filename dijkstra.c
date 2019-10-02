// 61706613 工藤瑛士

#include <stdio.h>

// determined input
#define NNODE 6
#define INF 100 // infinity
/*int cost[NNODE][NNODE] = {
    {  0,  2, 5,  1, INF, INF},
    {  2,  0, 3,  2, INF, INF},
    {  5,  3, 0,  3,   1,   5},
    {  1,  2, 3,  0,   1, INF},
    {INF,INF, 1,  1,   0,   2},
    {INF,INF, 5, INF,  2,   0}
};*/

int cost[NNODE][NNODE] = {
    {  0,  2, 5,  1, 0, 0},
    {  2,  0, 3,  2, 0, 0},
    {  5,  3, 0,  3,   1,   5},
    {  1,  2, 3,  0,   1, 0},
    {0,0, 1,  1,   0,   2},
    {0,0, 5, 0,  2,   0}
};


int dist[NNODE];
int prev[NNODE];

// determiend input

// Eiji's solution

#include <string.h>
#define TRUE 1
#define FALSE 0

//This check whether the node is in n_now

int minDistance(int *dist, int *sptSet) {

    int min = INF;
    int min_index;

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
    int sptSet[NNODE];

    //initialize
    int i;
    for (i = 0; i < NNODE; i++) {
        dist[i] = INF;
        sptSet[i] = FALSE;       
    }

    //route to route is 0
    dist[root] = 0;

    int cnt;
    for (cnt = 0; cnt < NNODE - 1; cnt++) {

        // u is the index of the min distance
        // return with answered dist, sptSet is the hint to look for the distance
        int u = minDistance(dist, sptSet);
        sptSet[u] = TRUE;

        //update the dist value of the adjacenet v
        int v;
        for (v = 0; v < NNODE; v++) {
            if (!sptSet[v] && cost[u][v] != 0 && cost[u][v] != INF && dist[u] != INF
                    && dist[u] + cost[u][v] < dist[u]) {
                dist[v] = dist[u] + cost[u][v];
            }
        }
    }

}

int main() {

    int i;
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
