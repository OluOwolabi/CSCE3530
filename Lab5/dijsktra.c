//Name: Olu Owolabi
//Course: CSCE 3530
//Dijsktra Algorithm Implementation
#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#define INFINITY 9999
#define MAXROUTER 10

char vertex[6] = {'u', 'v', 'w', 'x', 'y', 'z'};	//global array for vertex variable names

void dijsktra(int v, int cost[6][6], FILE *output1);	//
void minPathByDijkstra(int costmat[MAXROUTER][MAXROUTER],int n,int source, FILE * output);

int main()
{
	FILE *input, *output1;
	char ch;
	int i, j, cost;
	char sV1[2], sV2[2], V1, V2;	//input characters

	int costmat[MAXROUTER][MAXROUTER],aa,cc,n,u;
	char c1, c2, t1, t2;
	aa=0;
	//6 Verticies - u,v,w,x,y,z = 0,1,2,3,4,5
	int graph[6][6];

	for(i=0; i<6; i++)	//initialize the graph with 999 in place of infinity
	{
		for(j=0; j<6; j++)
		{
			graph[i][j] = 999;
		}
	}

	input = (fopen("router.txt", "r"));	//open input file
	if( input == NULL )
   	{
      		perror("Error while opening the file.\n");
      		exit(EXIT_FAILURE);
   	}
	printf("\n<<router.txt input values>>\n");
        while( fscanf(input, "%1s %1s %d", sV1, sV2, &cost) != EOF )	//input the values for the graph matrix byscanning from te input file
        {
		V1 = sV1[0];	//transpose the strings to characters
		V2 = sV2[0];
		printf("%c %c %d\n", V1, V2, cost);	//print input graph
                switch(V1)	//switch for first node input
                {
                        case 'u':
                                i = 0;
                                break;
                        case 'v':
                                i = 1;
                                break;
                        case 'w':
                                i = 2;
                                break;
                        case 'x':
                                i = 3;
                                break;
                        case 'y':
                                i = 4;
                                break;
                        case 'z':
                                i = 5;
                                break;
                }
                switch(V2)	//switch for second node input
                {
                        case 'u':
                                j = 0;
                                break;
                        case 'v':
                                j = 1;
                                break;
                        case 'w':
                                j = 2;
                                break;
                        case 'x':
                                j = 3;
                                break;
                        case 'y':
                                j = 4;
                                break;
                        case 'z':
                                j = 5;
                                break;
                }
                graph[i][j] = cost;	//input the cost into the appropriate places in the graph for each node.
		graph[j][i] = cost;
        }

	printf("\nThe following output is in the format:\n");
	printf("\tVertex V from N (N, Distance from N to V(value))\n");
	output1 = (fopen("LS.txt", "a+"));	//print output to file LS.txt

	fprintf(output1, "\nPart I: Dijsktra's Algorithm\n");
	printf("\nPart I: Dijsktra's Algorithm\n");
	fprintf(output1, "\nThis part of the algorithm will show the actual value of the router that will give the least cost\n");
	printf("\nThis part of the algorithm will show the actual value of the router that will give the least cost\n");
		//run algorithms and print results
	for(i=0; i<6; i++){
                dijsktra(i, graph, output1);
  }
	n=6;
	for(aa=0;aa<n;aa++)
	for(cc=0;cc<n;cc++)

	costmat[aa][cc]=0;
	while(fscanf(input, "%c%c%c%c%d\nRouter", &c1, &t1, &c2, &t2, &u)!=-1){
			costmat[c1-'u'][c2-'u']=u;
			costmat[c2-'u'][c1-'u']=u;
	}

	fprintf(output1, "\nPart II: Dijsktra's Algorithm\n");
	printf("\nPart II: Dijsktra's Algorithm\n");
	fprintf(output1, "\nThis part of the algorithm will show the actual router that will give the least cost\n");
	printf("\nThis part of the algorithm will show the actual router that will give the least cost\n");
	for(aa=0;aa<n;aa++)
	minPathByDijkstra(costmat,n,aa,output1);

	fclose(output1);
	return 0;
}

