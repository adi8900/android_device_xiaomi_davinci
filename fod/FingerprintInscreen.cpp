/*
 * Copyright (C) 2019 The LineageOS Project
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

#define LOG_TAG "FingerprintInscreenService"

#include "FingerprintInscreen.h"

#include <android-base/logging.h>
#include <fstream>
#include <cmath>

#define COMMAND_NIT 10
#define PARAM_NIT_FOD 3
#define PARAM_NIT_NONE 0

#define FOD_HBM_PATH "/sys/devices/platform/soc/soc:qcom,dsi-display/fod_hbm"
#define FOD_HBM_ON 1
#define FOD_HBM_OFF 0

#define FOD_STATUS_PATH "/sys/devices/virtual/touch/tp_dev/fod_status"
#define FOD_STATUS_ON 1
#define FOD_STATUS_OFF 0

#define FOD_SENSOR_X 445
#define FOD_SENSOR_Y 1931
#define FOD_SENSOR_SIZE 190

namespace {

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

} // anonymous namespace

namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_0 {
namespace implementation {

FingerprintInscreen::FingerprintInscreen() {
    xiaomiFingerprintService = IXiaomiFingerprint::getService();
}

Return<int32_t> FingerprintInscreen::getPositionX() {
    return FOD_SENSOR_X;
}

Return<int32_t> FingerprintInscreen::getPositionY() {
    return FOD_SENSOR_Y;
}

Return<int32_t> FingerprintInscreen::getSize() {
    return FOD_SENSOR_SIZE;
}

Return<void> FingerprintInscreen::onStartEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onFinishEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onPress() {
    set(FOD_HBM_PATH, FOD_HBM_ON);
    xiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_FOD);
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    set(FOD_HBM_PATH, FOD_HBM_OFF);
    xiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_NONE);
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    set(FOD_STATUS_PATH, FOD_STATUS_ON);
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    set(FOD_STATUS_PATH, FOD_STATUS_OFF);
    set(FOD_HBM_PATH, FOD_HBM_OFF);
    xiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_NONE);
    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t acquiredInfo, int32_t vendorCode) {
    LOG(ERROR) << "acquiredInfo: " << acquiredInfo << ", vendorCode: " << vendorCode << "\n";
    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t error, int32_t vendorCode) {
    LOG(ERROR) << "error: " << error << ", vendorCode: " << vendorCode << "\n";
    return false;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t brightness) {
    float alpha;

    if (brightness > 62) {
        alpha = 1.0 - pow(brightness / 255.0 * 430.0 / 600.0, 0.45);
    } else {
        alpha = 1.0 - pow(brightness / 200.0, 0.45);
    }

    return 255 * alpha;
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}

Return<void> FingerprintInscreen::setCallback(const sp<::vendor::lineage::biometrics::fingerprint::inscreen::V1_0::IFingerprintInscreenCallback>& callback) {
    (void) callback;
    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor
