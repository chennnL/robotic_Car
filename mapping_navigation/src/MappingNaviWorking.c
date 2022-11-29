#include <stdio.h>
#include <stdlib.h>
#include "PWM.h"    // placeholder for PWM functions

// Compass Index Constants for _wallStatus, _routeVisited
#define NORTHIDX 0
#define SOUTHIDX 1
#define EASTIDX 2
#define WESTIDX 3
// Compass Bearing Constants
#define NORTHANGLE 0
#define EASTANGLE 90
#define SOUTHANGLE 180
#define WESTANGLE 270
// Boolean Value Constants
#define TRUE 1
#define FALSE 0
// Graph and Weight Definition
#define NUMOFVERTICES 20
#define WEIGHT 27

// Coordinates Class Definition
typedef struct Coordinates
{
    int _xCoords;                               // x coords, each unit represents 27cm in grid
    int _yCoords;                               // y coords, each unit represents 27cm in grid
} Coordinates;

// Vertex Class Definition (Adjacency Vertex), multiple instances can exist in Adjacency list
typedef struct Vertex
{
    int _vertexID;                              // adjacency vertex unique identifier
    struct Vertex *next;                        // pointer to next adjacency vertex in adjacency list
} Vertex;

// VertexProperty Class Definition, only one unique instance can exist in a Vertex Property List
typedef struct VertexProperty
{
    int _vertexID;                              // vertex property unique identifier
    struct Coordinates vertexCoords;            // coordinates of the vertex within the maze
    int isVisited;                              // to indicate if all vertex routes has been explored (Y - 1, N - 0)
    int _wallStatus[4];                         // to indicate status of walls around a vertex in an NSEW orientation (Blocked - 1, Open - 0)
    int _routeVisited[4];                       // to indicate status of routes exploration around a vertex in an NSEW orientation (Explored/Blocked - 1, Unexplored - 0)
} VertexProperty;

// Graph Class Definition
typedef struct Graph
{
    int _numOfVertices;                         // number of vertices in a graph
    struct VertexProperty **vertexPropertyList; // an list of vertex properties
    struct Vertex **adjLists;                   // an adjacency vertex list
} Graph;

/*  Function to print x and y coordinates   */
void printCoordinates(Coordinates *pCoords)
{
    printf("\nx=%d, y=%d",pCoords->_xCoords, pCoords->_yCoords);
}

/*  Function to initialise an origin coordinate (0,0) instance  
    Allocates memory for Coordinate instance, Sets x and y coords to 0  */
Coordinates* initialiseOriginCoordinate()
{
    Coordinates* newCoord = malloc(sizeof(Coordinates));
    newCoord->_xCoords = 0;
    newCoord->_yCoords = 0;
    return newCoord;
}   

/*  Function to intialise a Vertex instance for the Adjacency List 
    Allocates memory for Vertex instance, set VertexID, set next pointer to null */
Vertex *createVertex(int vertexID)
{
    Vertex *newVertex = malloc(sizeof(Vertex)); 
    newVertex->_vertexID = vertexID;                           
    newVertex->next = NULL;                
    return newVertex;
}

/*  Function to intialise a VertexProperty instance for the Vertex List 
    Allocates memory for VertexProperty instance and initialises
    > _vertexID as specified
    > isVisited as 0
    > vertexCoords to 999 (to simulate infinity)
    > _wallStatus and _routeVisited (all elements) to 0 */
VertexProperty *createVertexProperty(int vertexID)
{
    VertexProperty *newVertexProperty = malloc(sizeof(VertexProperty));
    newVertexProperty->_vertexID = vertexID;
    newVertexProperty->isVisited = 0;
    newVertexProperty->vertexCoords._xCoords = 999;
    newVertexProperty->vertexCoords._yCoords = 999;
    for (int i = 0; i < 4; i++)
    {
        newVertexProperty->_wallStatus[i] = 0;
        newVertexProperty->_routeVisited[i] = 0;
    }
    return newVertexProperty;
}

/*  Function to intialise a Graph instance for the Car Mapping Algorithm
    Allocates memory for Graph instance and initalises
    > _numOfVertices as specified
    > adjLists (adjacency list) as null and allocates memory for it
    > vertexPropertyList, populate it with 20 Vertex Property instances and allocates memory for it */
