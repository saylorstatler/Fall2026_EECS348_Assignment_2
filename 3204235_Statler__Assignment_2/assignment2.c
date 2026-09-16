/*
 * EECS 348 Assignment 2
 * Program: CEO Email Priority Queue
 * Description: Uses a list-based MaxHeap to prioritize CEO emails
 * Inputs: EMAIL, NEXT, READ, COUNT commands
 * Outputs: Next email and unread email count
 * Collaborators: None
 * Other Sources: Gemini
 * Author: Saylor Statler
 * Creation Date: September 15, 2026
 * Revision Date: September 16, 2026
 * Revisions: Used Gemini's generated code as a starting point and modified to meet requirements.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 100

// Stores one email.
typedef struct {
    char category[32];
    char subject[256];
    char date[16];
    int priority;
    long dateValue;
} Email;

// Stores emails in a list-based MaxHeap.
typedef struct {
    Email *data;
    int size;
    int capacity;
} MaxHeap;

// Gives each sender category a priority.
int getPriority(char *category) {
    if (strcmp(category, "Boss") == 0) return 5;
    if (strcmp(category, "Subordinate") == 0) return 4;
    if (strcmp(category, "Peer") == 0) return 3;
    if (strcmp(category, "ImportantPerson") == 0) return 2;
    return 1;
}

// Converts a date to a number for comparison.
long getDateValue(char *date) {
    int month, day, year;
    sscanf(date, "%d-%d-%d", &month, &day, &year);
    return year * 10000L + month * 100 + day;
}

// Returns 1 when email a has higher priority than email b.
int higher(Email *a, Email *b) {
    if (a->priority != b->priority)
        return a->priority > b->priority;

    return a->dateValue > b->dateValue;
}

// Swaps two emails.
void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

// Moves an email up the heap.
void heapUp(MaxHeap *heap, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;

        if (!higher(&heap->data[i], &heap->data[parent]))
            break;

        swap(&heap->data[i], &heap->data[parent]);
        i = parent;
    }
}

// Moves an email down the heap.
void heapDown(MaxHeap *heap, int i) {
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;

        if (left < heap->size &&
            higher(&heap->data[left], &heap->data[largest]))
            largest = left;

        if (right < heap->size &&
            higher(&heap->data[right], &heap->data[largest]))
            largest = right;

        if (largest == i)
            break;

        swap(&heap->data[i], &heap->data[largest]);
        i = largest;
    }
}

// Adds an email to the MaxHeap.
void addEmail(MaxHeap *heap, char *category, char *subject, char *date) {
    // Increase capacity when the list is full.
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->data = realloc(heap->data,
                             heap->capacity * sizeof(Email));
    }

    Email *email = &heap->data[heap->size];

    strcpy(email->category, category);
    strcpy(email->subject, subject);
    strcpy(email->date, date);
    email->priority = getPriority(category);
    email->dateValue = getDateValue(date);

    heap->size++;
    heapUp(heap, heap->size - 1);
}

// Removes the highest-priority email.
void readEmail(MaxHeap *heap) {
    if (heap->size == 0)
        return;

    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;

    if (heap->size > 0)
        heapDown(heap, 0);
}

int main() {
    MaxHeap heap;
    heap.size = 0;
    heap.capacity = INITIAL_CAPACITY;
    heap.data = malloc(heap.capacity * sizeof(Email));

    char line[512];

    // Read commands until the end of the input file.
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = '\0';

        // Add an email to the heap.
        if (strncmp(line, "EMAIL ", 6) == 0) {
            char *category = line + 6;
            char *comma1 = strchr(category, ',');
            char *subject;
            char *comma2;
            char *date;

            if (!comma1)
                continue;

            *comma1 = '\0';
            subject = comma1 + 1;

            comma2 = strchr(subject, ',');
            if (!comma2)
                continue;

            *comma2 = '\0';
            date = comma2 + 1;

            addEmail(&heap, category, subject, date);
        }

        // Display the highest-priority email.
        else if (strcmp(line, "NEXT") == 0) {
            if (heap.size > 0) {
                Email *email = &heap.data[0];

                printf("Next email:\n");
                printf("Sender: %s\n", email->category);
                printf("Subject: %s\n", email->subject);
                printf("Date: %s\n", email->date);
            }
        }

        // Remove the highest-priority email.
        else if (strcmp(line, "READ") == 0) {
            readEmail(&heap);
        }

        // Display the number of unread emails.
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap.size);
        }
    }

    // Free the dynamically allocated heap.
    free(heap.data);

    return 0;
}
