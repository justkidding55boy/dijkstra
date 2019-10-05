// 61706613 工藤瑛士

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

// Eiji's solution

#include <string.h>
#define TRUE 1
#define FALSE 0

int *connectedArray(int *sptSet, int root) {
    //繋がっているものの中でrootからのコストの配列を持ってくる
    //+まだN'に入っていない中で
    
    int *theConnectedArray;


    int i;
    for (i = 0; i < NNODE; i++) {
        if (sptSet[i] == FALSE && cost[root][i] != INF) {
            theConnectedArray = cost[root][i];

	    
        }
    }
}

//This check whether the node is in n_now

int minDistance(int *dist, int *sptSet, int root) {
    int min = INF;
    int min_index = INF;
//繋がっているもので最小のものを持ってくる
    int theArray = connectedArray(sptSet, root)
    
}


// get the root node
// return the dist set and prev set
void dijkstra(int root) {
    //Effectives: compose the min cost to the node to dist[NNODE]
    //compose the previous node to prev[NNODE]
    
    //sptSet includes the calculated nodes
    int sptSet[NNODE];

    //initialize
    int i;
    for (i = 0; i < NNODE; i++) {
        dist[i] = INF;
        sptSet[i] = FALSE;       
    }

    //route to route is 0
    dist[root] = 0;

    //繋がっているもので最小のものを持ってくる
    //持ってきたら周りをupdateする
   
    int *targetting_cost = connectedArray(sptSet, root);

    int min = INF;
    for (i = 0; i < NNODE; i++) {
	if (targetting_cost[i] < min) {
	    min = targetting_cost;
	}
    }

    int u = minDistance(dist, sptSet, root);

    


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