Graph *createGraph(int numOfVertices)
{
    Graph *graph = malloc(sizeof(Graph));
    graph->_numOfVertices = numOfVertices;
    graph->adjLists = malloc(numOfVertices * sizeof(Vertex *)); 
    graph->vertexPropertyList = malloc(numOfVertices * sizeof(VertexProperty *));
    for (int i = 0; i < numOfVertices; i++)
    {
        graph->adjLists[i] = NULL;
        graph->vertexPropertyList[i] = createVertexProperty(i);
    }
    return graph;
}

/*  Function add bidirectional edges of two vertices into an Adjacency List
    Creates a Vertex Instance for Vertex B and adds it as an edge for Vertex A in adjacency list
    Creates a Vertex Instnace for Vertex A and adds it as an edge for Vertex B in adjacency list
    > New vertex, set next of adjacency list to previous head for Vertex [X] to ensure links remain */
void addEdge(Graph *graph, int srcVertex, int dstVertex)
{
    // Add edge from source to destination
    struct Vertex *newVertex = createVertex(dstVertex);
    newVertex->next = graph->adjLists[srcVertex]; 
    graph->adjLists[srcVertex] = newVertex;

    // Add edge from destination to source
    newVertex = createVertex(srcVertex);
    newVertex->next = graph->adjLists[dstVertex];
    graph->adjLists[dstVertex] = newVertex;
}

/*  Function to print out the content of the adjacency list
    Traverses through each Vertex within adjacency list
    Prints contents of LinkedList for each Vertex using the .next pointer for reference */
void printAdjacencyList(Graph *graph)
{
    printf("\nAdjacency List of each Vertex:\n");
    for (int currVert = 0; currVert < graph->_numOfVertices; currVert++)
    {
        Vertex *tempVert = graph->adjLists[currVert];
        // prints all adjacent vertexes of current vertex stored in current element of adjList
        printf("Adjacent Vertices of Vertex %d: ", currVert);
        while (tempVert)
        {
            printf("%d ", tempVert->_vertexID);
            tempVert = tempVert->next;
        }
        printf("\n");
    }
}

/* Function print all contents of each Vertex Property in Vertex Property List */
void printVertexProperties(Graph *graph)
{
    for (int currVert = 0; currVert < graph->_numOfVertices; currVert++)
    {
        VertexProperty *tempVertProperty = graph->vertexPropertyList[currVert];
        printf("VertexID: %d\n", tempVertProperty->_vertexID);
        printf("Coordinates (%d, %d)\n",tempVertProperty->vertexCoords._xCoords, tempVertProperty->vertexCoords._yCoords);
        printf("IsVisited: %d\n", tempVertProperty->isVisited);
        printf("WallStatus: %d %d %d %d\n", tempVertProperty->_wallStatus[0], tempVertProperty->_wallStatus[1], tempVertProperty->_wallStatus[2], tempVertProperty->_wallStatus[3]);
        printf("RouteVisited: %d %d %d %d\n\n", tempVertProperty->_routeVisited[0], tempVertProperty->_routeVisited[1], tempVertProperty->_routeVisited[2], tempVertProperty->_routeVisited[3]);
    }
}

/*  Function to update routeVisited based on the bearings of the new route travelled at vertex
    Updates directly to pRouteVisited by pointer    */
void updateRouteVisited(int* pRouteVisited, int pCarBearing)
{
    switch (pCarBearing)
    {
        // if route is visited is north
        case (NORTHANGLE):
            pRouteVisited[0] = 1;
            break;
        // if route is visited is south
        case (SOUTHANGLE):
            pRouteVisited[1] = 1;
            break;
        // if route is visited is east
        case (EASTANGLE):
            pRouteVisited[2] = 1;
            break;
        // if route is visited is west
        case (WESTANGLE):
            pRouteVisited[3] = 1;
            break;
    }
}

/*  Function directly updates isVisited based on its address
    Checks entire routeVisited, if any value is 0. If it is returns isVisited as 0, if not return isVisited as 1    */
