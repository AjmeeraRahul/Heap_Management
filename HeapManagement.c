#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEAP_SIZE 1024    

struct heap_Node {
    int node_size;
    char variable_name[100];
    char status[20];
    int heap_start_address;
    int heap_end_address;
    struct heap_Node* next;
    struct heap_Node* prev;
};

struct free_Node {
    int free_size;
    int free_start_address;
    int free_end_address;
    struct free_Node* next;
    struct free_Node* prev;
};

typedef struct heap_Node heap_node;
typedef struct free_Node free_node;

free_node* initilise_heap(free_node* free_lptr,int size) {
    free_node* new_node = (free_node*)malloc(sizeof(free_node));
    new_node->free_size = size;
    new_node->free_start_address = 0;
    new_node->free_end_address = size - 1;
    if(free_lptr==NULL){
    new_node->next = NULL;
    new_node->prev = NULL;
    }
    else{
        new_node->next=free_lptr;
        new_node->prev=NULL;
    }
    free_lptr = new_node;
    return free_lptr;
}

heap_node* sort_heap_list(heap_node* heap_lptr) {
    heap_node* ifptr = heap_lptr;
    while(ifptr != NULL) {
        heap_node* jfptr = heap_lptr;  
        while(jfptr != NULL && jfptr->next != NULL) {
            if((jfptr->heap_start_address) > (jfptr->next->heap_start_address)) {
                int temp_end, temp_start, temp_size;
                char temp_name[100], temp_status[20];  

                temp_end = jfptr->heap_end_address;
                temp_start = jfptr->heap_start_address;
                temp_size = jfptr->node_size;
                strcpy(temp_name, jfptr->variable_name);
                strcpy(temp_status, jfptr->status);    
                
                jfptr->heap_start_address = jfptr->next->heap_start_address;
                jfptr->heap_end_address = jfptr->next->heap_end_address;
                jfptr->node_size = jfptr->next->node_size;
                strcpy(jfptr->variable_name, jfptr->next->variable_name);
                strcpy(jfptr->status, jfptr->next->status); 

                jfptr->next->heap_start_address = temp_start;
                jfptr->next->heap_end_address = temp_end;
                jfptr->next->node_size = temp_size;
                strcpy(jfptr->next->variable_name, temp_name);
                strcpy(jfptr->next->status, temp_status);    
            }
            jfptr = jfptr->next;
        }
        ifptr = ifptr->next;
    }
    return heap_lptr;
}

free_node* sort_free_list(free_node* free_lptr) {
    free_node* ifptr = free_lptr;
    while(ifptr != NULL) {
        free_node* jfptr = free_lptr;  
        while(jfptr != NULL && jfptr->next != NULL) {
            if((jfptr->free_start_address) > (jfptr->next->free_start_address)) {
                int temp_end, temp_start, temp_size;

                temp_end = jfptr->free_end_address;
                temp_start = jfptr->free_start_address;
                temp_size = jfptr->free_size;
                
                jfptr->free_start_address = jfptr->next->free_start_address;
                jfptr->free_end_address = jfptr->next->free_end_address;
                jfptr->free_size = jfptr->next->free_size;

                jfptr->next->free_start_address = temp_start;
                jfptr->next->free_end_address = temp_end;
                jfptr->next->free_size = temp_size;
            }
            jfptr = jfptr->next;
        }
        ifptr = ifptr->next;
    }
    return free_lptr;
}

