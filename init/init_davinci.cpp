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

#include <android-base/logging.h>
#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::init::property_set;

void property_override(char const prop[], char const value[])
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void property_override_dual(char const system_prop[], char const vendor_prop[],
    char const value[])
{
    property_override(system_prop, value);
    property_override(vendor_prop, value);
}

void load_davinciglobal() {
    property_override("ro.product.model", "Mi 9T");
    property_override("ro.build.product", "davinci");
    property_override("ro.product.device", "davinci");
    property_override("ro.build.description", "davinci-user 9 PKQ1.190302.001 V10.3.15.0.PFJEUXM release-keys");
    property_override_dual("ro.build.fingerprint", "ro.vendor.build.fingerprint", "google/walleye/walleye:8.1.0/OPM1.171019.011/4448085:user/release-keys");
}

void load_davinciin() {
    property_override("ro.product.model", "Redmi K20");
    property_override("ro.build.product", "davinciin");
    property_override("ro.product.device", "davinciin");
    property_override("ro.build.description", "davinciin-user 9 PKQ1.190302.001 V10.3.15.0.PFJINXM release-keys");
    property_override_dual("ro.build.fingerprint", "ro.vendor.build.fingerprint", "google/walleye/walleye:8.1.0/OPM1.171019.011/4448085:user/release-keys");
}

void load_davinci() {
    property_override("ro.product.model", "Redmi K20");
    property_override("ro.build.product", "davinci");
    property_override("ro.product.device", "davinci");
    property_override("ro.build.description", "davinci-user 9 PKQ1.190302.001 V10.3.15.0.PFJCNXM release-keys");
    property_override_dual("ro.build.fingerprint", "ro.vendor.build.fingerprint", "google/walleye/walleye:8.1.0/OPM1.171019.011/4448085:user/release-keys");
}


void vendor_load_properties() {
    std::string region = android::base::GetProperty("ro.boot.hwc", "");

    if (region.find("CN") != std::string::npos) {
        load_davinci();
    } else if (region.find("INDIA") != std::string::npos) {
        load_davinciin();
    } else if (region.find("GLOBAL") != std::string::npos) {
        load_davinciglobal();
    } else {
        LOG(ERROR) << __func__ << ": unexcepted region!";
    }
}
