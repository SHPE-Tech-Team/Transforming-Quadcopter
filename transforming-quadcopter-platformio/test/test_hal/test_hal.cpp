#include <unity.h>
#include "motor_hal.h"

void setUp()
{
    hal_init();
    hal_set_logging(false);
}
void tearDown() {}

void test_pulse_for_stopped_is_middle()
{
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, command_to_pulse_us(0.0f));
}

void test_pulse_for_full_forward_is_shortest()
{
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 125.0f, command_to_pulse_us(1.0f));
}

void test_pulse_for_full_backward_is_longest()
{
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 250.0f, command_to_pulse_us(-1.0f));
}

void test_pulse_never_leaves_range()
{
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 125.0f, command_to_pulse_us(5.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 250.0f, command_to_pulse_us(-5.0f));
}

void test_init_is_stopped()
{
    HalState state = hal_get_state();
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, state.pulse_left_us);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, state.pulse_right_us);
}

void test_set_ground_motor_updates_correct_side()
{
    hal_set_ground_motor(SIDE_RIGHT, 1.0f);
    HalState state = hal_get_state();
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 125.0f, state.pulse_right_us);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, state.pulse_left_us);
}

void test_servo_angle_is_limited()
{
    hal_set_servo(1, 500);
    hal_set_servo(2, -20);
    HalState state = hal_get_state();
    TEST_ASSERT_EQUAL_INT(180, state.servo1_deg);
    TEST_ASSERT_EQUAL_INT(0, state.servo2_deg);
}

void test_actuator_and_led_are_stored()
{
    hal_set_actuator(1, true);
    hal_set_actuator(2, false);
    hal_set_led(10, 20, 30);
    HalState state = hal_get_state();
    TEST_ASSERT_TRUE(state.actuator1_signal1_high);
    TEST_ASSERT_FALSE(state.actuator2_signal1_high);
    TEST_ASSERT_EQUAL_INT(20, state.led_green);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_pulse_for_stopped_is_middle);
    RUN_TEST(test_pulse_for_full_forward_is_shortest);
    RUN_TEST(test_pulse_for_full_backward_is_longest);
    RUN_TEST(test_pulse_never_leaves_range);
    RUN_TEST(test_init_is_stopped);
    RUN_TEST(test_set_ground_motor_updates_correct_side);
    RUN_TEST(test_servo_angle_is_limited);
    RUN_TEST(test_actuator_and_led_are_stored);
    return UNITY_END();
}