heap_node* insert_in_heap_with_first_allocation(free_node** free_ptr, heap_node* heap_lptr, int size, char name[100]) {
    if (size <= 0 || size > MAX_HEAP_SIZE) {  // Added size validation
        printf("\nInvalid size requested\n");
        return heap_lptr;
    }

    free_node* free_head = *free_ptr;
    free_node* free_lptr = free_head;
    int insert = 0;
    
    while(insert == 0 && free_lptr != NULL) {
        if(free_lptr->free_size == size) {
            heap_node* new_node = (heap_node*)malloc(sizeof(heap_node));
            if (new_node == NULL) {    // Added NULL check for malloc
                printf("\nMemory allocation failed\n");
                return heap_lptr;
            }
            
            new_node->node_size = size;
            strcpy(new_node->variable_name, name);
            strcpy(new_node->status, "allocated");

            new_node->heap_start_address = free_lptr->free_start_address;
            new_node->heap_end_address = new_node->heap_start_address + (size - 1);

            new_node->prev = NULL;
            new_node->next = heap_lptr;
            
            if(heap_lptr != NULL) {
                heap_lptr->prev = new_node;
            }
            heap_lptr = new_node;

            if(free_lptr != *free_ptr) {
                free_node* prev = free_lptr->prev;
                free_node* next = free_lptr->next;
                prev->next = next;
                if(next != NULL) {
                    next->prev = prev;
                }
            } else {
                *free_ptr = free_lptr->next;
                if(*free_ptr != NULL) {
                    (*free_ptr)->prev = NULL;
                }
            }

            free(free_lptr);
            insert = 1;
        }
        else if(free_lptr->free_size > size) {
            heap_node* new_node = (heap_node*)malloc(sizeof(heap_node));
            if (new_node == NULL) {    // Added NULL check for malloc
                printf("\nMemory allocation failed\n");
                return heap_lptr;
            }
            
            new_node->node_size = size;
            strcpy(new_node->variable_name, name);
            strcpy(new_node->status, "allocated");

            new_node->heap_start_address = free_lptr->free_start_address;
            new_node->heap_end_address = new_node->heap_start_address + (size - 1);

            free_lptr->free_start_address = new_node->heap_end_address + 1;
            free_lptr->free_size = free_lptr->free_size - size;
            free_lptr->free_end_address = free_lptr->free_start_address + (free_lptr->free_size - 1);

            new_node->next = heap_lptr;
            new_node->prev = NULL;
            if(heap_lptr != NULL) {
                heap_lptr->prev = new_node;
            }
            heap_lptr = new_node;

            insert = 1;
        }
        else {
            free_lptr = free_lptr->next;
        }
    }

    if(!insert) {
        printf("\nHeap is out of memory and cannot be inserted\n");
    } else {
        printf("Memory is allocated\n");
    }
    
    *free_ptr = free_head;
    return sort_heap_list(heap_lptr);
}

heap_node* insert_in_heap_with_best_fit_allocation(free_node** free_ptr, heap_node* heap_lptr, int size, char name[100]) {
    if (size <= 0 || size > MAX_HEAP_SIZE) {  
        printf("\nInvalid size requested\n");
        return heap_lptr;
    }

    free_node* free_head = *free_ptr;
    free_node* free_lptr = free_head;
    free_node* min_fptr = NULL;
    int insert = 0, min_size = MAX_HEAP_SIZE + 1;  

    while(free_lptr != NULL) {
        if(free_lptr->free_size == size) {
            min_fptr = free_lptr;
            min_size = size;
            break;
        }
        else if(free_lptr->free_size > size && free_lptr->free_size < min_size) {
            min_size = free_lptr->free_size;
            min_fptr = free_lptr;
        }
        free_lptr = free_lptr->next;
    }

    if(min_fptr != NULL) {
        heap_node* new_node = (heap_node*)malloc(sizeof(heap_node));
        if (new_node == NULL) {    
            printf("\nMemory allocation failed\n");
            return heap_lptr;
        }

        new_node->node_size = size;
        strcpy(new_node->variable_name, name);
        strcpy(new_node->status, "allocated");

        new_node->heap_start_address = min_fptr->free_start_address;
        new_node->heap_end_address = new_node->heap_start_address + (size - 1);

        if(min_size == size) {
            if(min_fptr != *free_ptr) {
                free_node* prev = min_fptr->prev;
                free_node* next = min_fptr->next;
                prev->next = next;
                if(next != NULL) {
                    next->prev = prev;
                }
            } else {
                *free_ptr = min_fptr->next;
                if(*free_ptr != NULL) {
                    (*free_ptr)->prev = NULL;
                }
            }
            free(min_fptr);
        } else {
            min_fptr->free_start_address = new_node->heap_end_address + 1;
            min_fptr->free_size = min_fptr->free_size - size;
            min_fptr->free_end_address = min_fptr->free_start_address + (min_fptr->free_size - 1);
        }

        new_node->prev = NULL;
        new_node->next = heap_lptr;
        if(heap_lptr != NULL) {
            heap_lptr->prev = new_node;
        }
        heap_lptr = new_node;
        printf("Memory is allocated\n");
    } else {
        printf("\nHeap is out of memory and cannot be inserted\n");
    }

    return sort_heap_list(heap_lptr);
}

