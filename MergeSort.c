#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MAX_ELEMENTS 1000

//struct to pass parameters to sorting function
typedef struct {
    int *arr;
    int start;
    int end;
} sortType;

//to merge two sorted halves
void merge(int arr[], int start, int mid, int end) {
    
    // arrays 'left' and 'right' to store the two halves
    // Parameters: 'start' is the start index, 'mid' is the middle index, and 'end' is the end index
    // The merged array is stored back in 'arr'
    // This is a helper function used in the mergeSort function.
    int i, j, k;
    int n1 = mid - start + 1;
    int n2 = end - mid;

    int left[n1], right[n2];

    for (i = 0; i < n1; i++)
        left[i] = arr[start + i];
    for (j = 0; j < n2; j++)
        right[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = start;

    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = left[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = right[j];
        j++;
        k++;
    }
}

//recursive merge sort function //also runner function for threads
void *mergeSort(void *arguments) {
    sortType *sp = (sortType *)arguments;
    int start = sp->start;
    int end = sp->end;
    int *arr = sp->arr;

    if (start < end) {
        int mid = start + (end - start) / 2;

        //threads for sorting each half
        pthread_t leftThread, rightThread;

        sortType leftParams = {arr, start, mid};
        sortType rightParams = {arr, mid + 1, end};

        pthread_create(&leftThread, NULL, mergeSort, &leftParams); //calling with the appropriate arguments
        pthread_create(&rightThread, NULL, mergeSort, &rightParams);

        // Wait for threads to finish
        pthread_join(leftThread, NULL);
        pthread_join(rightThread, NULL);

        merge(arr, start, mid, end);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("format: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *f= fopen(argv[1], "r");

    //FILE *f = fopen("input-sort.txt", "r");

    if (f == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    int no_elements;
    fscanf(f, "%d", &no_elements);

    if (no_elements <= 0 || no_elements > MAX_ELEMENTS) {
        printf("Invalid number of elements.\n");
        fclose(f);
        return 1;
    }

    int *arr = (int *)malloc(no_elements * sizeof(int));

    for (int i = 0; i < no_elements; i++) {
        fscanf(f, "%d", &arr[i]);
    }

    fclose(f);

    //initial sorting
    sortType initialParams = {arr, 0, no_elements - 1};

    // Initialize pthread
    pthread_t initialThread;
    pthread_create(&initialThread, NULL, mergeSort, &initialParams);

    //wait for the initial thread to finish
    pthread_join(initialThread, NULL);

    //printing sorted array
    for (int i = 0; i < no_elements; i++) {
        printf("%d ", arr[i]);
    }

    //free allocated memory
    free(arr);

    return 0;
}
