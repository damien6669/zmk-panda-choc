#include <zephyr/kernel.h>
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>

static bool argent_breach_done = false;

#define NUM_LEDS RGB_UNDERGLOW_NUM_LEDS
#define HALF_LEDS (NUM_LEDS / 2)

// Helper function to set all LEDs to one color
static void set_all_leds(const struct zmk_led_color *color) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        rgb_underglow_set_color(i, color);
    }
}

// Argent Breach startup wave animation
static void argent_breach_wave(void) {
    struct zmk_led_color color = {0, 0, 0};

    // All LEDs off at start
    set_all_leds(&color);

    // Wave effect moving outward from center
    for (uint8_t step = 0; step < (NUM_LEDS / 2); step++) {
        uint8_t hue = (step * 40) % 255;

        // Left side
        if ((NUM_LEDS / 2) - step - 1 < NUM_LEDS) {
            struct zmk_led_color left_color = hsv_to_rgb(hue, 255, 255);
            rgb_underglow_set_color((NUM_LEDS / 2) - step - 1, &left_color);
        }

        // Right side
        if ((NUM_LEDS / 2) + step < NUM_LEDS) {
            struct zmk_led_color right_color = hsv_to_rgb(hue, 255, 255);
            rgb_underglow_set_color((NUM_LEDS / 2) + step, &right_color);
        }

        k_sleep(K_MSEC(120));
    }

    // Set solid red after animation
    struct zmk_led_color red = hsv_to_rgb(0, 255, 255);
    set_all_leds(&red);
}

// Event handler to set red on wake-up
static int argent_breach_wake_handler(const struct zmk_event_header *eh) {
    const struct zmk_activity_state_changed *ev = cast_zmk_activity_state_changed(eh);
    if (ev->state == ZMK_ACTIVITY_ACTIVE) {
        struct zmk_led_color red = hsv_to_rgb(0, 255, 255);
        set_all_leds(&red);
    }
    return 0;
}

ZMK_LISTENER(argent_breach_listener, argent_breach_wake_handler);
ZMK_SUBSCRIPTION(argent_breach_listener, zmk_activity_state_changed);

// Initialization function
void argent_breach_init(void) {
    if (!argent_breach_done) {
        argent_breach_wave();
    }
}
