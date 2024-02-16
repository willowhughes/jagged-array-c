#include <stdio.h>
#include "jagged.h"

int main(int argc, char** argv) {
    jagged_t jagged;
    jagged_init(&jagged, 8);

    jagged_add(&jagged, 0, 5);
    jagged_add(&jagged, 0, 7);
    jagged_add(&jagged, 0, 9);
    jagged_add(&jagged, 6, 9);
    jagged_add(&jagged, 6, 3);
    jagged_add(&jagged, 1, 13);
    jagged_add(&jagged, 7, 100);
    jagged_add(&jagged, 7, 110);
    jagged_add(&jagged, 7, 120);
    jagged_add(&jagged, 6, 6);
    jagged_add(&jagged, 3, 3);

    jagged_print(&jagged);
    printf("jagged size: %d\n", jagged_size(&jagged));
    // int suc;
    // printf("element returned: %d\n", jagged_element(&jagged, 1, 5, &suc));
    // printf("success: %d\n", suc);

    // printf("remove return code: %d\n", jagged_remove(&jagged, 2, 0));
    // printf("# of slots: %d\n", jagged_slots(&jagged, 0));
    // jagged_print(&jagged);
    printf("jagged size: %d\n", jagged_size(&jagged));

    printf("pack return code: %d\n", jagged_pack(&jagged));
    printf("unpack return code: %d\n", jagged_unpack(&jagged));
    printf("pack return code: %d\n", jagged_pack(&jagged));
    printf("unpack return code: %d\n", jagged_unpack(&jagged));

    jagged_print(&jagged);
    jagged_free(&jagged);
}
