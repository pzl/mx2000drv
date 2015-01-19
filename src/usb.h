#ifndef MX_USB_H
#define MX_USB_H

#define MX_VENDOR_ID 0x1ea7
#define MX_PRODUCT_ID 0x002c

int initialize_usb(void);
void find_device(void);
void finish_usb(void);

#endif