void updateIsVisited(int* pRouteVisited, int* pIsVisited)
{
    for(int i = 0; i < 4; i++)
    {
        // if any route is not visited, it assigns isVisited as 0 and exits
        if(pRouteVisited[i] == 0)
        {
            pIsVisited = 0;
            return;
        }
    }
    // all routes in routeVisited is visited, assigns isVisited as 1 and exits
    *pIsVisited = 1;
}

/*  Function checks if the maze has been fully explored
    Traverses through every Vertex Property in Vertex Property List and checks if all isVisited are 1 (fully explored)
    > If any isVisited is 0, return 0
    > If all isVisited is 1, return 1   */
int isMazeFullyVisited(Graph* graph)
{
    for(int i = 0; i < NUMOFVERTICES; i++)
    {
        if(graph->vertexPropertyList[i]->isVisited == FALSE)
        {
            return 0;
        }
    }
    return 1;
}

/*  Function to check if coordinates exist in the Vertex Property List
    Traverses through Vertex Property List
    > If coordinates exist, return 1
    > If coordinates does not exist, return 0   */
int existCoordsInVertexPropertyList(Coordinates* pCoordsToCheck, VertexProperty** pVertexList)
{
    for(int i = 0; i < NUMOFVERTICES; i++)
    {
        if(pCoordsToCheck->_xCoords == pVertexList[i]->vertexCoords._xCoords && pCoordsToCheck->_yCoords == pVertexList[i]->vertexCoords._yCoords)
            return 1;
    }
    return 0;
}

/*  Function to return the vertexID of a coordinate within the vertex property list
    Prerequisite is that coordinate has to exists within Vertex Property List   */
int getVertexIDWithCoords(Coordinates* pCoords, VertexProperty** pVertexList)
{
    if(existCoordsInVertexPropertyList(pCoords, pVertexList)==TRUE)
    {
        for (int i = 0; i < NUMOFVERTICES; i++)
        {
            if (pCoords->_xCoords == pVertexList[i]->vertexCoords._xCoords &&
            pCoords->_yCoords == pVertexList[i]->vertexCoords._yCoords)
                return pVertexList[i]->_vertexID;
        }
    }
    else
    {
        printf("\nError! Coordinate does not exists within the vertex list\n");
    }
}

/*  Function to update the new coordinates of the car when moving from one vertex to another
    Coordinates of the car (either x or y) should increase by one unit, based on the compass bearing it is travelling in    */
void getNewVertexCoordinates(Coordinates* pPrevCoords, int pTravelledBearings)
{
    switch (pTravelledBearings)
    {
        // if car is moving north, add 1 to y coords
        case(NORTHANGLE):
            pPrevCoords->_yCoords += 1;
            break;
        // if car is moving south, subtract 1 from y coords
        case(SOUTHANGLE):
            pPrevCoords->_yCoords -= 1;
            break;
        // if car is moving east, add 1 to x coords
        case(EASTANGLE):
            pPrevCoords->_xCoords += 1;
            break;
        // if car is moving south, subtract 1 from x coords
        case(WESTANGLE):
            pPrevCoords->_xCoords -= 1;
            break;
    }
}

/*  Function to get the VertexID of an Vertex that has not been fully explored  
    Traverses through Vertex Property List and checks if isVisited is 0, return VertexID if true   */
int getUnexploredVertexID(VertexProperty** pVertexList)
{
    for(int vertIdx = 0; vertIdx < NUMOFVERTICES; vertIdx++)
    {
        if(pVertexList[vertIdx]->isVisited == 0)
            return vertIdx;
    }
    printf("\nError! All the vertices have been visited, unable to return VertexID\n");
}

/*  Function to set coordinates within a Vertex Property
    Considers base case for point of origin (0,0)
    For other cases where vertexID is not 0, we assign vertex property coordinate to vertex property    */
void setCoordsAtVertexProperty(Coordinates* pCurrCoords, VertexProperty* pCurrVertexProperty)
{
    // base case for point of origin
    if(pCurrVertexProperty->_vertexID == 0)
    {
        pCurrVertexProperty->vertexCoords._xCoords = 0;
        pCurrVertexProperty->vertexCoords._yCoords = 0;        
    }
    // other cases where vertexID is anything but 0
    else
    {
        pCurrVertexProperty->vertexCoords._xCoords = pCurrCoords->_xCoords;
        pCurrVertexProperty->vertexCoords._yCoords = pCurrCoords->_yCoords;
    }
}

