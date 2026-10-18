#include <unity.h>
#include "mode_control.h"
#include "motor_hal.h"
#include "config.h"

void setUp() {
    hal_init();
    hal_set_logging(false);
}
void tearDown() {}

void test_driving_mode_outputs() {
    mode_apply(MODE_DRIVING);
    HalState state = hal_get_state();
    TEST_ASSERT_EQUAL_INT(cfg::MODE1_SERVO1, state.servo1_deg);
    TEST_ASSERT_EQUAL_INT(cfg::MODE1_SERVO2, state.servo2_deg);
    TEST_ASSERT_FALSE(state.actuator1_signal1_high);
    TEST_ASSERT_FALSE(state.actuator2_signal1_high);
    TEST_ASSERT_EQUAL_INT(255, state.led_red);
}

void test_landed_mode_outputs() {
    mode_apply(MODE_LANDED);
    HalState state = hal_get_state();
    TEST_ASSERT_EQUAL_INT(cfg::MODE2_SERVO1, state.servo1_deg);
    TEST_ASSERT_FALSE(state.actuator1_signal1_high);
    TEST_ASSERT_EQUAL_INT(255, state.led_blue);
    TEST_ASSERT_EQUAL_INT(1, state.led_red);
}

void test_flying_mode_outputs() {
    mode_apply(MODE_FLYING);
    HalState state = hal_get_state();
    TEST_ASSERT_EQUAL_INT(cfg::MODE3_SERVO1, state.servo1_deg);
    TEST_ASSERT_TRUE(state.actuator1_signal1_high);
    TEST_ASSERT_TRUE(state.actuator2_signal1_high);
}

void test_switching_modes_changes_actuators_back() {
    mode_apply(MODE_FLYING);
    mode_apply(MODE_DRIVING);
    HalState state = hal_get_state();
    TEST_ASSERT_FALSE(state.actuator1_signal1_high);
}

void test_only_driving_allows_ground_drive() {
    TEST_ASSERT_TRUE(mode_allows_ground_drive(MODE_DRIVING));
    TEST_ASSERT_FALSE(mode_allows_ground_drive(MODE_LANDED));
    TEST_ASSERT_FALSE(mode_allows_ground_drive(MODE_FLYING));
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_driving_mode_outputs);
    RUN_TEST(test_landed_mode_outputs);
    RUN_TEST(test_flying_mode_outputs);
    RUN_TEST(test_switching_modes_changes_actuators_back);
    RUN_TEST(test_only_driving_allows_ground_drive);
    return UNITY_END();
}
