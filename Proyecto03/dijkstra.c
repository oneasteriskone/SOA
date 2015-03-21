#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define IN 999

void dijsktra(int cost[][98],int source,int target,int retVal[25])
{
    int dist[98],prev[98],selected[98]={0},i,m,min,start,d,j;
    int path[98];
    for(i=1;i< 98;i++)
    {
        dist[i] = IN;
        prev[i] = -1;
    }
    start = source;
    selected[start]=1;
    dist[start] = 0;
    while(selected[target] ==0)
    {
        min = IN;
        m = 0;
        for(i=1;i< 98;i++)
        {
            d = dist[start] +cost[start][i];
            if(d< dist[i]&&selected[i]==0)
            {
                dist[i] = d;
                prev[i] = start;
            }
            if(min>dist[i] && selected[i]==0)
            {
                min = dist[i];
                m = i;
            }
        }
        start = m;
        selected[start] = 1;
    }
    start = target;
    j = 0;
    while(start != -1)
    {
        printf("path for j= %d is %d\n",j,start);
        retVal[j++] = start;
        start = prev[start];
    }
}


/*
int main()
{
    int cost[98][98],i,j,w,ch;
    //char* co = malloc(200);
    int source, target,x,y;
    printf("\tShortest Path Algorithm(DIJKSRTRA's ALGORITHM\n\n");
    for(i=1;i< 98;i++)
    for(j=1;j< 98;j++)
    cost[i][j] = IN;
    for(x=1;x< 98;x++)
    {
        for(y=x+1;y< 98;y++)
        {
            printf("Enter the weight of the path between node %d and %d: ",x,y);
            scanf("%d",&w);
            cost [x][y] = cost[y][x] = w;
        }
        printf("\n");
    }
    printf("\nEnter The Source:");
    scanf("%d", &source);
    printf("\nEnter The target");
    scanf("%d", &target);
    int co[25];
    i = 0;
    for(i;i<25;i++)co[i] = IN;
    dijsktra(cost,source,target,co);
    i = 0;
    while(co[i] != IN){
        printf("%d ",co[i++]);
    }   
    //printf("\nShortest Path: %s\n",co);
}
*/
