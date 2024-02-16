#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jagged.h"

// Initialize a jagged array object with the given number of bins.
// The array is initially unpacked.
void jagged_init(jagged_t* jagged, int bins) {
    jagged->number_of_bins = bins;
    jagged->size = 0;
    jagged->bins = (entry_t**)malloc(bins * sizeof(entry_t*));
    for (int i = 0; i < bins; i++) {
        jagged->bins[i] = NULL;
    }
    jagged->packed_values = NULL;
    jagged->offsets = NULL;
}

void jagged_free(jagged_t* jagged) {
    if (jagged->bins == NULL) {     // is packed
        free(jagged->packed_values);
        free(jagged->offsets);
    } else {
        for (int i = 0; i < jagged->number_of_bins; i++) {
            entry_t* entry = jagged->bins[i];
            while (entry != NULL) {
                entry_t* nextentry = entry->next;
                free(entry);
                entry = nextentry;
            }
            jagged->bins[i] = NULL;
        }
        free(jagged->bins);
        jagged->bins = NULL;
    }
}

// Return the number of elements in the jagged array
int jagged_size(jagged_t* jagged) {
    return jagged->size;
}

// Return the number of bins
int jagged_bins(jagged_t* jagged) {
    return jagged->number_of_bins;
}

// Return the number of slots in the given bin
int jagged_slots(jagged_t* jagged, int bin) {
    int count = 1;
    if (jagged->bins[bin] == NULL) {
        return 0;
    }
    entry_t* entry = jagged->bins[bin];
    while (entry->next != NULL) {
        entry = entry->next;
        count++;
    }
    return count;
}

// Return the element stored at the given bin and slot number.
// Success is set to 0 if the element was found, or -1 otherwise.
// If success is -1, 0 is returned.
int jagged_element(jagged_t* jagged, int bin, int slot, int* success) {
    *success = -1;
    if (jagged->bins == NULL) {     // ispacked
        *success = 0;
        if (jagged->offsets[bin + 1] == jagged->offsets[bin]) {
            *success = -1;
            return 0;
        } else {
            *success = 0;
            return jagged->packed_values[jagged->offsets[bin] + slot];
        }
    }   // else
    if (jagged->bins[bin] == NULL) {
        return 0;
    }
    *success = 0;
    entry_t* entry = jagged->bins[bin];
    for (int i = 0; i < slot; i ++) {
        if (entry->next == NULL) {
            *success = -1;
            break;
        }
        entry = entry->next;
    }
    if (*success == 0) {
        return entry->value;
    }
    return 0;
}

// Add an element to the bin. Return 0 is the element was
// added, or -1 if the representation is packed
int jagged_add(jagged_t* jagged, int bin, int element) {
    if (jagged->bins == NULL) {
        return -1;
    }
    if (jagged->bins[bin] == NULL) {                // bin is empty
        entry_t* newentry = (entry_t*) malloc(sizeof(entry_t));
        newentry->value = element;
        newentry->next = NULL;
        jagged->bins[bin] = newentry;
    } else {
        entry_t* entry = jagged->bins[bin];
        while (entry->next != NULL) {
            entry = entry->next;
        }                                           // found last node
        entry_t* newentry = (entry_t*) malloc(sizeof(entry_t));
        newentry->value = element;
        newentry->next = NULL;
        entry->next = newentry;
    }
    jagged->size++;
    return 0;
}

// Remove the element from the given bin and slot. Return 0 on success,
// or -1 if the representation was packed or element not found.
int jagged_remove(jagged_t* jagged, int bin, int slot) {
    if (jagged->bins == NULL) {
        return -1;
    }
    if (jagged->bins[bin] == NULL) {
        return -1;
    } else if (slot == 0) {
        entry_t* next = jagged->bins[bin]->next;
        free(jagged->bins[bin]);
        jagged->bins[bin] = next;
    } else {
        entry_t* entry = jagged->bins[bin];
        for (int i = 0; i < slot - 1; i ++) {
            if (entry->next == NULL) {
                return -1;
            }
            entry = entry->next;
        }
        if (entry->next == NULL) {
            return -1;
        }
        entry_t* nextnext = entry->next->next;
        free(entry->next);
        entry->next = nextnext;
    }
    jagged->size--;
    return 0;
}


