#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#define ASSERT_EQ(a, b)     REQUIRE((a) == (b))
#define ASSERT_TRUE(a)      REQUIRE(a)
#define ASSERT_FALSE(a)     REQUIRE(!(a))


extern "C" {
#include "jagged.h"
}

const int TEST_BINS = 8;

TEST_CASE("Jagged.Create", "[Jagged]") {
    jagged_t jagged;
    jagged_init(&jagged, TEST_BINS);

    ASSERT_EQ(TEST_BINS, jagged_bins(&jagged));
    ASSERT_EQ(0, jagged_size(&jagged));
    ASSERT_EQ(NULL, jagged.packed_values);

    for (int i = 0; i < 8; i++) {
        ASSERT_EQ(NULL, jagged.bins[i]);
    }

    jagged_free(&jagged);
}

// Get the index-th element from the linked list
int test_entry_get_element(entry_t* list, int index) {
    if (index == 0) {
        return list->value;
    } else {
        return test_entry_get_element(list->next, index - 1);
    }
}

TEST_CASE("Jagged.Add", "[Jagged]") {
    jagged_t jagged;
    jagged_init(&jagged, TEST_BINS);;

    jagged_add(&jagged, 1, 5);

    ASSERT_EQ(1, jagged_size(&jagged));
    int success = 0;
    ASSERT_EQ(5, jagged_element(&jagged, 1, 0, &success));
    ASSERT_EQ(0, success);
    ASSERT_EQ(5, test_entry_get_element(jagged.bins[1], 0));

    jagged_add(&jagged, 1, 7);
    ASSERT_EQ(2, jagged_size(&jagged));
    success = 0;
    ASSERT_EQ(7, jagged_element(&jagged, 1, 1, &success));
    ASSERT_EQ(0, success);
    ASSERT_EQ(7, test_entry_get_element(jagged.bins[1], 1));

    jagged_add(&jagged, 7, 71);
    ASSERT_EQ(3, jagged_size(&jagged));
    ASSERT_EQ(71, jagged_element(&jagged, 7, 0, &success));
    ASSERT_EQ(71, test_entry_get_element(jagged.bins[7], 0));

    // Check linked list structure
    ASSERT_EQ(7, jagged.bins[1]->next->value);

    jagged_free(&jagged);
}

TEST_CASE("Jagged.Remove", "[Jagged]") {
    jagged_t jagged;
    jagged_init(&jagged, TEST_BINS);

    jagged_add(&jagged, 2, 5);
    jagged_add(&jagged, 2, 7);
    jagged_add(&jagged, 2, 15);
    jagged_add(&jagged, 6, 71);

    jagged_remove(&jagged, 2, 2);
    ASSERT_EQ(3, jagged_size(&jagged));

    int success = 0;
    jagged_element(&jagged, 2, 2, &success);
    ASSERT_EQ(-1, success);

    jagged_remove(&jagged, 2, 0);
    ASSERT_EQ(7, jagged_element(&jagged, 2, 0, &success));
    ASSERT_EQ(0, success);

    // Check linked list structure
    ASSERT_EQ(7, jagged.bins[2]->value);
    ASSERT_EQ(NULL, jagged.bins[2]->next);

    jagged_free(&jagged);
}

TEST_CASE("Jagged.Pack", "[Jagged]") {
    jagged_t jagged;
    jagged_init(&jagged, TEST_BINS);

    jagged_add(&jagged, 2, 5);
    jagged_add(&jagged, 2, 7);
    jagged_add(&jagged, 2, 15);
    jagged_add(&jagged, 6, 71);

    jagged_pack(&jagged);

    ASSERT_EQ(NULL, jagged.bins);
    ASSERT_FALSE(jagged.packed_values == NULL);
    ASSERT_FALSE(jagged.offsets == NULL);

    int success = 0;
    ASSERT_EQ(5, jagged_element(&jagged, 2, 0, &success));
    ASSERT_EQ(7, jagged_element(&jagged, 2, 1, &success));
    ASSERT_EQ(15, jagged_element(&jagged, 2, 2, &success));
    ASSERT_EQ(71, jagged_element(&jagged, 6, 0, &success));

    ASSERT_EQ(-1, jagged_add(&jagged, 3, 55));

    // Test the offsets array
    ASSERT_EQ(0, jagged.offsets[0]);
    ASSERT_EQ(0, jagged.offsets[1]);
    ASSERT_EQ(0, jagged.offsets[1]);
    ASSERT_EQ(3, jagged.offsets[3]);
    ASSERT_EQ(3, jagged.offsets[4]);
    ASSERT_EQ(3, jagged.offsets[5]);
    ASSERT_EQ(3, jagged.offsets[6]);
    ASSERT_EQ(4, jagged.offsets[7]);

    jagged_free(&jagged);
}

TEST_CASE("Jagged.Unpack", "[Jagged]") {
    jagged_t jagged;
    jagged_init(&jagged, TEST_BINS);

    jagged_add(&jagged, 2, 5);
    jagged_add(&jagged, 2, 7);
    jagged_add(&jagged, 2, 15);
    jagged_add(&jagged, 6, 71);

    jagged_pack(&jagged);
    ASSERT_EQ(NULL, jagged.bins);

    jagged_unpack(&jagged);

    ASSERT_FALSE(jagged.bins == NULL);
    ASSERT_EQ(NULL, jagged.packed_values);
    ASSERT_EQ(NULL, jagged.offsets);

    int success = 0;
    ASSERT_EQ(5, jagged_element(&jagged, 2, 0, &success));
    ASSERT_EQ(7, jagged_element(&jagged, 2, 1, &success));
    ASSERT_EQ(15, jagged_element(&jagged, 2, 2, &success));
    ASSERT_EQ(71, jagged_element(&jagged, 6, 0, &success));

    jagged_free(&jagged);
}

TEST_CASE("Jagged.Leak", "[Jagged]") {
    jagged_t jagged;
    jagged_init(&jagged, TEST_BINS);

    jagged_add(&jagged, 2, 5);
    jagged_add(&jagged, 2, 7);
    jagged_add(&jagged, 2, 15);
    jagged_add(&jagged, 6, 71);

    jagged_pack(&jagged);

    jagged_unpack(&jagged);

    int success = 0;
    ASSERT_EQ(5, jagged_element(&jagged, 2, 0, &success));
    ASSERT_EQ(7, jagged_element(&jagged, 2, 1, &success));
    ASSERT_EQ(15, jagged_element(&jagged, 2, 2, &success));
    ASSERT_EQ(71, jagged_element(&jagged, 6, 0, &success));

    jagged_free(&jagged);
}
