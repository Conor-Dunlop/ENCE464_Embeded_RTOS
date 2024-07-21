#include "unity.h"
#include "circBufT.h"

const uint8_t STANDARD_TEST_CAPACITY = 5;
static circBuf_t buff;

void setUp(void)
{
    initCircBuf(&buff, STANDARD_TEST_CAPACITY);
}

void tearDown(void)
{
    freeCircBuf(&buff);
}

/* Helper functions */
void writeConsecutiveSequenceToBuffer(uint16_t start, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
      writeCircBuf(&buff, start + i);
    }
} 

void assertReadingSequence(uint16_t start, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++) {
      TEST_ASSERT_EQUAL(start + i, readCircBuf(&buff));
    }
} 

int32_t * reconstructBufferWithSize(uint16_t size)
{
  freeCircBuf(&buff);
  return initCircBuf(&buff, size);
}

/* Test cases */

void test_new_buffer_is_empty(void)
{
    // Arrange: given buffer is empty

    // Act: when buffer is read
    int32_t value = readCircBuf(&buff);

    // Assert: then 0 is returned
    TEST_ASSERT_EQUAL(0, value);
}

void test_single_element_in_single_element_out(void)
{
    // Arrange: given buffer has a single element
    writeCircBuf(&buff, 11);

    // Act: when buffer is read
    int32_t value = readCircBuf(&buff);

    // Assert: then the same value is returned
    TEST_ASSERT_EQUAL(11, value);
}

void test_first_element_in_first_element_out(void)
{
    // Arrange
    writeConsecutiveSequenceToBuffer(20, STANDARD_TEST_CAPACITY);

    // Act/Assert
    assertReadingSequence(20, STANDARD_TEST_CAPACITY);
}

void test_write_and_read_indices_are_independent(void)
{
    for (uint8_t i = 0; i < STANDARD_TEST_CAPACITY; i++)
    {
      // Arrange: given one element is written
      writeCircBuf(&buff, 20 + i);

      // Act: when buffer is read
      int32_t value = readCircBuf(&buff);

      // Assert: the last written element is returned
      TEST_ASSERT_EQUAL(20 + i, value);
    }
}

void test_buffer_is_clean_after_full_buffer_cycle_completed(void)
{
    TEST_IGNORE(); // Remove this when the test is written

    // Arange: given buffer is fully written to and and then fully read from

    // Act: when buffer is read

    // Assert: same behaviour as when buffer was empty
}

void test_buffer_is_circular(void)
{
    TEST_IGNORE(); // Remove this when the test is written

    // Arange: given buffer is fully written to and then fully read from

    // Arrange: given a new value is written

    // Act: when buffer is read

    // Assert: the last written element is returned
}

void test_no_values_overwritten_after_full(void)
{
    TEST_IGNORE(); // Remove this when the test is written

    // Arrange: given buffer is filled to capacity

    // Given: when one more element is written to buffer

    // Assert: first element in, first element out, no overflow
}

void test_min_capacity_when_buffer_is_created_then_buffer_empty(void)
{
    // Arrange
    reconstructBufferWithSize(1);

    // Act/Assert
    TEST_ASSERT_EQUAL(0, readCircBuf(&buff));
}

void test_min_capacity_when_single_element_written_to_buffer_then_same_value_is_read(void)
{
    // Arrange
    reconstructBufferWithSize(1);

    // Act
    writeCircBuf(&buff, 87);

    // Act/Assert
    TEST_ASSERT_EQUAL(87, readCircBuf(&buff));
}

void test_capacity_0_invalid(void)
{
    TEST_IGNORE(); // Remove this when the test is written

    // Arrange/Act

    // Assert: the return value of initCircBuf is NULL
}

void test_capacity_higher_than_max_invalid(void)
{
    TEST_IGNORE(); // Remove this when the test is written

    // Arrange/Act

    // Assert: the return value of initCircBuf is NULL
}
