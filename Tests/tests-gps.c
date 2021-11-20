#include "tests-gps.h"

void test_GPS_mode_should_change(void)
{
    /* This Test Updates gps mode */
    /* Expected Result: 0 */ 
    SetMode('l');
    TEST_ASSERT_EQUAL(0,ReturnMode());
}

void test_GPS_check_enter(void)
{
    /* This Test checks te incoming char for enters */
    /* Expected Result: true */ 
    TEST_ASSERT_EQUAL(true,Check_Enter(10));
}

void test_GPS_return_last_coords(void)
{
    /* This Test returned the last known gps coordinates */
    /* Expected Result: "52.00;6.00" */ 
    TEST_ASSERT_EQUAL_STRING("52.00;6.00",ReturnLastCoords());
}
