#include <unity.h>

#include "cpabeTest.h"
#include "qsortTest.h"

void setUp() {}

void tearDown() {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_BasicSorting);
  RUN_TEST(test_NegativeSorting);
  RUN_TEST(test_OddNumberSorting);
  RUN_TEST(test_basicCpabe);

  return UNITY_END();
}