/*  Function to set a Vertex Property's wallStatus and routeVisited using mock data to make up for lack of integration with Ultrasonic
    Value assignment done one vertex at a time
    Should take in number of vertices mapped so far to assign corresponding values in mock data */
void setWallStatusRouteVisitedWithMockData(int *pWallStatus, int *pRouteVisited, int pNumOfVerticesMapped)
{
    // mock wall status (NSEW) based on the way car traverses through maze
    int mockWallStatus[20][4] =
    {
        {1, 0, 0, 1},   // 00
        {1, 0, 0, 0},   // 01
        {1, 0, 0, 0},   // 02
        {1, 1, 0, 0},   // 03
        {1, 0, 1, 0},   // 04
        {0, 0, 1, 1},   // 05
        {0, 0, 1, 0},   // 06
        {1, 1, 0, 0},   // 07
        {1, 1, 0, 0},   // 08
        {0, 1, 0, 1},   // 09
        {0, 0, 1, 1},   // 10
        {0, 0, 1, 1},   // 11
        {0, 0, 1, 1},   // 12
        {0, 1, 0, 1},   // 13
        {1, 1, 0, 0},   // 14
        {1, 1, 0, 0},   // 15
        {1, 1, 0, 0},   // 16
        {0, 1, 1, 0},   // 17
        {0, 1, 0, 1},   // 18
        {1, 1, 1, 0},   // 19
    };
    // assign corresponding wall status
    pWallStatus[NORTHIDX] = mockWallStatus[pNumOfVerticesMapped][0];
    pWallStatus[SOUTHIDX] = mockWallStatus[pNumOfVerticesMapped][1];
    pWallStatus[EASTIDX] = mockWallStatus[pNumOfVerticesMapped][2];
    pWallStatus[WESTIDX] = mockWallStatus[pNumOfVerticesMapped][3];
    // mirror route visited
    pRouteVisited[NORTHIDX] = mockWallStatus[pNumOfVerticesMapped][0];
    pRouteVisited[SOUTHIDX] = mockWallStatus[pNumOfVerticesMapped][1];
    pRouteVisited[EASTIDX] = mockWallStatus[pNumOfVerticesMapped][2];
    pRouteVisited[WESTIDX] = mockWallStatus[pNumOfVerticesMapped][3];
}

/*  Function to get bearings of where the car is coming from, based on the opposite of the cars current bearings
    E.g. If node 0, car travelled in East direction, node 1 acknowledge that car is from West
    Used by the next node to mark that the opposite direction has be visited since the car came from that direction
    Executed at node 0; return direction needed for node 1  */
int getCarOppositeBearings(int pCarBearings)
{
    switch(pCarBearings)
    {
        case(NORTHANGLE):
            return SOUTHANGLE;
        case(SOUTHANGLE):
            return NORTHANGLE;
        case(EASTANGLE):
            return WESTANGLE;
        case(WESTANGLE):
            return EASTANGLE;
    }
}

/*  Function to get bearings of an unexplored route within a Vertex
    Returns by the traversal priorities of car which is N->E->W->S
*/
int getUnexploredBearings(int* pRouteVisited, int pCompassBearing)
{
    // Split routeVisited into "boolean" variables north, south, east, west for less array accesses
    int northFlag = pRouteVisited[0]; 
    int southFlag = pRouteVisited[1];
    int eastFlag = pRouteVisited[2];
    int westFlag = pRouteVisited[3];

    // if north is open
    if (northFlag == 0)
    {   
        return NORTHANGLE;
    }
    // if north == 1 (blocked)
    else
    {
        // if east is open
        if (eastFlag == 0)
        {  
            return EASTANGLE;
        }
        // if east blocked, check if west is open
        else if (westFlag == 0)
        {      
            return WESTANGLE;
        }
        // if east and west are blocked, check if south is open
        else if(southFlag == 0)
        {      
            return SOUTHANGLE;
        }
        // Error message
        else
        {
            printf("Error! Vertex is fully visited, function only handles routeVisited that is not fully visited");
        }
    }
}

