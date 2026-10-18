#include <unity.h>
#include "script.h"
#include "config.h"

void setUp() {
    filter_reset();
}
void tearDown() {}

static const Step TABLE[] = {
    { 1000, 0.5f, 0.0f, MODE_DRIVING },
    {  500, 0.0f, 1.0f, MODE_FLYING }
};

void test_holds_still_and_landed_during_start_delay() {
    Command cmd = script_get_from(TABLE, 2, cfg::START_DELAY_MS - 1);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, cmd.drive);
    TEST_ASSERT_EQUAL_INT(MODE_LANDED, cmd.mode);
}

void test_first_step_after_delay() {
    Command cmd = script_get_from(TABLE, 2, cfg::START_DELAY_MS + 10);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, cmd.drive);
    TEST_ASSERT_EQUAL_INT(MODE_DRIVING, cmd.mode);
}

void test_second_step() {
    Command cmd = script_get_from(TABLE, 2, cfg::START_DELAY_MS + 1200);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, cmd.steer);
    TEST_ASSERT_EQUAL_INT(MODE_FLYING, cmd.mode);
}

void test_after_last_step_stops_and_keeps_last_mode() {
    Command cmd = script_get_from(TABLE, 2, cfg::START_DELAY_MS + 5000);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, cmd.drive);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, cmd.steer);
    TEST_ASSERT_EQUAL_INT(MODE_FLYING, cmd.mode);
}

void test_empty_script_stays_landed() {
    Command cmd = script_get_from(TABLE, 0, cfg::START_DELAY_MS + 5000);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, cmd.drive);
    TEST_ASSERT_EQUAL_INT(MODE_LANDED, cmd.mode);
}

void test_default_script_done_flag() {
    TEST_ASSERT_FALSE(script_done(0));
    TEST_ASSERT_TRUE(script_done(60000));
}

void test_filter_moves_part_of_the_way() {
    Command raw = { 1.0f, 0.0f, MODE_DRIVING };
    Command first = filter_command(raw);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, cfg::CMD_FILTER_B, first.drive);
    Command second = filter_command(raw);
    TEST_ASSERT_TRUE(second.drive > first.drive);
    TEST_ASSERT_TRUE(second.drive < 1.0f);
}

void test_filter_passes_mode_through() {
    Command raw = { 0.0f, 0.0f, MODE_FLYING };
    Command out = filter_command(raw);
    TEST_ASSERT_EQUAL_INT(MODE_FLYING, out.mode);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_holds_still_and_landed_during_start_delay);
    RUN_TEST(test_first_step_after_delay);
    RUN_TEST(test_second_step);
    RUN_TEST(test_after_last_step_stops_and_keeps_last_mode);
    RUN_TEST(test_empty_script_stays_landed);
    RUN_TEST(test_default_script_done_flag);
    RUN_TEST(test_filter_moves_part_of_the_way);
    RUN_TEST(test_filter_passes_mode_through);
    return UNITY_END();
}
