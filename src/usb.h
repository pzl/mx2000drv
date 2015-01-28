#ifndef MX_USB_H
#define MX_USB_H

#define MX_VENDOR_ID 0x1ea7
#define MX_PRODUCT_ID 0x002c
#define MX_CONTROL_INTERFACE 2


#define ERR_NO_HID -1
#define ERR_KERNEL_DRIVER -2
#define ERR_GET_DEVICE -3
#define ERR_NO_DEVICE -4
#define ERR_CMD -5

int initialize_usb(void);
void finish_usb(void);

int find_device(void);

int send_command(unsigned char *buf);
int read_back(unsigned char *buf);


#endif