/*  Function to orientate car based on the car's current bearing and our car's desired bearing 
    Orientation will be done using PWM turning functions
    Type of turning function and number of turns is dependent on difference between current and desired bearings
    If bearing difference is negative, car has to turn right, if bearing difference is positive, car has to turn left, if bearing is 180 or -180 turn twice left/right  */
void orientateCar(int carCompassBearing, int desiredCompassBearing)
{
    int bearingDifference = carCompassBearing - desiredCompassBearing;
    // check how much is needed to turn based on bearing difference   
    switch(bearingDifference)
    {
        // bearing difference of -180 degrees
        case(-180):
            // call PWM function to turn right twice
            
            printf("\nCar turn right twice");
            // Motor_Turn_Right();
            // Motor_Turn_Right();
            break;
        case(-90):
            // call PWM function to turn right once
            printf("\nCar turns right");
            // Motor_Turn_Right();
            break;
        case(180):
            // call PWM function to turn left twice
            printf("\nCar turn left twice");
            // Motor_Turn_Left();
            // Motor_Turn_Left();
            break;
        case(90):
            // call PWM function to turn left once
            printf("\nCar turn left");
            // Motor_Turn_Left();
            break;
    }
}

/*  Function to get the car to navigate to a destination location using its
    Shortest vertex path, vertex list, current coordinate and the car's compass bearing */
void navigateToDestination(int* pShortestPathToDest, int pPathSize, VertexProperty** pVertexList, Coordinates* pCurrCoords, int* pCurrCarBearings)
{
    printf("\nBeginning navigation...");
    int nextPathVertexID = 999;
    // iterate through shortest path
    for(int i = 0; i < pPathSize; i++)
    {
        nextPathVertexID = pShortestPathToDest[i];
        // iterate through vertex list
        for(int j = 0; j < NUMOFVERTICES; j++) 
        {
            
            // match found between next path vertexID and vertexID stored in vertex list
            if(nextPathVertexID == pVertexList[j]->_vertexID)
            {
                // get difference between current vertex coordinates and the next vertex path coordinates
                int xCoordDiff = pCurrCoords->_xCoords - pVertexList[j]->vertexCoords._xCoords;
                int yCoordDiff = pCurrCoords->_yCoords - pVertexList[j]->vertexCoords._yCoords;

                    // orientate car
                    // adjust bearings
                    // move car forward
                    // adjust coords


                // no difference in x coords and no difference in y coords, most applicable for source vertex
                if(xCoordDiff == 0 && yCoordDiff == 0)
                {
                    printf("\nSource vertex caught, moving on to next vertex");
                    break;
                }
                // no difference in x coords, difference in y coords
                else if(xCoordDiff == 0)
                {
                    // next vertex is in south direction
                    if(yCoordDiff > 0)
                    {
                        printf("\nCar turning south");
                        orientateCar(*pCurrCarBearings, SOUTHANGLE);
                        *pCurrCarBearings = SOUTHANGLE;
                        printf("\nCar moves forward");
                        // Motor_Drive_Forward();
                        getNewVertexCoordinates(pCurrCoords, SOUTHANGLE);
                        printCoordinates(pCurrCoords);
                        break;
                    }
                    // next vertex is in north direction
                    else
                    {
                        printf("\nCar turning north");
                        orientateCar(*pCurrCarBearings, NORTHANGLE);
                        *pCurrCarBearings = NORTHANGLE;
                        printf("\nCar moves forward");
                        // Motor_Drive_Forward();
                        getNewVertexCoordinates(pCurrCoords, NORTHANGLE);
                        printCoordinates(pCurrCoords);
                        break;
                    }
                }
                // no difference in y coords, difference in x coords
                else if(yCoordDiff == 0)
                {
                    // next vertex is in east direction
                    if(yCoordDiff > 0)
                    {
                        printf("\nCar turning east");
                        orientateCar(*pCurrCarBearings, EASTANGLE);
                        *pCurrCarBearings = EASTANGLE;
                        printf("\nCar moves forward");
                        // Motor_Drive_Forward();
                        getNewVertexCoordinates(pCurrCoords, EASTANGLE);
                        printCoordinates(pCurrCoords);
                        break;
                    }
                    // next vertex is in west direction
                    else
                    {
                        printf("\nCar turning west");
                        orientateCar(*pCurrCarBearings, WESTANGLE);
                        *pCurrCarBearings = WESTANGLE;
                        printf("\nCar moves forward");
                        // Motor_Drive_Forward();
                        getNewVertexCoordinates(pCurrCoords, WESTANGLE);
                        printCoordinates(pCurrCoords);
                        break;
                    }
                }
                // difference in both x and y coords, not supposed to happen
                else
                {
                    printf("\nError! Difference in both x and y coordinates between current and next coordinates \n");
                    return;
                }
            }
        }
    }
    printf("\nNavigation is complete. Resume mapping");
}