// this removes a given element (passed into slot) instead of given position
/*int jagged_remove(jagged_t* jagged, int bin, int slot) {
    if (jagged->bins[bin] == NULL) {                // bin is empty
        return -1;
    } else if (jagged->bins[bin]->value == slot) {
        entry_t* nextentry = jagged->bins[bin]->next;
        free(jagged->bins[bin]);
        jagged->bins[bin] = nextentry;
    } else {
        entry_t* entry = jagged->bins[bin];
        while (entry->next != NULL && entry->next->value != slot) {
            entry = entry->next;
        }
        if (entry->next == NULL) {
            return -1;
        }
        entry_t* nextentry = entry->next->next;
        free(entry->next);
        entry->next = nextentry;
    }
    jagged->size--;
    return 0;
}*/

// Unpack the jagged array. Return 0 if successful or -1 if the array is
// already unpacked.
int jagged_unpack(jagged_t* jagged) {
    if (jagged->bins != NULL) {
        return -1;
    }
    printf("\nunpacking...\n");
    jagged->bins = (entry_t**)malloc(jagged->number_of_bins * sizeof(entry_t*));
    int difference = 0;
    int packedindex = 0;
    for (int i = 0; i < (jagged->number_of_bins - 1); i++) {
        if (jagged->offsets[i + 1] > jagged->offsets[i]) {
            difference = jagged->offsets[i + 1] - jagged->offsets[i];
            for (int j = 0; j < difference; j++) {
                if (jagged->bins[i] == NULL) {                // bin is empty
                    entry_t* newentry = (entry_t*) malloc(sizeof(entry_t));
                    newentry->value = jagged->packed_values[packedindex];
                    newentry->next = NULL;
                    jagged->bins[i] = newentry;
                } else {
                    entry_t* entry = jagged->bins[i];
                    while (entry->next != NULL) {
                        entry = entry->next;
                    }                                    // found last node
                    entry_t* newentry = (entry_t*) malloc(sizeof(entry_t));
                    newentry->value = jagged->packed_values[packedindex];
                    newentry->next = NULL;
                    entry->next = newentry;
                }
                packedindex++;
            }
            printf("\n");
        } else {
            jagged->bins[i] = NULL;
        }
    }   // final bin unload
    while (packedindex < jagged->size) {
        if (jagged->bins[jagged->number_of_bins - 1] == NULL) {  // bin is empty
            entry_t* newentry = (entry_t*) malloc(sizeof(entry_t));
            newentry->value = jagged->packed_values[packedindex];
            newentry->next = NULL;
            jagged->bins[jagged->number_of_bins - 1] = newentry;
        } else {
            entry_t* entry = jagged->bins[jagged->number_of_bins - 1];
            while (entry->next != NULL) {
                entry = entry->next;
            }                                           // found last node
            entry_t* newentry = (entry_t*) malloc(sizeof(entry_t));
            newentry->value = jagged->packed_values[packedindex];
            newentry->next = NULL;
            entry->next = newentry;
        }
        packedindex++;
    }
    printf("\n");
    free(jagged->packed_values);
    free(jagged->offsets);
    jagged->packed_values = NULL;
    jagged->offsets = NULL;
    return 0;
}

// Pack the jagged array. Return 0 if successful or -1 if the array is already
// packed.
int jagged_pack(jagged_t* jagged) {
    int offset = 0;
    if (jagged->bins == NULL) {
        return -1;
    }
    jagged->packed_values = malloc(jagged->size * sizeof(int));
    jagged->offsets = malloc(jagged->number_of_bins * sizeof(int));
    for (int i = 0; i < jagged->number_of_bins; i++) {
        jagged->offsets[i] = offset;
        if (jagged->bins[i] == NULL) {
            continue;
        }
        for (entry_t* entry = jagged->bins[i];
                entry != NULL; entry = entry->next) {
            jagged->packed_values[offset] = entry->value;
            offset++;
        }
    }   // packed_values & offsets filled
    for (int i = 0; i < jagged->number_of_bins; i++) {  // free unpacked array
        entry_t* entry = jagged->bins[i];
        while (entry != NULL) {
            entry_t* nextentry = entry->next;
            free(entry);
            entry = nextentry;
        }
        jagged->bins[i] = NULL;
    }
    free(jagged->bins);
    jagged->bins = NULL;
    return 0;
}

// Print a jagged array out. Useful for debugging
void jagged_print(jagged_t* jagged) {
    for (int i = 0; i < jagged->number_of_bins; i++) {
        printf("bin %d: ", i);
        if (jagged->bins[i] != NULL) {
            for (entry_t* entry = jagged->bins[i];
                    entry != NULL; entry = entry->next) {
                printf("%d ", entry->value);
            }
        }
        printf("\n");
    }
}
