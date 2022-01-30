#include <stdio.h>
#include <stdlib.h>

int isHeap_recursive(int arr[], int i, int n) {

    if(i >= n || n <= 0) {
        
        return 1;
    }

    if((2 * i + 1) < n) {
        
        if(arr[2 * i + 1] < arr[i]) {
            
            return 0;
        }
    }

    if((2 * i + 2) < n) {
        
        if(arr[2 * i + 2] < arr[i]) {
            
            return 0;
        }
    }

    return (isHeap_recursive(arr, 2 * i + 1, n) && isHeap_recursive(arr, 2 * i + 2, n));
}

int isHeap_iter(int arr[], int n) {

    for(int i = 0; i <= (n-2)/2; i++) {
        
        if(arr[2 * i +1 ] < arr[i]  && arr[2 * i + 2] < arr[i]) {
            
            return 0;
        }
    }

    return 1;
}

int main(void) {
    
    int amount = 0;

    printf("How many entries? ");
    scanf("%d", &amount);

    int* arr = (int*)malloc(sizeof(int) * amount);

    printf("Enter %d entries: \n", amount);

    for(int i = 0; i < amount; i++) {
        
        scanf("%d", &arr[i]);
    }

    isHeap_recursive(arr, 0, amount) ? printf("Recursive says Yes!\n") : printf("Recursive says No!\n");
    isHeap_iter(arr, amount) ? printf("Iterative says Yes!\n") : printf("Iterative says No!\n");
}