/* Using: Basic Dijkstra to server as the working version                             */
/* Input from mapping: Graph, Vs, Vt, shortestPathSrcToDist                           */
/* Determine distances of shortest paths from source_vertex to all vertices           */
/* Output to mapping: Array of shortest path from source to target and path its size. */
/* It is a O(V^2) function                                                            */
int dijkstraWorking(struct Graph *graph, int source_vertex, int target_vertex, int *path_mapping)
{
    int distTo[NUMOFVERTICES]; // distTo values used to pick shortest distance found so far
    int edgeTo[NUMOFVERTICES]; // edgeTo store predecessor as part of shortest path
    int visited[NUMOFVERTICES];
    int minDistance;
    int count;
    int u; // current vertex

    // Initialize distTo, edgeTo, and visited
    for (int v = 0; v < NUMOFVERTICES; ++v) // O(V): iterate all vertices
    {
        distTo[v] = INT_MAX;    
        edgeTo[v] = -1;         
        visited[v] = 0;
    }

    distTo[source_vertex] = 0;  
    count = 0;

    while (count < NUMOFVERTICES) // O(V): iterate all vertices
    {
        minDistance = INT_MAX;
        
        for (int i = 0; i < NUMOFVERTICES; ++i) // O(V): iterate all vertices
        {
        /* check if shortest known dist i is lesser than minimum distance we are tracking and if i is visited already */
            if (distTo[i] < minDistance && !visited[i])     
            {
                minDistance = distTo[i];
                u = i;  // u gives the vertex at minimum distTo
            }
        }
        // mark u (current vertex) as visited
        visited[u] = 1;

        /* Traverse through all adjacent        */
        /* vertices of u (the extracted vertex) */ 
        /* and update their distance values     */            
        struct Vertex *currVert = malloc(sizeof(struct Vertex));
        currVert = graph->adjLists[u];

        while (currVert != NULL) // O(E): iterate neigbour
        {
            int v = currVert->_vertexID;

            // If shortest distance to v is
            // not finalized yet, and distance to v
            // through u is less than its
            // previously calculated distance
            if (distTo[u] != INT_MAX &&
                distTo[u] + WEIGHT < distTo[v])
            {
                distTo[v] = distTo[u] + WEIGHT;
                edgeTo[v] = u;
            }
            currVert = currVert->next;
        }
        count++;
        free(currVert);
    }
    // print out all the edgeTo[] aka best predecessor adj (neighbor) path found
    // for (int i = 1; i < NUMOFVERTICES; ++i)
    //     printf("%d - %d\n", edgeTo[i], i);

    // print source_vertex, target, and shortest path
    printf("\n\nDijkstra Calculating...");
    printf("\nSource Vertex: %d", source_vertex);
    printf("\nTarget Vertex: %d", target_vertex);
    printf("\nPath= ");

    int path[NUMOFVERTICES];
    path[0] = target_vertex;
    int i = 1;
    int vertex = target_vertex;

    do
    {
        vertex = edgeTo[vertex];    // find the best predecessor adj (neighbor) path found for current "vertex"
        path[i] = vertex;           // append it in the path found to be passed to mapping's ShortestPathSrcToDist array
        i++;

    } while (vertex != source_vertex);
    int x = 0;
    for (int k = i - 1; k > -1; k--) // reverse from target_vertex-->source, to source-->target_vertex
    {
        printf("%d->", path[k]); 
        path_mapping[x] = path[k];
        x++;
    }

    printf("\nNumber of vertices in path array sending to mapping: %d\n", i); // return number of vertices in path array to mapping
    return i;
}

