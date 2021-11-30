/*
 * Copyright 2021 The Imaging Source Europe GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "v4l2_genicam_mapping.h"

using namespace tcam;

namespace
{
//{ , { "", TCAM_PROPERTY_TYPE_UNKNOWN, {}, }, },

static const tcam::v4l2::v4l2_genicam_mapping v4l2_conv_dict[] = {
    // brightness
    {
        0x00980900,
        "BlackLevel",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    // Saturation
    {
        0x00980902,
        "",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    // Hue
    {
        0x00980903,
        "",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    // Gain
    {
        0x00980913,
        "",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    // Gamma
    {
        0x00980910,
        "",
        TCAM_PROPERTY_TYPE_DOUBLE,
        tcam::v4l2::MappingType::Scale,
    },
    // exposure_auto
    {
        0x009a0901,
        "ExposureAuto",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    // exposure absolute
    {
        0x009a0902,
        "ExposureTime",
        TCAM_PROPERTY_TYPE_DOUBLE,
        tcam::v4l2::MappingType::Scale,
    },
    {
        0x0098090c,
        "BalanceWhiteAuto",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x0098090e,
        "BalanceWhiteRed",
        TCAM_PROPERTY_TYPE_DOUBLE,
        tcam::v4l2::MappingType::Scale,
    },
    {
        0x0098090f,
        "BalanceWhiteBlue",
        TCAM_PROPERTY_TYPE_DOUBLE,
        tcam::v4l2::MappingType::Scale,
    },
    // privacy aka trigger
    {
        0x009a0910,
        "TriggerMode",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x009a090a,
        "Focus",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    // Exposure Time (us)
    {
        0x199e201,
        "ExposureTime",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    // Auto Shutter
    {
        0x199e202,
        "ExposureAuto",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x199e203,
        "ExposureAutoReference",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x199e204,
        "Gain",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    // Gain Auto
    {
        0x199e205,
        "GainAuto",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x199e207,
        "BalanceWhitePreset",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x199e208,
        "TriggerMode",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x0199e209,
        "TriggerSoftware",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e210,
        "TriggerDelay",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    {
        0x0199e211,
        "StrobeEnable",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x0199e212,
        "StrobePolarity",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x0199e213,
        "StrobeOperation",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e214,
        "StrobeDuration",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e215,
        "StrobeDelay",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e216,
        "GPOut",
        TCAM_PROPERTY_TYPE_INTEGER,
    },
    {
        0x0199e217,
        "GPIn",
        TCAM_PROPERTY_TYPE_INTEGER,
    },
    {
        0x0199e218,
        "OffsetX",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e219,
        "OffsetY",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e220,
        "OffsetAutoCenter",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x0199e921,
        "BalanceWhiteRed",
        TCAM_PROPERTY_TYPE_DOUBLE,
        tcam::v4l2::MappingType::Scale,
    },
    {
        0x0199e922,
        "BalanceWhiteGreen",
        TCAM_PROPERTY_TYPE_DOUBLE,
        tcam::v4l2::MappingType::Scale,
    },
    {
        0x0199e923,
        "BalanceWhiteBlue",
        TCAM_PROPERTY_TYPE_DOUBLE,
        tcam::v4l2::MappingType::Scale,
    },
    {
        0x0199e927,
        "OffsetX",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e928,
        "OffsetY",
    },
    {
        0x0199e234,
        "TriggerActivation",
    },
    {
        0x0199e235,
        "TriggerOperation",
    },
    {
        0x0199e236,
        "TriggerSelector",
    },
    {
        0x0199e237,
        "TriggerBurstCount",
    },
    {
        0x0199e238,
        "TriggerDebouncer",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    {
        0x0199e239,
        "TriggerMask",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    {
        0x0199e240,
        "TriggerDenoise",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    {
        0x0199e241,
        "AutoFunctionsROIEnable",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e242,
        "AutoFunctionsROILeft",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e243,
        "AutoFunctionsROITop",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e244,
        "AutoFunctionsROIWidth",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e245,
        "AutoFunctionsROIHeight",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e246,
        "BalanceWhiteMode",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e247,
        "BalanceWhiteAutoPreset",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e248,
        "BalanceWhiteGreen",
        TCAM_PROPERTY_TYPE_DOUBLE,
        tcam::v4l2::MappingType::Scale,
    },
    {
        0x0199e249,
        "BalanceWhiteTemperaturePreset",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e250,
        "BalanceWhiteTemperature",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e251,
        "ReverseY",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e252,
        "ReverseX",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e253,
        "HighlightReduction",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e254,
        "ExposureAutoUpperLimitAuto",
        TCAM_PROPERTY_TYPE_ENUMERATION,
        tcam::v4l2::MappingType::IntToEnum,
    },
    {
        0x0199e255,
        "ExposureAutoLowerLimit",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    {
        0x0199e256,
        "ExposureAutoUpperLimit",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    {
        0x0199e257,
        "OverrideScanningMode",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e258,
        "AutoFunctionsROIPreset",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
    {
        0x0199e259,
        "GainAutoLowerLimit",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    {
        0x0199e260,
        "GainAutoUpperLimit",
        TCAM_PROPERTY_TYPE_DOUBLE,
    },
    // TriggerGlobalResetRelease
    {
        0x0199e261,
        "TriggerOperation",
        TCAM_PROPERTY_TYPE_ENUMERATION, // Seems to map to TriggerOperation
        tcam::v4l2::MappingType::IntToEnum
    },
    {
        0x0199e262,
        "IMXLowLatencyMode",
        TCAM_PROPERTY_TYPE_UNKNOWN,
    },
};

} // namespace


const struct tcam::v4l2::v4l2_genicam_mapping* tcam::v4l2::find_mapping(uint32_t v4l2_id)
{

    for (unsigned int i = 0; i < sizeof(v4l2_conv_dict) / sizeof(v4l2_genicam_mapping); i++)
    {
        if (v4l2_conv_dict[i].v4l2_id == v4l2_id)
        {
            return &v4l2_conv_dict[i];
        }
    }
    return nullptr;
}
