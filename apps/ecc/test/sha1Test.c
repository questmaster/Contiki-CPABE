#include "sha1Test.h"
#include <unity.h>

#include "../sha1.h"


void test_Sha1Computation(void) {
    SHA1Context context;
    const uint8_t *message = "Text";
    uint8_t output[SHA1HashSize];
    uint8_t expected[SHA1HashSize] = {0xc3, 0x32, 0x8c, 0x39, 0xb0, 0xe2, 0x9f, 0x78, 0xe9, 0xff, 0x45, 0xdb, 0x67, 0x42, 0x48, 0xb1, 0xd2, 0x45, 0x88, 0x7d};

    TEST_ASSERT_EQUAL_INT(shaSuccess, SHA1_Reset(&context));
    TEST_ASSERT_EQUAL_INT(shaSuccess, SHA1_Update(&context, message, 4));
    TEST_ASSERT_EQUAL_INT(shaSuccess, SHA1_Digest(&context, output));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, output, SHA1HashSize);
}

void test_Sha1InvalidInput(void) {
    SHA1Context context;
    const uint8_t *message = NULL;

    TEST_ASSERT_EQUAL_INT(shaSuccess, SHA1_Reset(&context));
    TEST_ASSERT_EQUAL_INT(shaNull, SHA1_Update(&context, message, 4));
}

void test_Sha1InvalidUse(void) {
    SHA1Context context;
    const uint8_t *message = "Text";
    uint8_t output[SHA1HashSize];

    TEST_ASSERT_EQUAL_INT(shaSuccess, SHA1_Reset(&context));
    TEST_ASSERT_EQUAL_INT(shaSuccess, SHA1_Update(&context, message, 4));
    TEST_ASSERT_EQUAL_INT(shaSuccess, SHA1_Digest(&context, output));

    TEST_ASSERT_EQUAL_INT(shaStateError, SHA1_Update(&context, message, 4));

}