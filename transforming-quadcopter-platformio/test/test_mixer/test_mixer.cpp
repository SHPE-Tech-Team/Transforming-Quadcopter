#include <unity.h>
#include "mixer.h"
#include "config.h"

// These tests assume cfg::INVERT_LEFT and cfg::INVERT_RIGHT are false.
void setUp() {}
void tearDown() {}

void test_zero_in_gives_zero_out() {
    Command cmd = { 0.0f, 0.0f, MODE_DRIVING };
    MotorCmd out = mix_ground(cmd);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, out.left);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, out.right);
}

void test_full_forward_uses_speed_limit() {
    Command cmd = { 1.0f, 0.0f, MODE_DRIVING };
    MotorCmd out = mix_ground(cmd);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, cfg::SPEED_LIMIT, out.left);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, cfg::SPEED_LIMIT, out.right);
}

void test_backward_is_negative() {
    Command cmd = { -0.5f, 0.0f, MODE_DRIVING };
    MotorCmd out = mix_ground(cmd);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -0.5f * cfg::SPEED_LIMIT, out.left);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -0.5f * cfg::SPEED_LIMIT, out.right);
}

void test_steer_right_makes_left_wheel_faster() {
    Command cmd = { 0.5f, 1.0f, MODE_DRIVING };
    MotorCmd out = mix_ground(cmd);
    TEST_ASSERT_TRUE(out.left > out.right);
}

void test_steer_without_drive_turns_wheels_opposite_ways() {
    Command cmd = { 0.0f, 1.0f, MODE_DRIVING };
    MotorCmd out = mix_ground(cmd);
    TEST_ASSERT_FLOAT_WITHIN(0.001f,  cfg::STEER_GAIN * cfg::SPEED_LIMIT, out.left);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -cfg::STEER_GAIN * cfg::SPEED_LIMIT, out.right);
}

void test_small_commands_are_deadbanded() {
    Command cmd = { 0.05f, 0.0f, MODE_DRIVING };
    MotorCmd out = mix_ground(cmd);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, out.left);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, out.right);
}

void test_output_is_capped_at_full_power() {
    // drive 1 plus steer 1 asks the left wheel for 1.2, which must be capped at 1.0
    Command cmd = { 1.0f, 1.0f, MODE_DRIVING };
    MotorCmd out = mix_ground(cmd);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f * cfg::SPEED_LIMIT, out.left);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.8f * cfg::SPEED_LIMIT, out.right);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_zero_in_gives_zero_out);
    RUN_TEST(test_full_forward_uses_speed_limit);
    RUN_TEST(test_backward_is_negative);
    RUN_TEST(test_steer_right_makes_left_wheel_faster);
    RUN_TEST(test_steer_without_drive_turns_wheels_opposite_ways);
    RUN_TEST(test_small_commands_are_deadbanded);
    RUN_TEST(test_output_is_capped_at_full_power);
    return UNITY_END();
}
