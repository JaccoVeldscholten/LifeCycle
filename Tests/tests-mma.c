#include "tests-mma.h"

void test_MMA_Should_Calculate_G(void){
    /* This test calculates the G force of the MMA*/
    /* Expected Result: 79 */ 
    TEST_ASSERT_EQUAL(79, AmountGToMMAValue(5));
}
