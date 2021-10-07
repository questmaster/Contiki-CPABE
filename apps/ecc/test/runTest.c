#include <unity.h>

#include "sha1Test.h"

void setUp() {}

void tearDown() {}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_Sha1Computation);
  RUN_TEST(test_Sha1InvalidInput);
  RUN_TEST(test_Sha1InvalidUse);

  return UNITY_END();
}