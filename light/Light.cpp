/*
 * Copyright (C) 2018-2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LightService"

#include <log/log.h>

#include "Light.h"

#include <fstream>

#define LCD_LED         "/sys/class/backlight/panel0-backlight/"
#define WHITE_LED       "/sys/class/leds/red/"
#define BLUE_LED        "/sys/class/leds/blue/"
#define GREEN_LED       "/sys/class/leds/green/"

#define BREATH          "breath"
#define BRIGHTNESS      "brightness"
#define MAX_BRIGHTNESS  "max_brightness"
#define DELAY_OFF       "delay_off"
#define DELAY_ON        "delay_on"
namespace {
/*
 * Write value to path and close file.
 */
static void set(std::string path, std::string value) {
    std::ofstream file(path);

    if (!file.is_open()) {
        ALOGW("failed to write %s to %s", value.c_str(), path.c_str());
        return;
    }

    file << value;
}

static void set(std::string path, int value) {
    set(path, std::to_string(value));
}

static int get(std::string path) {
    std::ifstream file(path);
    int value;

    if (!file.is_open()) {
    ALOGW("failed to read from %s", path.c_str());
    return 0;
    }

    file >> value;
    return value;
}

static int getMaxBrightness(std::string path) {
    int value = get(path);
    ALOGW("Got max brightness %d", value);
    return value;
}

static uint32_t getBrightness(const LightState& state) {
    uint32_t alpha, red, green, blue;

    /*
     * Extract brightness from AARRGGBB.
     */
    alpha = (state.color >> 24) & 0xFF;
    red = (state.color >> 16) & 0xFF;
    green = (state.color >> 8) & 0xFF;
    blue = state.color & 0xFF;

    /*
     * Scale RGB brightness if Alpha brightness is not 0xFF.
     */
    if (alpha != 0xFF) {
        red = red * alpha / 0xFF;
        green = green * alpha / 0xFF;
        blue = blue * alpha / 0xFF;
    }

    return (77 * red + 150 * green + 29 * blue) >> 8;
}

static inline uint32_t scaleBrightness(uint32_t brightness, uint32_t maxBrightness) {
    if (brightness == 0) {
        return 0;
    }

    return (brightness - 1) * (maxBrightness - 1) / (0xFF - 1) + 1;
}

static inline uint32_t getScaledBrightness(const LightState& state, uint32_t maxBrightness) {
    return scaleBrightness(getBrightness(state), maxBrightness);
}

static void handleBacklight(const LightState& state) {
    uint32_t brightness = getScaledBrightness(state, getMaxBrightness(LCD_LED MAX_BRIGHTNESS));
    set(LCD_LED BRIGHTNESS, brightness);
}

static void handleNotification(const LightState& state) {
    uint32_t whiteBrightness = getScaledBrightness(state, getMaxBrightness(WHITE_LED MAX_BRIGHTNESS));
    uint32_t blueBrightness = getScaledBrightness(state, getMaxBrightness(BLUE_LED MAX_BRIGHTNESS));
    uint32_t greenBrightness = getScaledBrightness(state, getMaxBrightness(GREEN_LED MAX_BRIGHTNESS));

    /* Disable breathing */
    set(WHITE_LED BREATH, 0);
    set(BLUE_LED BREATH, 0);
    set(GREEN_LED BREATH, 0);

    if (state.flashMode == Flash::TIMED) {
        /* White, Blue and GREEN */
        set(WHITE_LED DELAY_OFF, state.flashOnMs);
        set(WHITE_LED DELAY_ON, state.flashOffMs);
        set(BLUE_LED DELAY_OFF, state.flashOnMs);
	set(BLUE_LED DELAY_ON, state.flashOffMs);
        set(GREEN_LED DELAY_OFF, state.flashOnMs);
	set(GREEN_LED DELAY_ON, state.flashOffMs);

        /* Enable Breathing */
        set(WHITE_LED BREATH, 1);
        set(BLUE_LED BREATH, 1);
        set(BLUE_LED BREATH, 1);
    } else {
        set(WHITE_LED BRIGHTNESS, whiteBrightness);
        set(BLUE_LED BRIGHTNESS, blueBrightness);
        set(GREEN_LED BRIGHTNESS, greenBrightness);
    }
}

static inline bool isLit(const LightState& state) {
    return state.color & 0x00ffffff;
}

/* Keep sorted in the order of importance. */
static std::vector<LightBackend> backends = {
    { Type::ATTENTION, handleNotification },
    { Type::NOTIFICATIONS, handleNotification },
    { Type::BATTERY, handleNotification },
    { Type::BACKLIGHT, handleBacklight },
};

}  // anonymous namespace

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

Return<Status> Light::setLight(Type type, const LightState& state) {
    LightStateHandler handler = nullptr;

    /* Lock global mutex until light state is updated. */
    std::lock_guard<std::mutex> lock(globalLock);

    /* Update the cached state value for the current type. */
    for (LightBackend& backend : backends) {
        if (backend.type == type) {
            backend.state = state;
            handler = backend.handler;
        }
    }

    /* If no handler has been found, then the type is not supported. */
    if (!handler) {
        return Status::LIGHT_NOT_SUPPORTED;
    }

    /* Light up the type with the highest priority that matches the current handler. */
    for (LightBackend& backend : backends) {
        if (handler == backend.handler && isLit(backend.state)) {
            handler(backend.state);
            return Status::SUCCESS;
        }
    }

    /* If no type has been lit up, then turn off the hardware. */
    handler(state);

    return Status::SUCCESS;
}

Return<void> Light::getSupportedTypes(getSupportedTypes_cb _hidl_cb) {
    std::vector<Type> types;

    for (const LightBackend& backend : backends) {
        types.push_back(backend.type);
    }

    _hidl_cb(types);

    return Void();
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android
