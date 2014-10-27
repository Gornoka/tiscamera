

#include "v4l2_utils.h"

#include "standard_properties.h"

#include "utils.h"
#include "logging.h"
#include "Error.h"

#if HAVE_UDEV
#include <libudev.h>
#endif

#include <glob.h>

#include <vector>
#include <algorithm>

using namespace tcam;

struct v4l2_property
{
    PROPERTY_ID id;
    std::vector<int> v4l2_id;
};


static std::vector<struct v4l2_property> v4l2_mappings =
{
    {
        .id = PROPERTY_INVALID,
        .v4l2_id = {},
    },
    {
        .id = PROPERTY_EXPOSURE,
        .v4l2_id = { V4L2_CID_EXPOSURE_ABSOLUTE, V4L2_CID_EXPOSURE },
    },
    {
        .id = PROPERTY_EXPOSURE_AUTO,
        .v4l2_id = { V4L2_CID_AUTO_EXPOSURE_BIAS },
    },
    {
        .id = PROPERTY_GAIN,
        .v4l2_id = { V4L2_CID_GAIN },
    },
    {
        .id = PROPERTY_GAIN_RED,
        .v4l2_id = { 0x980921 },
    },
    {
        .id = PROPERTY_GAIN_GREEN,
        .v4l2_id = {  0x980922 },
    },
    {
        .id = PROPERTY_GAIN_BLUE,
        .v4l2_id = {  0x980923 },
    },
    {
        .id = PROPERTY_GAIN_AUTO,
        .v4l2_id = { 0 },
    },
    {
        .id = PROPERTY_TRIGGER_MODE,
        .v4l2_id = { V4L2_CID_PRIVACY, 0x0199e208},
    },
    {
        .id = PROPERTY_TRIGGER_SOURCE,
        .v4l2_id = {0},
    },
    {
        .id = PROPERTY_TRIGGER_ACTIVATION,
        .v4l2_id = {0},
    },
    {
        .id = PROPERTY_SOFTWARETRIGGER,
        .v4l2_id = {/* usb 2: */ 0x980925, /* usb 3: */ 0x0199e209},
    },
    {
        .id = PROPERTY_GPIO,
        .v4l2_id = {0x0199e217},
    },
    {
        .id = PROPERTY_GPIN,
        .v4l2_id = {0},
    },
    {
        .id = PROPERTY_GPOUT,
        .v4l2_id = {0x0199e216},
    },
    {
        .id = PROPERTY_OFFSET_X,
        .v4l2_id = {0x00980927 /*usb2*/, 0x0199e218 /*usb3*/},
    },
    {
        .id = PROPERTY_OFFSET_Y,
        .v4l2_id = {0x00980928 /*usb2*/, 0x0199e219/*usb3*/},
    },
    {
        .id = PROPERTY_OFFSET_AUTO,
        .v4l2_id = {0x0199e220},
    },
    {
        .id = PROPERTY_BRIGHTNESS,
        .v4l2_id = {V4L2_CID_BRIGHTNESS},
    },
    {
        .id = PROPERTY_CONTRAST,
        .v4l2_id = {V4L2_CID_CONTRAST},
    },
    {
        .id = PROPERTY_SATURATION,
        .v4l2_id = {V4L2_CID_SATURATION},
    },
    {
        .id = PROPERTY_HUE,
        .v4l2_id = {V4L2_CID_HUE},
    },
    {
        .id = PROPERTY_GAMMA,
        .v4l2_id = {V4L2_CID_GAMMA},
    },
    {
        .id = PROPERTY_WB_AUTO,
        .v4l2_id = {},
    },
    {
        .id = PROPERTY_IRCUT,
        .v4l2_id = {},
    },
    {
        .id = PROPERTY_IRIS,
        .v4l2_id = {},
    },
    {
        .id = PROPERTY_FOCUS,
        .v4l2_id = {V4L2_CID_FOCUS_ABSOLUTE},
    },
    {
        .id = PROPERTY_ZOOM,
        .v4l2_id = {V4L2_CID_ZOOM_ABSOLUTE},
    },
    {
        .id = PROPERTY_FOCUS_AUTO,
        .v4l2_id = {},
    },
    {
        .id = PROPERTY_STROBE_ENABLE,
        .v4l2_id = {0x0199e211},
    },
};