free_node* combine_consecutive_free_blocks(free_node* free_lptr) {
    if (free_lptr == NULL) return NULL;  // Added NULL check
    
    free_node* fptr = free_lptr;
    while(fptr != NULL && fptr->next != NULL) {
        if(fptr->next->free_start_address == fptr->free_end_address + 1) {
            free_node* dfptr = fptr->next;
            fptr->free_end_address = dfptr->free_end_address;
            fptr->free_size = fptr->free_size + dfptr->free_size;
            
            fptr->next = dfptr->next;
            if(dfptr->next != NULL) {
                dfptr->next->prev = fptr;
            }
            
            free(dfptr);
        } else {
            fptr = fptr->next;
        }
    }
    return free_lptr;
}

heap_node* deallocate(heap_node* heap_lptr, free_node** free_lptr, char to_deallocate[100]) {
    heap_node* hptr = heap_lptr;
    int done = 0;

    while(done == 0 && hptr != NULL) {
        if(strcmp(hptr->variable_name, to_deallocate) == 0) {
            free_node* new_node = (free_node*)malloc(sizeof(free_node));
            if (new_node == NULL) {    
                printf("\nMemory allocation failed during deallocation\n");
                return heap_lptr;
            }

            new_node->free_size = hptr->node_size;
            new_node->free_start_address = hptr->heap_start_address;
            new_node->free_end_address = hptr->heap_end_address;
            new_node->prev = NULL;
            new_node->next = *free_lptr;
            if(*free_lptr != NULL) {
                (*free_lptr)->prev = new_node;
            }
            *free_lptr = new_node;

            if(hptr != heap_lptr) {
                heap_node* prev = hptr->prev;
                heap_node* next = hptr->next;
                prev->next = next;
                if(next != NULL) {
                    next->prev = prev;
                }
            } else {
                heap_lptr = hptr->next;
                if(heap_lptr != NULL) {
                    heap_lptr->prev = NULL;
                }
            }

            free(hptr);
            done = 1;
        } else {
            hptr = hptr->next;
        }
    }

    if(done == 0) {
        printf("Variable not found in heap\n");
    } else {
        printf("Variable is deallocated successfully\n");
        *free_lptr = sort_free_list(*free_lptr);
        *free_lptr = combine_consecutive_free_blocks(*free_lptr);
    }

    return heap_lptr;
}

