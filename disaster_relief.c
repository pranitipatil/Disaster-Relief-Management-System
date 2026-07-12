
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define MAX 10 // maximum number of relief camps
/* STRUCTURES */
// Relief request node for Priority Queue
struct Request
{
char campName[30];
int severity; // higher = more critical
int food;
int water;
struct Request *next;
};
// Camp information stored in BST
struct Camp
{
char name[30];
int node; // location node (for graph)
struct Camp *left, *right;
};
// Delivery record for Stack
struct Delivery
{
char campName[30];
int food, water;
struct Delivery *next;
};
/* PRIORITY QUEUE */
// Insert a new relief request based on severity
struct Request* enqueue(struct Request *front, char name[], int severity, int
food, int water)
{
struct Request *newReq = (struct Request *)malloc(sizeof(struct
Request));
strcpy(newReq->campName, name);
newReq->severity = severity;
newReq->food = food;
newReq->water = water;
newReq->next = NULL;
if (front == NULL || severity > front->severity)
{
newReq->next = front;
front = newReq;
}
else
{
struct Request *temp = front;
while (temp->next != NULL && temp->next->severity >= severity)
temp = temp->next;
newReq->next = temp->next;
temp->next = newReq;
}
printf("Request from '%s' added (Severity %d)\n", name, severity);
return front;
}
// Remove and return highest priority request
struct Request* dequeue(struct Request **front)
{
if (*front == NULL) return NULL;
struct Request *temp = *front;
*front = (*front)->next;
return temp;
}
/* BST (for Camps) */
struct Camp* insertCamp(struct Camp *root, char name[], int node)
{
if (root == NULL)
{
struct Camp *newCamp = (struct Camp *)malloc(sizeof(struct Camp));
strcpy(newCamp->name, name);
newCamp->node = node;
newCamp->left = newCamp->right = NULL;
printf("Camp '%s' added at node %d\n", name, node);
return newCamp;
}
if (strcmp(name, root->name) < 0)
root->left = insertCamp(root->left, name, node);
else if (strcmp(name, root->name) > 0)
root->right = insertCamp(root->right, name, node);
else
printf("Camp already exists!\n");
return root;
}
struct Camp* searchCamp(struct Camp *root, char name[])
{
if (root == NULL) return NULL;
if (strcmp(name, root->name) == 0)
return root;
else if (strcmp(name, root->name) < 0)
return searchCamp(root->left, name);
else
return searchCamp(root->right, name);
}
/* STACK (Delivery Log) */
struct Delivery* pushDelivery(struct Delivery *top, char name[], int food, int
water)
{
struct Delivery *newD = (struct Delivery *)malloc(sizeof(struct Delivery));
strcpy(newD->campName, name);
newD->food = food;
newD->water = water;
newD->next = top;
top = newD;
return top;
}
void showRecentDeliveries(struct Delivery *top)
{
if (top == NULL)
{
printf("No deliveries yet.\n");
return;
}
printf("\nRecent Deliveries:\n");
struct Delivery *temp = top;
int count = 1;
while (temp != NULL)
{
printf("%d. %s -> Food:%d Water:%d\n", count++, temp->campName,
temp->food, temp->water);
temp = temp->next;
}
}
/* GRAPH (Shortest Path - Dijkstra) */
int minDistance(int dist[], int sptSet[], int numLocations)
{
int min = INT_MAX, min_index = -1;
for (int v = 0; v < numLocations; v++)
if (!sptSet[v] && dist[v] <= min)
min = dist[v], min_index = v;
return min_index;
}
void dijkstra(int graph[MAX][MAX], int numLocations, int src, int dest)
{
int dist[MAX];
int sptSet[MAX];
int parent[MAX];
for (int i = 0; i < numLocations; i++)
{
dist[i] = INT_MAX;
sptSet[i] = 0;
parent[i] = -1;
}
dist[src] = 0;
for (int count = 0; count < numLocations - 1; count++)
{
int u = minDistance(dist, sptSet, numLocations);
if (u == -1) break;
sptSet[u] = 1;
for (int v = 0; v < numLocations; v++)
{
if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
&& dist[u] + graph[u][v] < dist[v])
{
dist[v] = dist[u] + graph[u][v];
parent[v] = u;
}
}
}
printf("Shortest route from Base(%d) to Camp(%d): ", src, dest);
int path[MAX], idx = 0;
for (int v = dest; v != -1; v = parent[v])
path[idx++] = v;
for (int i = idx - 1; i >= 0; i--)
{
printf("%d", path[i]);
if (i > 0)
printf(" -> ");
}
printf(" (Distance = %d)\n", dist[dest]);
}
/* DISPATCHING TRUCK */
void dispatchTruck(struct Request **front, struct Camp *root, struct Delivery
**top,
int graph[MAX][MAX], int numLocations)
{
struct Request *req = dequeue(front);
if (req == NULL)
{
printf("No pending requests.\n");
return;
}
struct Camp *camp = searchCamp(root, req->campName);
if (camp == NULL)
{
printf("Camp not found in records!\n");
free(req);
return;
}
printf("\nDispatching Truck to '%s'...\n", req->campName);
dijkstra(graph, numLocations, 0, camp->node);
printf("Delivery completed to %s (Food:%d Water:%d)\n",
req->campName, req->food, req->water);
*top = pushDelivery(*top, req->campName, req->food, req->water);
free(req);
}
/* MAIN MENU */
int main()
{
struct Request *front = NULL;
struct Camp *root = NULL;
struct Delivery *top = NULL;
int graph[MAX][MAX];
int numLocations;
int choice;
printf("Enter total number of camps (max %d): \n(1 base camp is included
in this) ", MAX);
fflush(stdout);
scanf("%d", &numLocations);
printf("Note: Node 0 is configured as the base camp by default.\n");
printf("\nEnter adjacency matrix for routes (0 if no direct road):\n");
for (int i = 0; i < numLocations; i++)
{
for (int j = 0; j < numLocations; j++)
{
fflush(stdout);
scanf("%d", &graph[i][j]);
}
}
getchar(); // Consume newline after matrix input before menu
do {
printf("\n-------------------------------\n");
printf("Disaster Relief Menu\n");
printf("1. Add Relief Camp\n");
printf("2. Submit Relief Request\n");
printf("3. Dispatch Next Truck\n");
printf("4. Show Recent Deliveries\n");
printf("5. Search Camp Info\n");
printf("6. Exit\n");
printf("Enter your choice: ");
fflush(stdout);
scanf("%d", &choice);
getchar();
char name[30];
int node, sev, food, water;
switch (choice)
{
case 1:
printf("Enter Camp Name: ");
fflush(stdout);
fgets(name, sizeof(name), stdin);
name[strcspn(name, "\n")] = 0; // Remove newline
printf("Enter Node Number (1-%d): ", numLocations - 1);
fflush(stdout);
scanf("%d", &node);
getchar();
root = insertCamp(root, name, node);
break;
case 2:
printf("Enter Camp Name: ");
fflush(stdout);
fgets(name, sizeof(name), stdin);
name[strcspn(name, "\n")] = 0;
printf("Enter Severity (1-Low, 2-Medium, 3-High): ");
fflush(stdout);
scanf("%d", &sev);
printf("Enter Food (kg): ");
fflush(stdout);
scanf("%d", &food);
printf("Enter Water (liters): ");
fflush(stdout);
scanf("%d", &water);
getchar();
front = enqueue(front, name, sev, food, water);
break;
case 3:
dispatchTruck(&front, root, &top, graph, numLocations);
break;
case 4:
showRecentDeliveries(top);
break;
case 5:
printf("Enter Camp Name: ");
fflush(stdout);
fgets(name, sizeof(name), stdin);
name[strcspn(name, "\n")] = 0;
struct Camp *c = searchCamp(root, name);
if (c)
printf("Found Camp '%s' at Node %d\n", c->name, c->node);
else
printf("Camp not found!\n");
break;
case 6:
printf("Exiting system...\n");
break;
default:
printf("Invalid choice!\n");
}
} while (choice != 6);
return 0;
}