uint32_t tcam::convertV4L2flags (uint32_t v4l2_flags)
{
    uint32_t internal_flags = 0;

    if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_DISABLED))
    {
        internal_flags = set_bit(internal_flags, PROPERTY_FLAG_DISABLED);
    }
    if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_GRABBED))
    {
        internal_flags = set_bit(internal_flags, PROPERTY_FLAG_GRABBED);
    }
    if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_READ_ONLY))
    {
        internal_flags = set_bit(internal_flags, PROPERTY_FLAG_READ_ONLY);
    }
    if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_UPDATE))
    {}
    if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_INACTIVE))
    {
        internal_flags = set_bit(internal_flags, PROPERTY_FLAG_INACTIVE);
    }
    if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_SLIDER))
    {}
    if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_WRITE_ONLY))
    {
        internal_flags = set_bit(internal_flags, PROPERTY_FLAG_WRITE_ONLY);
    }
    if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_VOLATILE))
    {}
    // if (is_bit_set(v4l2_flags, V4L2_CTRL_FLAG_HAS_PAYLOAD))
    // {}

    return internal_flags;
}


static PROPERTY_ID find_mapping (int v4l2_id)
{
    auto f = [v4l2_id] (int p)
        {
            if (v4l2_id == p)
                return true;
            return false;
        };

    for (const auto& m : v4l2_mappings)
    {
        auto match = std::find_if(m.v4l2_id.begin(), m.v4l2_id.end(), f);

        if (match != m.v4l2_id.end())
            return m.id;
    }
    return PROPERTY_INVALID;
}


std::shared_ptr<Property> tcam::createProperty (int fd,
                                                       struct v4l2_queryctrl* queryctrl,
                                                       struct v4l2_ext_control* ctrl,
                                                       std::shared_ptr<PropertyImpl> impl)
{

    // assure we have the typ
    Property::VALUE_TYPE type;

    switch (queryctrl->type)
    {
        case V4L2_CTRL_TYPE_BOOLEAN:
        {
            type = Property::BOOLEAN;
            break;
        }
        case V4L2_CTRL_TYPE_INTEGER:
        {
            type = Property::INTEGER;
            break;
        }
        case V4L2_CTRL_TYPE_STRING:
        {
            type = Property::STRING;
            break;
        }
        case V4L2_CTRL_TYPE_INTEGER_MENU:
        {
            type = Property::ENUM;
            break;
        }
        case V4L2_CTRL_TYPE_BUTTON:
        {
            type = Property::BUTTON;
            break;
        }
        default:
        {
            setError(Error("undefined property type", ENOENT));
            type = Property::UNDEFINED;
            break;
        }
    }

    auto prop_id = find_mapping (ctrl->id);

    auto ctrl_m = get_control_reference(prop_id);

    PROPERTY_TYPE type_to_use;
    camera_property cp = {};

    if (ctrl_m.id == PROPERTY_INVALID)
    {
        tcam_log(TCAM_LOG_WARNING, "Unable to find std property. Passing raw property identifier through. '%s'(%x)", (char*)queryctrl->name, queryctrl->id);
        // pass through and do not associate with anything existing
        type_to_use = value_type_to_ctrl_type(type);
        memcpy(cp.name, (char*)queryctrl->name, sizeof(cp.name));
        cp.type = value_type_to_ctrl_type(type);
    }
    else
    {
        type_to_use = ctrl_m.type_to_use;
        memcpy(cp.name, ctrl_m.name.c_str(), ctrl_m.name.size());
        cp.type = ctrl_m.type_to_use;
    }

    uint32_t flags;
    // simply copy existing flags
    if (queryctrl->flags)
    {
        flags = convertV4L2flags(queryctrl->flags);
    }

    switch (type_to_use)
    {
        case PROPERTY_TYPE_BOOLEAN:
        {
            if (queryctrl->default_value == 0)
            {
                cp.value.b.default_value = false;
            }
            else if (queryctrl->default_value == 1)
            {
                cp.value.b.default_value = true;
            }
            else
            {
                tcam_log(TCAM_LOG_ERROR,
                        "Boolean '%s' has impossible default value: %d Setting to false",
                        cp.name,
                        queryctrl->default_value);
                cp.value.b.default_value = false;
            }

            if (ctrl->value == 0)
            {
                cp.value.b.value = false;
            }
            else if (ctrl->value == 1)
            {
                cp.value.b.value = true;
            }
            else
            {
                tcam_log(TCAM_LOG_ERROR,
                        "Boolean '%s' has impossible value: %d Setting to false",
                        cp.name,
                        ctrl->value);
                cp.value.b.value = false;
            }
            cp.flags = flags;

            return std::make_shared<Property>(PropertySwitch(impl, cp, type));
        }
        case PROPERTY_TYPE_INTEGER:
        {
            cp.value.i.min = queryctrl->minimum;
            cp.value.i.max = queryctrl->maximum;
            cp.value.i.step = queryctrl->step;
            cp.value.i.default_value = queryctrl->default_value;
            cp.value.i.value = ctrl->value;
            cp.flags = flags;

            return std::make_shared<Property>(PropertyInteger(impl, cp, type));
        }
        // case TCAM_CTRL_TYPE_DOUBLE:
        // {
        // Does not exist in v4l2
        // }
        case PROPERTY_TYPE_STRING:
        {
            memcpy(cp.value.s.value,(char*)queryctrl->name, sizeof(cp.value.s.value));
            memcpy(cp.value.s.default_value, (char*)queryctrl->name, sizeof(cp.value.s.default_value));
            cp.flags = flags;

            return std::make_shared<Property>(PropertyString(impl, cp, type));
        }
        case PROPERTY_TYPE_STRING_TABLE:
        {
            cp.value.i.min = queryctrl->minimum;
            cp.value.i.max = queryctrl->maximum;
            cp.value.i.step = 0;
            cp.value.i.default_value = queryctrl->default_value;
            cp.value.i.value = ctrl->value;
            cp.flags = flags;

            struct v4l2_querymenu qmenu = {};

            qmenu.id = queryctrl->id;

            std::map<std::string, int> m;

            for (int i = 0; i <= queryctrl->maximum; i++)
            {
                qmenu.index = i;
                if (tcam_xioctl(fd, VIDIOC_QUERYMENU, &qmenu))
                    continue;

                std::string map_string((char*) qmenu.name);
                m.emplace(map_string, i);
            }

            return std::make_shared<Property>(PropertyStringMap(impl, cp, m, type));
        }
        case PROPERTY_TYPE_BUTTON:
        {
            cp.flags = flags;

            return std::make_shared<Property>(PropertyButton(impl, cp, type));
        }
        default:
        {
            std::string s = "Unknown V4L2 Control type: ";
            s.append((char*)queryctrl->name);
            tcam_log(TCAM_LOG_ERROR, s.c_str());
            setError(Error(s, EIO));
            break;
        }
    }
    return nullptr;
}


std::vector<DeviceInfo> tcam::get_v4l2_device_list ()
{
    std::vector<DeviceInfo> device_list;


    struct udev* udev = udev_new();
    if (!udev)
    {
        setError(Error("Unable to create udev reference.", EIO));
        return device_list;
    }

    /* Create a list of the devices in the 'video4linux' subsystem. */
    struct udev_enumerate* enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "video4linux");
    udev_enumerate_scan_devices(enumerate);
    struct udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry* dev_list_entry;

    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char* path;
        char needed_path[100];

        /* Get the filename of the /sys entry for the device
           and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        struct udev_device* dev = udev_device_new_from_syspath(udev, path);

        /* The device pointed to by dev contains information about
           the hidraw device. In order to get information about the
           USB device, get the parent device with the
           subsystem/devtype pair of "usb"/"usb_device". This will
           be several levels up the tree, but the function will find
           it.*/

        /* we need to copy the devnode (/dev/videoX) before the path
           is changed to the path of the usb device behind it (/sys/class/....) */
        strcpy(needed_path, udev_device_get_devnode(dev));

        dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");

        if (!dev)
        {
            setError(Error("udev_device_get_parent_with_subsystem_devtype failed", EIO));
            return device_list;
        }

        /* From here, we can call get_sysattr_value() for each file
           in the device's /sys entry. The strings passed into these
           functions (idProduct, idVendor, serial, etc.) correspond
           directly to the files in the directory which represents
           the USB device. Note that USB strings are Unicode, UCS2
           encoded, but the strings returned from
           udev_device_get_sysattr_value() are UTF-8 encoded. */

        // TODO: no hard coded numbers find more general approach

        static const char* TCAM_VENDOR_ID_STRING = "199e";

        if (strcmp(udev_device_get_sysattr_value(dev, "idVendor"), TCAM_VENDOR_ID_STRING) == 0)
        {
            tcam_device_info info = {};
            info.type = TCAM_DEVICE_TYPE_V4L2;
            strcpy(info.identifier, needed_path);
            strcpy(info.name, udev_device_get_sysattr_value(dev, "product"));
            strcpy(info.serial_number, udev_device_get_sysattr_value(dev, "serial"));

            device_list.push_back(DeviceInfo(info));
        }

        udev_device_unref(dev);
    }

    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    udev_unref(udev);

    return device_list;
}
