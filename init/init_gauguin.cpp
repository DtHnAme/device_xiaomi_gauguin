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
#include <vector>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/_system_properties.h>

#include "property_service.h"
using android::base::GetProperty;

void property_override(char const prop[], char const value[], bool add = true){
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

/* From Magisk@jni/magiskhide/hide_utils.c */
static const char *snet_prop_key[] = {
    "ro.boot.vbmeta.device_state",
    "ro.boot.verifiedbootstate",
    "ro.boot.flash.locked",
    "ro.boot.selinux",
    "ro.boot.veritymode",
    "ro.boot.warranty_bit",
    "ro.warranty_bit",
    "ro.debuggable",
    "ro.secure",
    "ro.build.type",
    "ro.build.tags",
    "ro.build.selinux",
    NULL
};

 static const char *snet_prop_value[] = {
    "locked",
    "green",
    "1",
    "enforcing",
    "enforcing",
    "0",
    "0",
    "0",
    "1",
    "user",
    "release-keys",
    "1",
    NULL
};

 static void workaround_snet_properties() {

     // Hide all sensitive props
    for (int i = 0; snet_prop_key[i]; ++i) {
        property_override(snet_prop_key[i], snet_prop_value[i]);
    }
}

std::vector<std::string> ro_props_default_source_order = {
        "", "bootimage.", "odm.", "product.", "system.", "system_ext.", "vendor.",
};

void set_ro_build_prop(const std::string& prop, const std::string& value) {
    for (const auto& source : ro_props_default_source_order) {
        auto prop_name = "ro." + source + "build." + prop;
        if (source == "")
            property_override(prop_name.c_str(), value.c_str());
        else
            property_override(prop_name.c_str(), value.c_str(), false);
    }
};

void set_ro_product_prop(const std::string& prop, const std::string& value) {
    for (const auto& source : ro_props_default_source_order) {
        auto prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str(), false);
    }
};

void vendor_load_properties() {
    std::string region;
    std::string hardware_revision;
    region = GetProperty("ro.boot.hwc", "GLOBAL");
    hardware_revision = GetProperty("ro.boot.hwversion", "UNKNOWN");
    
    std::string model;
    std::string device;
    std::string fingerprint;
    std::string description;
    std::string mod_device;

    if (region == "GLOBAL") {
        model = "Mi 10T Lite";
        device = "gauguin";
        fingerprint =
                "google/redfin/redfin:11/RQ2A.210305.006/7119741:user/release-keys";
        description = "redfin-user 11 RQ2A.210305.006 7119741 release-keys";
        mod_device = "M2007J17G";
    } else if (region == "CN") {
        model = "Redmi Note 9 Pro";
        device = "gauguinpro";
        fingerprint =
                "google/redfin/redfin:11/RQ2A.210305.006/7119741:user/release-keys";
        description = "redfin-user 11 RQ2A.210305.006 7119741 release-keys";
        mod_device = "M2007J17C";
    } else if (region == "INDIA") {
        model = "Mi 10i";
        device = "gauguininpro";
        fingerprint =
                "google/redfin/redfin:11/RQ2A.210305.006/7119741:user/release-keys";
        description = "redfin-user 11 RQ2A.210305.006 7119741 release-keys";
        mod_device = "M2007J17I	";
    }
    // Workaround SafetyNet
    workaround_snet_properties();
    
    
    set_ro_build_prop("fingerprint", fingerprint);
    set_ro_product_prop("device", device);
    set_ro_product_prop("model", model);
    property_override("ro.build.description", description.c_str());
    if (mod_device != "") {
        property_override("ro.product.mod_device", mod_device.c_str());
    }

    property_override("ro.boot.hardware.revision", hardware_revision.c_str()); 
}
