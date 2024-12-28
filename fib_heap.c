#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
// Node structure for Fibonacci Heap
typedef struct FibNode
{
    int key;                // Key value of the node
    int degree;             // Degree of the node (number of children)
    struct FibNode *parent; // Pointer to the parent node
    struct FibNode *child;  // Pointer to the first child node
    struct FibNode *left;   // Pointer to the left sibling node
    struct FibNode *right;  // Pointer to the right sibling node
    int mark;               // Indicates if the node has lost a child (used in the consolidation process)
} FibNode;
// Fibonacci Heap structure
typedef struct FibHeap
{
    FibNode *min; // Pointer to the minimum node in the heap
    int n;        // Number of nodes in the heap
} FibHeap;
// Create a new Fibonacci Heap
FibHeap *createHeap()
{
    FibHeap *heap = (FibHeap *)malloc(sizeof(FibHeap)); // Allocate memory for the heap
    heap->min = NULL;                                   // Initialize the min pointer to NULL (empty heap)
    heap->n = 0;                                        // Set the number of nodes to 0
    return heap;                                        // Return the created heap
}
// Create a new node with a given key
FibNode *createNode(int key)
{
    FibNode *node = (FibNode *)malloc(sizeof(FibNode)); // Allocate memory for the new node
    node->key = key;                                    // Set the key of the node
    node->degree = 0;                                   // Initialize the degree to 0 (no children yet)
    node->parent = NULL;                                // No parent at the moment
    node->child = NULL;                                 // No children yet
    node->left = node;                                  // Left pointer points to itself (circular list)
    node->right = node;                                 // Right pointer points to itself (circular list)
    node->mark = 0;                                     // Mark is initially 0 (no lost children)
    return node;                                        // Return the created node
}
// Insert a node into the Fibonacci Heap
void insertNode(FibHeap *heap, FibNode *node)
{
    if (!heap->min)
    {
        heap->min = node; // If the heap is empty, make this node the minimum
    }
    else
    {
        // Add node to the root list of the heap
        node->left = heap->min;         // Set the left pointer to the current minimum node
        node->right = heap->min->right; // Set the right pointer to the right of the current minimum
        heap->min->right->left = node;  // Update the left pointer of the node to the right of the minimum
        heap->min->right = node;        // Update the right pointer of the minimum to the new node
                                        // If the new node's key is smaller than the current minimum, update the minimum pointer
        if (node->key < heap->min->key)
        {
            heap->min = node;
        }
    }
    heap->n++; // Increment the number of nodes in the heap
}
// Union of two Fibonacci Heaps
FibHeap *unionHeaps(FibHeap *h1, FibHeap *h2)

