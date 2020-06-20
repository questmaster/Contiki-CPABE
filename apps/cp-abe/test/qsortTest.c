#include "qsortTest.h"
#include <unity.h>

#include "../qsort.h"

int int_comparison(const void *lhs, const void *rhs) {
  int l = *((int *)lhs);
  int r = *((int *)rhs);

  if (l < r) {
    return -1;
  } else {
    if (l == r) {
      return 0;
    } else {
      return 1;
    }
  }
}

void test_BasicSorting(void) {
  int input[] = {9, 4, 7, 1};
  int expected[] = {1, 4, 7, 9};
  int numElems = 4;

  qsort(&input, numElems, sizeof(int), int_comparison);

  TEST_ASSERT_EQUAL_INT_ARRAY(expected, input, numElems);
}

void test_NegativeSorting(void) {
  int input[] = {9, -4, 7, -1};
  int expected[] = {-4, -1, 7, 9};
  int numElems = 4;

  qsort(&input, numElems, sizeof(int), int_comparison);

  TEST_ASSERT_EQUAL_INT_ARRAY(expected, input, numElems);
}

void test_OddNumberSorting(void) {
  int input[] = {9, 4, 7, 1, 70};
  int expected[] = {1, 4, 7, 9, 70};
  int numElems = 5;

  qsort(&input, numElems, sizeof(int), int_comparison);

  TEST_ASSERT_EQUAL_INT_ARRAY(expected, input, numElems);
}
