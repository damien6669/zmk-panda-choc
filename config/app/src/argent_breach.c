#include <zephyr/kernel.h>
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>

static bool argent_breach_done = false;

// Define the LED layout for Sofle V2 Choc
#define NUM_LEDS RGB_UNDERGLOW_NUM_LEDS
#define HALF_LEDS (NUM_LEDS / 2)

// Function to gradually light up LEDs from the middle outward
static void argent_breach_wave(void) {
    uint8_t i, step;
    struct zmk_led_color color;

    // Start with all LEDs off (black)
    color.r = 0;
    color.g = 0;
    color.b = 0;
    for (i = 0; i < NUM_LEDS; i++) {
        rgb_underglow_set_color(i, &color);
    }

    // Wave effect moving outward from the center (encoder/LCD side)
    for (step = 0; step < HALF_LEDS; step++) {
        uint8_t hue = (step * 40) % 255; // Gradient spectrum effect

        // Left half: Move outward from the center
        if (HALF_LEDS - step - 1 >= 0) {
            struct zmk_led_color left_color = hsv_to_rgb(hue, 255, 255);
            rgb_underglow_set_color(HALF_LEDS - step - 1, &left_color);
        }

        // Right half: Move outward from the center
        if (HALF_LEDS + step < NUM_LEDS) {
            struct zmk_led_color right_color = hsv_to_rgb(hue, 255, 255);
            rgb_underglow_set_color(HALF_LEDS + step, &right_color);
        }

        k_sleep(K_MSEC(120)); // Delay for wave effect speed
    }

    // Once the wave completes, set all LEDs to solid red
    struct zmk_led_color red_color = hsv_to_rgb(0, 255, 255); // Red
    for (i = 0; i < NUM_LEDS; i++) {
        rgb_underglow_set_color(i, &red_color);
    }

    argent_breach_done = true;
}

// Function to immediately set solid red on wake-up
static int argent_breach_wake_handler(const struct zmk_event_header *eh) {
    const struct zmk_activity_state_changed *ev = cast_zmk_activity_state_changed(eh);
    if (ev->state == ZMK_ACTIVITY_ACTIVE) {
        struct zmk_led_color red_color = hsv_to_rgb(0, 255, 255); // Red
        for (uint8_t i = 0; i < NUM_LEDS; i++) {
            rgb_underglow_set_color(i, &red_color);
        }
    }
    return 0;
}

// Register the wake-up event listener (for solid red)
ZMK_LISTENER(argent_breach_listener, argent_breach_wake_handler);
ZMK_SUBSCRIPTION(argent_breach_listener, zmk_activity_state_changed);

// Run only once at startup
void argent_breach_init(void) {
    if (!argent_breach_done) {
        argent_breach_wave();
    }
}