void display_heap(heap_node* heap_lptr, free_node* free_lptr) {
    heap_node* hptr = heap_lptr;
    free_node* fptr = free_lptr;

    printf("\n%-15s %-15s %-15s %-15s %-15s\n", 
           "Variable Name", "Size", "Status", "Start Address", "End Address");
    printf("----------------------------------------------------------------\n");
    
    while(hptr != NULL || fptr != NULL) {
        if(hptr != NULL && fptr != NULL) {
            if(hptr->heap_start_address < fptr->free_start_address) {
                printf("%-15s %-15d %-15s %-15d %-15d\n",
                       hptr->variable_name, hptr->node_size, hptr->status,
                       hptr->heap_start_address, hptr->heap_end_address);
                hptr = hptr->next;
            } else {
                       printf("%-15s %-15d %-15s %-15d %-15d\n",
                        "free", fptr->free_size, "free",
                        fptr->free_start_address, fptr->free_end_address);
                 fptr = fptr->next;
             }
         } else if(hptr != NULL) {
             printf("%-15s %-15d %-15s %-15d %-15d\n",
                    hptr->variable_name, hptr->node_size, hptr->status,
                    hptr->heap_start_address, hptr->heap_end_address);
             hptr = hptr->next;
         } else if(fptr != NULL) {
             printf("%-15s %-15d %-15s %-15d %-15d\n",
                    "free", fptr->free_size, "free",
                    fptr->free_start_address, fptr->free_end_address);
             fptr = fptr->next;
         }
     }
     printf("----------------------------------------------------------------\n");
 }
 
 void display_allocated_blocks(heap_node* heap_lptr) {
     heap_node* hptr = heap_lptr;
     
     printf("\n%-15s %-15s %-15s %-15s %-15s\n", 
            "Variable Name", "Size", "Status", "Start Address", "End Address");
     printf("----------------------------------------------------------------\n");
     
     if(hptr == NULL) {
         printf("No blocks are allocated\n");
     }
     
     while(hptr != NULL) {
         printf("%-15s %-15d %-15s %-15d %-15d\n",
                hptr->variable_name, hptr->node_size, hptr->status,
                hptr->heap_start_address, hptr->heap_end_address);
         hptr = hptr->next;
     }
     printf("----------------------------------------------------------------\n");
 }
 
 void display_free_blocks(free_node* free_lptr) {
     free_node* fptr = free_lptr;
     
     printf("\n%-15s %-15s %-15s %-15s %-15s\n", 
            "Variable Name", "Size", "Status", "Start Address", "End Address");
     printf("----------------------------------------------------------------\n");
     
     if(fptr == NULL) {
         printf("No free blocks available\n");
     }
     
     while(fptr != NULL) {
         printf("%-15s %-15d %-15s %-15d %-15d\n",
                "free", fptr->free_size, "free",
                fptr->free_start_address, fptr->free_end_address);
         fptr = fptr->next;
     }
     printf("----------------------------------------------------------------\n");
 }
 
 void free_all_heap(heap_node** heap_lptr, free_node** free_lptr) {
     heap_node* hptr = *heap_lptr;
     free_node* fptr = *free_lptr;
 
     while(hptr != NULL) {
         heap_node* temp = hptr;
         hptr = hptr->next;
         free(temp);
     }
 
     while(fptr != NULL) {
         free_node* temp = fptr;
         fptr = fptr->next;
         free(temp);
     }
 
     *heap_lptr = NULL;
     *free_lptr = NULL;
 }
 
 int main() {
     heap_node* heap_lptr = NULL;
     
     free_node* free_lptr = NULL;
      
     free_lptr = initilise_heap(free_lptr,MAX_HEAP_SIZE);
 
     int choice;
     char continue_program = 'y';
     
     while(continue_program == 'y' || continue_program == 'Y') {
         printf("\nMAX HEAP SIZE IS %d\n", MAX_HEAP_SIZE);
         printf("1.initialize free blocks\n");
         printf("2. Insert in heap with first fit allocation\n");
         printf("3. Insert in heap with best fit allocation\n");
         printf("4. Deallocate variable from heap\n");
         printf("5. Display entire heap\n");
         printf("6. Display allocated blocks\n");
         printf("7. Display free blocks\n");
         printf("0. Exit program\n");
         printf("Enter your choice: ");
         
         if(scanf("%d", &choice) != 1) {
             printf("Invalid input. Please enter a number.\n");
             while(getchar() != '\n');  // Clear input buffer
             continue;
         }
 
         switch(choice) {
            case 1: {
                int size;
                  printf("enter the size of the block: ");
                  scanf("%d",&size);
                  free_lptr = initilise_heap(free_lptr,size);
                break;
            }
             case 2: {
                 int size;
                 char name[100];
                 printf("Enter variable size to be allocated: ");
                 scanf("%d", &size);
                 printf("Enter variable name to be allocated: ");
                 scanf("%s", name);
                 heap_lptr = insert_in_heap_with_first_allocation(&free_lptr, heap_lptr, size, name);
                 break;
             }
             case 3: {
                 int size;
                 char name[100];
                 printf("Enter variable size to be allocated: ");
                 scanf("%d", &size);
                 printf("Enter variable name to be allocated: ");
                 scanf("%s", name);
                 heap_lptr = insert_in_heap_with_best_fit_allocation(&free_lptr, heap_lptr, size, name);
                 break;
             }
             case 4: {
                 char to_deallocate[100];
                 printf("Enter variable name to deallocate: ");
                 scanf("%s", to_deallocate);
                 heap_lptr = deallocate(heap_lptr, &free_lptr, to_deallocate);
                 break;
             }
             case 5:
                 display_heap(heap_lptr, free_lptr);
                 break;
             case 6:
                 display_allocated_blocks(heap_lptr);
                 break;
             case 7:
                 display_free_blocks(free_lptr);
                 break;
             case 0:
                 printf("Program exited successfully after freeing all the heap\n");
                 free_all_heap(&heap_lptr, &free_lptr);
                 continue_program = 'n';
                 break;
             default:
                 printf("Invalid choice. Please try again.\n");
         }
         
         if(choice != 0) {
             printf("\nDo you want to continue? (y/n): ");
             while(getchar() != '\n');  // Clear input buffer
             scanf("%c", &continue_program);
         }
     }
     return 0;
 }