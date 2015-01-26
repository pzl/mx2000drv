#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include "usb.h"

static int _is_mx(libusb_device *);

libusb_device_handle *handle;

int initialize_usb(void) {
	int ret;

	ret = libusb_init(NULL);
	if (ret < 0){
		return ret;
	}

	libusb_set_debug(NULL, 3);

	if (libusb_has_capability(LIBUSB_CAP_HAS_CAPABILITY)){
		if (libusb_has_capability(LIBUSB_CAP_HAS_HID_ACCESS) == 0){
			fprintf(stderr, "Cannot access HID devices on this platform\n");
			return -1;
		}
		if (libusb_has_capability(LIBUSB_CAP_SUPPORTS_DETACH_KERNEL_DRIVER) == 0) {
			fprintf(stderr, "Warn: may not be able to detach kernel driver\n");
		}
		ret = libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG);
		printf("hotplug support: %d\n", ret);
	}

	return 0;
}


int find_device(void) {
	libusb_device **list;
	libusb_device *mx = NULL;
	ssize_t count,
			i;
	int err;

	if (handle != NULL){
		fprintf(stderr, "Already active device\n");
		return -2;
	}

	count = libusb_get_device_list(NULL, &list);
	if (count < 0){
		fprintf(stderr, "Problem enumerating USB devices\n");
		return -1;
	}

	for (i=0; i < count; i++){
		if (_is_mx(list[i])){
			mx = list[i];
			break;
		}
	}

	if (mx) {
		if (libusb_open(mx, &handle)){
			fprintf(stderr, "Error opening mx device\n");
			return -1;
		}
		if (libusb_set_auto_detach_kernel_driver(handle, 1) != LIBUSB_SUCCESS) {
			fprintf(stderr, "Error setting automatic kernel detachment\n");
		}
	}
	printf("Opened mx device!\n");

	if (libusb_kernel_driver_active(handle, MX_CONTROL_INTERFACE) == 1){
		err = libusb_detach_kernel_driver(handle, MX_CONTROL_INTERFACE);
		if (err < 0){
			fprintf(stderr, "error detaching kernel driver: %s\n", libusb_strerror(err));
		}
	}

	err = libusb_claim_interface(handle, MX_CONTROL_INTERFACE);
	if (err < 0){
		fprintf(stderr, "error claiming interface %s\n", libusb_strerror(err));
	}
	

	libusb_free_device_list(list, 1);
	return 0;
}

/*
	Sends command to device
	buf: unsigned char[8] - command to send

	returns: 0 on success
			negative for err
*/
int send_command(unsigned char *buf) {
	int err;

	/* Make sure we have a valid device handle */
	if (handle == NULL){
		fprintf(stderr, "No device open for controlling\n");
		return -1;
	}

	err = libusb_control_transfer(handle,
	        LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, /* bmRequestType 0x21 */
	        0x09,													/* bRequest */
	        0x02b3,													/* wValue */
	        2,														/* wIndex */
	        buf, 
	        8,														/* data length */
	        0);														/* timeout */
	if (err < 0){
		printf("control error: %s\n", libusb_strerror(err));
		return -2;
	}
	return 0;
}

int read_back(unsigned char *buf){
	int err,
		nbytes_read;


	err = libusb_interrupt_transfer(handle,
	        LIBUSB_ENDPOINT_IN | 3,
	        buf, 8,
	        &nbytes_read, 1000);

	if (err < 0){
		printf("readback error: %s\n", libusb_strerror(err));
		return -1;
	}

	if (nbytes_read < 8){
		fprintf(stderr, "Possible data underflow when receiving profile change acknowledgment. Got %d bytes back\n", nbytes_read);
		return -2;
	}

	return 0;

}

void finish_usb(void) {
	libusb_release_interface(handle,MX_CONTROL_INTERFACE);
	libusb_attach_kernel_driver(handle,MX_CONTROL_INTERFACE);
	libusb_close(handle);
	libusb_exit(NULL);
}

static int _is_mx(libusb_device *device) {
	struct libusb_device_descriptor desc;

	if (libusb_get_device_descriptor(device, &desc)){
		fprintf(stderr, "Error getting device desc\n");
		return 0;
	}
	if (desc.idVendor == MX_VENDOR_ID && desc.idProduct == MX_PRODUCT_ID) {
		return 1;
	}

	return 0;
}