/*  Car Mapping Algorithm
    Local variables of the algorithm
    - current car coordinate -> currCarCoord
    - current vertex property being edited -> currVertexProperty
    - current vertex index -> currVertexIdx
    - previous vertex index -> prevVertexIdx
    - a counter to track number of vertices mapped so far
    - flag to indicate if car travelled to a new vertex but has not updated route visited for this vertex
*/
void carMappingAlgorithm(Graph* graph, int pCarBearings)
{
    // local variables used in routing algorithm
    Coordinates* currCarCoord = initialiseOriginCoordinate();
    VertexProperty* currVertexProperty = NULL;
    int currCarBearings = pCarBearings;
    int incomingRouteFlag = FALSE;
    int numOfVerticesMapped = 0;
    int prevVertexIdx = 0;
    int currVertexIdx = 0;
    // hardcoded shit for dijkstra temporary replacement
    int* firstNavPath = malloc(sizeof(int)*2);
    firstNavPath[0] = 1;
    firstNavPath[1] = 0;
    int* secondNavPath = malloc(sizeof(int)*5);
    secondNavPath[0] = 6;
    secondNavPath[1] = 5;
    secondNavPath[2] = 4;
    secondNavPath[3] = 3;
    secondNavPath[4] = 2;
    int navCounter = 0; 

    printf("\nBeginning car mapping...");
    // main car routing algortihm
    while(1)
    {
        printCoordinates(currCarCoord);
        currVertexProperty = graph->vertexPropertyList[currVertexIdx];          
        printf("\n\nDoes current vertex exist in the Vertex List: %d ", existCoordsInVertexPropertyList(currCarCoord, graph->vertexPropertyList));
        /****************************************************
          If coordinate does not exist in vertex property list
            > Need to assign coordinate to current vertex -> setCoordsAtVertexProperty()
            > Need to initialise wall status and mirror wall status to routes visited -> initialiseWallStatusRouteVisited()/setWallStatusRouteVisitedWithMockData()
            > Checks if incoming route flag is set to true (if car travelled to a new vertex but has not updated routeVisited for this vertex)
                > If true get opposite routing bearings (direction car came from) -> getCarOppositeBearings()
                > Add this bearings to routeVisited of this current vertex property -> updateRouteVisted()
                > Check and update if all routes has been visited with isVisited -> updateIsVisited()
            > Need to increase counter for number of vertices mapped so far
            > If current vertex is not origin vertex 0, add edges
        *****************************************************/
        if(existCoordsInVertexPropertyList(currCarCoord, graph->vertexPropertyList)==0)
        {
            setCoordsAtVertexProperty(currCarCoord, currVertexProperty);                                                            
            setWallStatusRouteVisitedWithMockData(currVertexProperty->_wallStatus, currVertexProperty->_routeVisited, numOfVerticesMapped);
            // initialiseWallStatusRouteVisited(currVertexProperty->_wallStatus, currVertexProperty->_routeVisited, currCarBearings);
            // if car previously arrived to a new vertex in the last iteration
            if(incomingRouteFlag==TRUE)
            {
                int oppoBearing = getCarOppositeBearings(currCarBearings);
                updateRouteVisited(currVertexProperty->_routeVisited, oppoBearing);
                updateIsVisited(currVertexProperty->_routeVisited, &currVertexProperty->isVisited);
                incomingRouteFlag = FALSE;
            }
            printf("\nCreated vertex for vertex %d",numOfVerticesMapped);
            numOfVerticesMapped++;
            if(currVertexIdx != 0)
            {
                addEdge(graph, prevVertexIdx, currVertexIdx);   
            }                                   
        }
        /****************************************************
          If vertex is not fully visited
            > Get bearings of a route that has not been explored in vertex in order of NEWS 
            > Orientate the car to face bearings of unexplored route -> orientateCar()
            > Add the route that is about to be travelled to routeVisited of current vertex property -> updateRouteVisited()
            > Check and update if all routes has been visited with isVisited -> updateIsVisited()
            > Move the car forward by one unit move -> Motor_Drive_Forward()
            > Get new vertex coordinate -> getNewVertexCoordinates()
            > Get vertex id of coordinate within vertex list if it exists
                > If it doesn't exists in vertex list
                    > Set [incomingRouteFlag] to true
                    > Set prevVertexID to currVertexID, currVertexID is set to numofmappedvertixes
                    > Continue onto next iteration of while loop
                > If it exists, 
                    > Set old vertexID as prevVertexID
                    > Get new vertexID -> getVertexIDWithCoords()
                    > Set new vertexID as currtexID
            > Change vertex property to this new vertex id
            > Get the route bearings of direction car travelled from (opposite of current bearings) to update in routeVisited of current new vertex
            > Add the route that was travelled to routeVisited of new vertex property -> updateRouteVisted()
            > Check and update if all routes has been visited with isVisited -> updateIsVisited()
            > Add edges to the adjacency list using prevVertIdx and currVertIdx
        *****************************************************/
        if(currVertexProperty->isVisited==0)
        {
            int newCarBearings = getUnexploredBearings(currVertexProperty->_routeVisited, currCarBearings);
            orientateCar(currCarBearings, newCarBearings);
            printf("\nCar faces %d degrees",newCarBearings);
            currCarBearings = newCarBearings;
            updateRouteVisited(currVertexProperty->_routeVisited, currCarBearings);
            updateIsVisited(currVertexProperty->_routeVisited, &currVertexProperty->isVisited); 
            printf("\nCar moves forward");
            //Motor_Drive_Forward();
            getNewVertexCoordinates(currCarCoord, currCarBearings);
        
            // if coordinates doesn't exist in vertex property list
            if(existCoordsInVertexPropertyList(currCarCoord, graph->vertexPropertyList)==0)
            {
                incomingRouteFlag=TRUE;
                prevVertexIdx = currVertexIdx;
                currVertexIdx = numOfVerticesMapped;
                continue;
            }
            
            // if coordinate exists in vertex property list
            prevVertexIdx = currVertexIdx;
            currVertexIdx = getVertexIDWithCoords(currCarCoord,graph->vertexPropertyList);
            currVertexProperty = graph->vertexPropertyList[currVertexIdx]; 
            int oppoBearing = getCarOppositeBearings(currCarBearings);
            updateRouteVisited(currVertexProperty->_routeVisited, oppoBearing);
            updateIsVisited(currVertexProperty->_routeVisited, &currVertexProperty->isVisited);
            addEdge(graph, prevVertexIdx, currVertexIdx); 
        }
        /****************************************************
            If vertex is fully visited (loop / dead end)
            > Need 
            > Need to find the vertex id of the vertex that has not been fully visited within vertex list
            > Dynamically allocate an integer array "shortPathToDest" with 100 elements to store vertex paths 
            > Get shortest path to destination node from Dijkstra function by return by pointer
            > Get the path size as well from the Dijkstra function
            > Navigate to the destination vertex
            > Is there really a need to add edges here? Probably not right? Because you're skipping multiple vertexes?
        *****************************************************/
        else
        {
            /****************************************************
                Break clause for the car algorithm
                > Within each iteration check through the vertex property list to see if all vertexes has been fully explored
                    > If yes, break out of while loop
                    > If no, just continue
            *****************************************************/
            if(isMazeFullyVisited(graph)==1)
            {
                printf("Maze is fully mapped, car mapping algorithm will now end!\n");
                break;
            }
            int unexploredVertexID = getUnexploredVertexID(graph->vertexPropertyList);
            prevVertexIdx = currVertexIdx; 
            currVertexIdx = unexploredVertexID;
            int* shortPathToDest = malloc((sizeof(int))*100);
            int pathSize = dijkstraWorking(graph, prevVertexIdx, unexploredVertexID, shortPathToDest);
            navigateToDestination(shortPathToDest, pathSize, graph->vertexPropertyList, currCarCoord, &currCarBearings);
            free(shortPathToDest);
            continue;

        }
    }
}

int main()
{
    // Initialise data structures and variables
    Graph *graph = createGraph(20);
    // run car mapping algorithm
    carMappingAlgorithm(graph,NORTHANGLE);
    // printing out results like vertex list and adjacency list
    printVertexProperties(graph);
    printAdjacencyList(graph);
    return 0;
}
