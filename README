**mx2000drv** is a Linux driver for the Perixx 2000IIB gaming mouse. It is capable of setting all the configurations for the mouse (remapping buttons, backlight colors and times, macros, sensitivity, DPI) and saving to profiles in the onboard memory.

Installation
----------------

**Dependencies**: **[libusb](http://libusb.info/)** for communicating with the mouse. You must have libusb development headers installed for your platform

1. `make`
2. `sudo make install`

To use this right after installing, you should do *one* of the following (to use the driver without being root):

* Unplug and re-plug-in the mouse
* Restart
* run:

```
sudo udevadm control --reload-rules
sudo udevadm trigger
```




Notes
---------
 
* This project is written in ANSI (C89) C.  I have not actively tried to compile it using MSVC, so a few things may need to be changed to get this to compile on Windows or other platforms. But it should be minimal effort. It is designed to be fairly cross-platform.
* This is a userspace driver, not for use as part of the kernel
* This driver is only for settings configuration. It does not interact with the pointer or keyboard interfaces of the mouse. These will continue to use the default HID driver.

Contributing
--------------

Please feel free to open issues with improvements, or suggestions, bugs, etc. Pull requests are also welcome. Opening issues to discuss patches and features first are encouraged.