{
    if (!h1->min) // If the first heap is empty, return the second heap
        return h2;
    if (!h2->min) // If the second heap is empty, return the first heap
        return h1;
    // Merge the root lists of both heaps
    FibNode *h1Min = h1->min;
    FibNode *h2Min = h2->min;
    h1Min->right->left = h2Min->left; // Update the right and left pointers of the root lists
    h2Min->left->right = h1Min->right;
    h1Min->right = h2Min; // Make the second heap's minimum node the rightmost node of the first heap
    h2Min->left = h1Min;  // Make the first heap's minimum node the leftmost node of the second heap
                          // Update the minimum pointer if necessary
    if (h2->min->key < h1->min->key)
    {
        h1->min = h2->min;
    }
    h1->n += h2->n; // Update the number of nodes in the first heap
    // free(h2); // Free the second heap
    // h2=NULL;
    return h1; // Return the merged heap
}
// Find the minimum node in the Fibonacci Heap
FibNode *findMin(FibHeap *heap)
{
    return heap->min; // Return the minimum node from the heap
}
// Helper function to print the root list (for debugging)
void printHeap(FibHeap *heap)
{
    if (!heap->min) // If the heap is empty, print a message
    {
        printf("Heap is empty.\n");
        return;
    }
    // Print all nodes in the root list
    FibNode *current = heap->min;
    do
    {
        printf("%d ", current->key); // Print the key of the current node
        current = current->right;    // Move to the next node in the root list
    } while (current != heap->min); // Stop when we have looped back to the first node
    printf("\n");
}
// Display the Fibonacci heap
void display(FibNode *node)
{
    if (node == NULL)
        return;
    FibNode *start = node;
    do
    {
        printf("%d ", node->key);
        if (node->child != NULL)
        {
            printf("(Child: ");
            display(node->child);
            printf(") ");
        }
        node = node->right;
    } while (node != start);
}
// Link two Fibonacci heap nodes
void linkNodes(FibNode *y, FibNode *x)
{
    // Remove y from root list
    y->left->right = y->right;
    y->right->left = y->left;
    // Make y a child of x
    y->parent = x;
    y->left = y;
    y->right = y;
    if (x->child == NULL)
    {
        x->child = y;
    }
    else
    {
        y->right = x->child->right;
        y->left = x->child;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
    y->mark = 0;
}
// to consolidate heap on degree to achieve unique degree elements
void consolidate(FibHeap *heap)
{
    int maxDegree = ((int)(log(heap->n) / log(2))) + 1;
    FibNode *degreeTable[maxDegree];
    for (int i = 0; i < maxDegree; i++)
    {
        degreeTable[i] = NULL;
    }
    FibNode *current = heap->min;
    FibNode *start = heap->min;
    if (current == NULL)
        return;
    int degree;
    do
    {
        FibNode *x = current;
        current = current->right;
        degree = x->degree;
        while (degreeTable[degree] != NULL)
        {
            FibNode *y = degreeTable[degree];
            if (y->key != x->key)
            {
                if (y->key < x->key)
                {
                    FibNode *temp = x;
                    x = y;
                    y = temp;
                }
                linkNodes(y, x);
                degreeTable[degree] = NULL;
            }
            degree++;
            if (degree == maxDegree)
                break;
        }
        if (degree != maxDegree)
            degreeTable[degree] = x;
    } while (current != start && degree != maxDegree);
    // Reconstruct the root list and find the new minimum
    heap->min = NULL;
    for (int i = 0; i < maxDegree; i++)
    {
        if (degreeTable[i] != NULL)
        {
            if (heap->min == NULL)
            {
                heap->min = degreeTable[i];
                heap->min->left = heap->min;
                heap->min->right = heap->min;
            }
            else
            {
                degreeTable[i]->left = heap->min;
                degreeTable[i]->right = heap->min->right;
                heap->min->right->left = degreeTable[i];
                heap->min->right = degreeTable[i];
                if (degreeTable[i]->key < heap->min->key)
                {
                    heap->min = degreeTable[i];
                }
            }
        }
    }
}
// to delete minimum node
FibNode *
extractMin(FibHeap *heap)
{
    FibNode *minNode = heap->min;
    if (minNode != NULL)
    {
        if (minNode->child != NULL)
        {
            FibNode *child = minNode->child;
            do
            {
                FibNode *nextChild = child->right;
                // Add child to the root list
                child->left = heap->min;
                child->right = heap->min->right;
                heap->min->right->left = child;
                heap->min->right = child;
                child->parent = NULL;
                child = nextChild;
            } while (child != minNode->child);
        }
        // Remove minNode from root list
        minNode->left->right = minNode->right;
        minNode->right->left = minNode->left;
        if (minNode == minNode->right)
        {
            heap->min = NULL;
        }
        else
        {
            heap->min = minNode->right;
            consolidate(heap);
        }
        heap->n--;
    }
    return minNode;
}
// Menu-driven interface for user interaction
void menu()
{
    printf("\nFibonacci Heap Operations:\n");
    printf("1. Insert Node\n");
    printf("2. Display Heap\n");
    printf("3. Find Minimum\n");
    printf("4. Union of Two Heaps\n");
    printf("5: Extract Min\n");
    printf("6. Exit\n");
}
int main()
{
    FibHeap *heap = createHeap();  // Create the first Fibonacci heap
    FibHeap *heap2 = createHeap(); // Create the second Fibonacci heap
    int choice, key;
    while (1)
    {
        menu(); // Display the menu
        printf("Enter your choice: ");
        scanf("%d", &choice); // Get the user's choice
        switch (choice)
        {
        case 1:
            // Insert a node
            printf("Enter key to insert: ");
            scanf("%d", &key);                 // Get the key for the new node
            insertNode(heap, createNode(key)); // Insert the node into the first heap
            printf("Node with key %d inserted.\n", key);
            break;
        case 2:
            // Display the heap
            printf("Current heap: ");
            // printHeap(heap); // Print the current heap
            if (heap->min != NULL)
            {
                display(heap->min);
            }
            else
            {
                printf("Heap is empty.");
            }
            printf("\n");
            break;
        case 3:
            // Find the minimum node
            if (heap->min) // If the heap is not empty
            {
                printf("Minimum node in the heap: %d\n", heap->min->key); // Print the minimum node
            }
            else
            {
                printf("Heap is empty.\n"); // If the heap is empty, print a message
            }
            break;
        case 4:
            // Union of two heaps
            printf("Insert nodes for the second heap:\n");
            printf("How many keys in the second heap:\n");
            int k;
            scanf("%d", &k);
            for (int j = 1; j <= k; j++)
            {
                printf("Enter key to insert into second heap: ");
                scanf("%d", &key); // Get the key for the new node in the second heap
                if (!heap2)
                    heap2 = createHeap();
                insertNode(heap2, createNode(key)); // Insert the node into the second heap
            }
            // printf("Enter key to insert into second heap: ");
            // scanf("%d", &key); // Get the key for the new node in the second heap
            // if (!heap2)
            //  heap2 = createHeap();
            // insertNode(heap2, createNode(key)); // Insert the node into the second heap
            printf("Second heap after insertion: ");
            printHeap(heap2); // Print the second heap
            // Perform the union of the two heaps
            heap = unionHeaps(heap, heap2); // Merge the two heaps
            printf("Heap after union: ");
            printHeap(heap); // Print the merged heap
            // free(heap2);
            heap2 = NULL;
            break;
        case 5:
            if (heap->min != NULL)
            {
                FibNode *minNode = extractMin(heap);
                printf("Extracted minimum key: %d\n", minNode->key);
                free(minNode);
            }
            else
            {
                printf("Heap is empty.\n");
            }
            break;
        case 6:
            // Exit the program
            printf("Exiting program.\n");
            exit(0); // Exit the program
        default:
            printf("Invalid choice, please try again.\n"); // If the user enters an invalid option
        }
    }
    return 0; // End of the program
}