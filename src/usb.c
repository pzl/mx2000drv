#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include "usb.h"

static int _is_mx(libusb_device *);
static void print_bytes(unsigned char *, int n);

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

int change_profile(unsigned char profile) {
	int err, nbytes_read;
	unsigned char control_data[8] = {
		0xb3, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	unsigned char data_read[8];


	/* Make sure we have a valid device handle */
	if (handle == NULL){
		fprintf(stderr, "No device open for controlling\n");
		return -1;
	}

	/* set profile choice in USB packet data */
	control_data[3] = profile;


	/* Send profile choice USB packet */
	err = libusb_control_transfer(handle,
	                        LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
	                        0x09,
	                        0x02b3, 2,
	                        control_data, 8,
	                        0);
	if (err < 0){
		printf("control error: %s\n", libusb_strerror(err));
	}


	/* Receive info back on profile change */
	err = libusb_interrupt_transfer(handle,
	                               LIBUSB_ENDPOINT_IN | 3,
	                               data_read, 8,
	                               &nbytes_read, 1000);
	if (err < 0){
		printf("readback error: %s\n", libusb_strerror(err));
	}

	if (nbytes_read < 8){
		fprintf(stderr, "Possible data underflow when receiving profile change acknowledgment. Got %d bytes back\n", nbytes_read);
	}


	/* check constant bits for typical packet structure */
	if ( data_read[0] != 0xb3 ||
	     data_read[1] != 0x20 ||
	     data_read[2] != 0x00 ||
	     (data_read[4] & 0x0F) != 0x03 ||
	     data_read[6] != 0x0e ||
	     data_read[7] != 0x0F) {
		printf("Unknown return data format when changing profile: ");
		print_bytes(data_read, 8);
		printf("\n");
	}

	if (data_read[3] != 0x0F || data_read[5] != 0x00) {
		printf("Special profile bits were set: ");
		print_bytes(data_read, 8);
		printf("\n");
	}

	if ((data_read[4] >> 4) != profile){
		fprintf(stderr, "Error: mouse did not acknowledge correct profile change");
		print_bytes(data_read, 8);
		printf("\n");
	} else {
		printf("Profile changed to %d\n", profile);
	}

	return err;

}

void finish_usb(void) {
	libusb_release_interface(handle,2);
	/*libusb_attach_kernel_driver(handle,2);*/
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

static void print_bytes(unsigned char *bytes, int n){
	int i;
	printf("0x");
	for (i=0; i<n; i++){
		printf("%02x", bytes[i]);
	}
}