void dijsktra(int v, int cost[6][6], FILE *output1){
	int i, j, u, count, w, visit[6], min;
	int distVal[6];	//distance array, to hold the distance values for each node from v
 	for(i=0; i<6; i++)	//initialize the distance array
 	{
		if(i == v)
		{
			visit[i] = 111;	//vertex v is already set to visited
		}
		else
		{
			visit[i] = 0;	//initialize the visited array
		}
		distVal[i] = cost[v][i];	//pull the known costs for the initial node v into the distance array.
 	}
	count=1;
 	while(count < 6)	//Dijkstra's algorithm
 	{
 		min=99; //initial minimum

  		for(w=0; w<6; w++)	//find the minimum distance for each node
		{
   			if((distVal[w] < min) && (!visit[w])){
    				min = distVal[w];
						u = w;
  			}
		}
		visit[u] = count;	//set the path number
		count++;		//increment path number
  		for(w=0; w<6; w++)	//calculate the other paths thatare related or open up due to the selection of the miniumum node distance
   		{
			if((distVal[u] + cost[u][w] < distVal[w]) && (!visit[w]))
    			{
				distVal[w] = distVal[u] + cost[u][w];	//set the new minimum distance for the node
			}
 		}
	}
	count = 1;
	while(count < 6)	//print the output for the node v
	{
		for(i=0; i<6; i++)
        	{
                	if((visit[i] == count) && (i != v))
                	{
				printf(" %c (%c, %d)\n", vertex[i], vertex[v], distVal[i]);
				fprintf(output1, " %c (%c, %d)\n", vertex[i], vertex[v], distVal[i]);
				count++;
                	}
        	}
	}
	printf("--------\n");
	fprintf(output1, "--------\n");
}
void minPathByDijkstra(int costmat[MAXROUTER][MAXROUTER],int n,int source, FILE * output1){
   int m=0;
   int tp1, tp2;
   int cost[MAXROUTER][MAXROUTER],costAry[MAXROUTER],prev[MAXROUTER];
   int selected[MAXROUTER],count,min,nxtRouter,i,d;
   //Loop to copy the input cost matrix
   for(i=0;i<n;i++)
       for(d=0;d<n;d++)
           if(costmat[i][d]==0)
               cost[i][d]=INFINITY;
           else
               cost[i][d]=costmat[i][d];
   ///Loop to initialize the predecessor array , visited nodes, omne dimensional cost array
   for(i=0;i<n;i++){
       costAry[i]=cost[source][i];
       prev[i]=source;
       selected[i]=0;
   }
   //Set the cost for the starting node
   costAry[source]=0;
   //Set value
   selected[source]=1;
   //Set the count
   count=1;
   //Loop
   while(count<n-1){
       //Set the values
       min=INFINITY;
       //Find the router which has the minimum cost
       for(i=0;i<n;i++)
       //Check condition
       if(costAry[i]<min&&!selected[i]){
           //Update the cost
           min=costAry[i];
           //Update the next router in the path
           nxtRouter=i;
       }
       //Set value
       selected[nxtRouter]=1;
       //Loop.
       for(i=0;i<n;i++)
           //Check condition.
           if(!selected[i])
               //Check condition.
               if(min+cost[nxtRouter][i]<costAry[i])
               {
                   //Find the cost.
                   costAry[i]=min+cost[nxtRouter][i];
                   //Set the predecessor.
                   prev[i]=nxtRouter;
               }
       count++;
   }
   for(i=0;i<n;i++)

   if(i!=source){
		 //Set values.
       m=0;
       d=i;
       tp1=d;
			 //Loop to get the path.
       do
      {
				   tp2=tp1;
           d=prev[d];
           tp1=d;
       }while(d!=source);
 		 //Print output to console.
       printf("\n %c (%c, %c)", (char) (i+'u'), (char)(tp1+'u'), (char) (tp2+'u'));
			 //Print output to file.
       fprintf(output1,"\n %c (%c, %c)", (char) (i+'u'), (char)(tp1+'u'), (char) (tp2+'u'));
   }
   fprintf(output1,"\n --------------");

   printf("\n --------------");
}
