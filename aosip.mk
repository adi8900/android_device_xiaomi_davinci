#
# Copyright (C) 2019 The AOSIP Project
#
# SPDX-License-Identifier: Apache-2.0
#

$(call inherit-product, device/xiaomi/davinci/device.mk)

# Inherit some common AOSIP stuff.
$(call inherit-product, vendor/aosip/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_BRAND := Xiaomi
PRODUCT_DEVICE := davinci
PRODUCT_MANUFACTURER := Xiaomi
PRODUCT_NAME := aosip_davinci

BUILD_FINGERPRINT := "Xiaomi/davinciin/davinciin:9/PKQ1.181121.001/V10.3.3.0.PFKINXM:user/release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi
