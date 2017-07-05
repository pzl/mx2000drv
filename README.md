kairos' notes:
--------------
I don't know a whole lot about the development world or GitHub, so if I've done anything unorthodox here, please let me know what I can do to fix this. As of right now, the macro function has been added with basic functionality. It takes input using fgets() then converts the ASCII over to USB scancodes with a "dictionary function" I wrote.
```
Syntax: mx2000drv macro (button number)

kairos@defrag:~$ mx2000drv macro 8
Enter any characters from [a-z, A-Z, 0-9, or other chars] to assign. (20 chars. max.)
macro: ABCdefGHI
Macro entry is: ABCdefGHI(end)
Macro entry end: I(end)
Macro length is: 9
Button 8: 0xa, 0x92
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
Warn: mouse profile may have changed, but got unknown response
kairos@defrag:~$ 
```
THE ABOVE CODE SEEMS TO BE NORMAL. THE MACRO FUNCTION DOES INDEED WORK DESPITE THIS WARNING MESSAGE BEING PRINTED!
note: Notice that "macro length" is "9" and the warning message appeared "9" times. This is printed once for each address (key in macro) written.

-------------
pzl's readme:
-------------

**mx2000drv** is a Linux driver for the Perixx MX-2000 series gaming mice. It is capable of setting all the configurations for the mouse (remapping buttons, backlight colors and times, macros, sensitivity, DPI) and saving to profiles in the onboard memory.

Installation
----------------

**Dependencies**: **[HIDAPI](http://www.signal11.us/oss/hidapi/)** for communicating with the mouse. Version 0.5.0 and below used only libusb, but it was discovered this was [not compatible with Mac](http://www.libusb.org/ticket/89) when using HID devices (including this mouse). 

On Mac, you must install a very recent version of HIDAPI, probably using the [github](https://github.com/signal11/hidapi) version and installing from source. 0.8.0 does not include the changes necessary to attach to the right USB interface of the mouse.

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

Usage
-----

**mx2000drv** [OPTIONS] COMMAND [ARGUMENTS]

## Options
- `-h`, `--help`
- `-p`, `--profile`=_PROFILE_
     There are 4 mouse configurations stored in memory, one of them is active at any given time. Use this option to target a particular mouse profile to read from or change. When not specified, assumes the current active configuration.
- `-v`, `--verbose`
- `-V`, `--version`

## Commands
### Configuration Commands
inspecting or changing mouse profile settings. Provide all arguments to change a setting to be that value. Leave off the final optional argument to retrieve the current value of that setting.

- **profile** [`PROFILE`]  
    The active mouse profile, an integer 1-4. The `-p` option is ignored for this setting.
- **button** `NUM` [`ASSIGNMENT`]  
    Map the specified button number to perform `ASSIGNMENT` when clicked. See the man page for the assignment of mouse buttons to numbers.
- **macro** `NUM` [`SEQUENCE`]  
    Map button to perform key `SEQUENCE` when clicked. See the man page for the assignment of mouse buttons to numbers.
- **color** [`RGB`]  
    Mouse backlight color as 6-character hexadecimal. Mostly. When breathe is enabled, each pair of hex characters also controls the fade cycle period of each LED color. So mixed values will end up with a bit of a color cycle between them. Compare `00FFFF` with `0084FF`
- **breathe** [`ON`|`OFF`]  
    Whether the mouse LEDs fade and pulse over time
- **cycle** [`ON`|`OFF`]  
    Cycle through the RGB combinations, including intermediates (yellow, cyan, magenta), only for the LEDs enabled by **color**. So `FFFFFF` will cycle through all colors and intermediates. `00FFFF` will not show red, or magenta, but will show all others.
- **lit** [`TIME`]  
    Number of seconds the backlight is steady on when **breathe** is enabled. Min: 0, Max: 15
- **dark** [`TIME`]  
    Number of seconds the backlight is off when **breathe** is enabled. Min: 0, Max: 15
- **pulse** [`TIME`]  
    Number of seconds the backlight takes to transition from lit to dark, when **breathe** is enabled. Min: 0, Max: 63
- **standby** [`TIME`]  
    Enable backlight `TIME` seconds after standby mode has come on. Min: 0, Max: 63
- **backlight** [`ON`|`OFF`]  
    Enable or disable the backlight mouse LEDs
- **sensitivity** `X`|`Y` [`VALUE`]  
    The pointer sensitivity, a value between -5 and 5, inclusive.
- **accel** [`SPEED`]  
    The mouse acceleration in milliseconds. Min: 0, Max: 255.
- **dpi** `PRESET` [`VALUE`]  
    Sets the resolution of a given DPI preset. `PRESET` must be a number 1-4. `VALUE` must be between 100 and 5600 in increments of 100
- **dpiset** [`PRESET`]  
    Sets or retrieves the active DPI preset (1-4).
- **poll** [`RATE`]  
    Mouse polling rate in Hz: 125, 250, 500, 1000

### Bulk Commands

- **reset**  
    Resets all mouse configurations, settings, macros, and profiles to factory default. Similar to a recovery mode.
- **dump** `FILE`  
    Save the entire mouse configuration including all settings, macros, and profiles to a `FILE`. `-` may be provided in place of a file name for stdout.
- **load** `FILE`  
    Load the entire mouse configuration including all settings, macros, and profiles from a `FILE` into memory. `-` may be provided in place of a file name for stdin.

## Examples

Get current profile: `mx2000drv profile`  
Set the current profile to 3: `mx2000drv profile 3`  
Set the backlight of the current profile to blue: `mx2000drv color 0000ff`  
Set the polling rate of profile 2 to 500Hz: `mx2000drv -p 2 poll 500`  
Get profile 1's X sensitivity: `mx2000drv -p 1 sensitivity x`  
Get current acceleration value: `mx2000drv accel`  
Save current mouse settings to file: `mx2000drv dump mem.bin`  
Load settings into mouse: `mx2000drv load mem.bin`  
Pipe current mouse settings into a hex viewer: `mx2000drv dump - | xxd`

Notes
---------

* The `button` and `macro` commands are **not yet** complete. See [Issue #3](https://github.com/pzl/mx2000drv/issues/3) for status. As of writing, the `button` command accepts a 4-character hex sequence as input that corresponds to the USB keyboard scan codes for a key. This is not going to be the final interface. The `macro` command is entirely non-functional.
* This is a userspace driver, not for use as part of the kernel
* This driver is only for settings configuration. It does not interact with the pointer or keyboard interfaces of the mouse. These will continue to use the default HID driver.

Contributing
--------------

Please feel free to open issues with improvements, or suggestions, bugs, etc. Pull requests are also welcome. Opening issues to discuss patches and features first are encouraged.


License
------------
```
MX2000DRV - portable driver for the MX-2000 II gaming mouse
Copyright (C) 2015  Dan Panzarella

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
```


This software and all files included are licensed under the GPL v2. See `doc/LICENSE` for the full terms.
