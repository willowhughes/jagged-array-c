#ifndef JAGGED_H_
#define JAGGED_H_

// A bin is a linked list
typedef struct entry {
    int value;
    struct entry* next;
} entry_t;

// A jagged array is made up to size number of array of pointers
// to bins in the unpacked representation, and the packed
// representation is an array of values and an array of offsets.
typedef struct {
    int size;
    int number_of_bins;

    // Unpacked representation
    entry_t** bins;

    // Packed representation
    int* packed_values;
    int* offsets;
} jagged_t;


// Initialize a jagged array object with the given number of bins.
// The array is initially unpacked.
void jagged_init(jagged_t* jagged, int bins);

// Free all memory associated with the jagged array
void jagged_free(jagged_t* jagged);

// Return the number of elements in the jagged array
int jagged_size(jagged_t* jagged);

// Return the number of bins
int jagged_bins(jagged_t* jagged);

// Return the number of slots in the given bin
int jagged_slots(jagged_t* jagged, int bin);

// Return the element stored at the given bin and slot number.
// The value pointed to by success is set to 0 if the element was found,
// or -1 otherwise.
// If the value pointed to by success is set to -1, 0 is returned.
int jagged_element(jagged_t* jagged, int bin, int slot, int* success);

// Add an element to the bin. Return 0 is the element was
// added, or -1 if the representation is packed
int jagged_add(jagged_t* jagged, int bin, int element);

// Remove the element from the given bin and slot. Return 0 on success,
// or -1 if the representation was packed or element not found.
// Remember to free any memory that was previously allocated to store
// the element.
int jagged_remove(jagged_t* jagged, int bin, int slot);

// Unpack the jagged array. Return 0 if successful or -1 if the array is
// already unpacked.
int jagged_unpack(jagged_t* jagged);

// Pack the jagged array. Return 0 if successful or -1 if the array is already
// packed.
int jagged_pack(jagged_t* jagged);

// Print a jagged array out. Useful for debugging.
// Use any format you find useful. If you only use the methods
// defined in the header to print, it should work for both packed
// and unpacked representations
void jagged_print(jagged_t* jagged);

#endif   // JAGGED_H_
