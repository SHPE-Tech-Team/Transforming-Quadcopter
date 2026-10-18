#include <unity.h>
#include "robot.h"
#include "motor_hal.h"
#include "config.h"

void setUp() {
    hal_set_logging(false);
    robot_init();
}
void tearDown() {}

// Runs the loop from time 0 up to end_ms.
static void run_until(uint32_t end_ms) {
    uint32_t t;
    for (t = 0; t <= end_ms; t = t + cfg::LOOP_PERIOD_MS) {
        robot_step(t);
    }
}

void test_starts_disarmed_stopped_and_landed() {
    robot_step(0);
    HalState state = hal_get_state();
    TEST_ASSERT_FALSE(robot_is_armed());
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, state.pulse_left_us);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, state.pulse_right_us);
    TEST_ASSERT_EQUAL_INT(cfg::MODE2_SERVO1, state.servo1_deg);
}

void test_arms_after_start_delay() {
    run_until(cfg::START_DELAY_MS - cfg::LOOP_PERIOD_MS);
    TEST_ASSERT_FALSE(robot_is_armed());
    robot_step(cfg::START_DELAY_MS);
    TEST_ASSERT_TRUE(robot_is_armed());
}

void test_drives_forward_in_first_step() {
    run_until(cfg::START_DELAY_MS + 1000);
    HalState state = hal_get_state();
    // forward 0.6 gives a pulse of 187.5 - 62.5 * 0.6 = 150
    float expected = 187.5f - 62.5f * 0.6f * cfg::SPEED_LIMIT;
    TEST_ASSERT_FLOAT_WITHIN(1.0f, expected, state.pulse_left_us);
    TEST_ASSERT_FLOAT_WITHIN(1.0f, expected, state.pulse_right_us);
    TEST_ASSERT_EQUAL_INT(cfg::MODE1_SERVO1, state.servo1_deg);
}

void test_turns_in_second_step() {
    run_until(cfg::START_DELAY_MS + 3000 + 750);
    HalState state = hal_get_state();
    // turning right: left wheel forward (short pulse), right wheel backward (long pulse)
    TEST_ASSERT_TRUE(state.pulse_left_us < 187.5f);
    TEST_ASSERT_TRUE(state.pulse_right_us > 187.5f);
}

void test_kill_stops_motors_and_stays_stopped() {
    run_until(cfg::START_DELAY_MS + 1000);
    robot_kill();
    robot_step(cfg::START_DELAY_MS + 1010);
    HalState state = hal_get_state();
    TEST_ASSERT_FALSE(robot_is_armed());
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, state.pulse_left_us);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, state.pulse_right_us);

    robot_step(cfg::START_DELAY_MS + 1020);
    state = hal_get_state();
    TEST_ASSERT_FALSE(robot_is_armed());
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 187.5f, state.pulse_left_us);
}

void test_stopped_after_script_ends() {
    run_until(15000);
    HalState state = hal_get_state();
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 187.5f, state.pulse_left_us);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 187.5f, state.pulse_right_us);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_starts_disarmed_stopped_and_landed);
    RUN_TEST(test_arms_after_start_delay);
    RUN_TEST(test_drives_forward_in_first_step);
    RUN_TEST(test_turns_in_second_step);
    RUN_TEST(test_kill_stops_motors_and_stays_stopped);
    RUN_TEST(test_stopped_after_script_ends);
    return UNITY_END();
}
