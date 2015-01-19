#ifndef MX_USB_H
#define MX_USB_H

#define MX_VENDOR_ID 0x1ea7
#define MX_PRODUCT_ID 0x002c
#define MX_CONTROL_INTERFACE 2

int initialize_usb(void);
void finish_usb(void);

int find_device(void);

int change_profile(unsigned char profile);


#endif
