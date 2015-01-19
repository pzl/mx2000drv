#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include "usb.h"

static int _is_mx(libusb_device *);
static void device_info(libusb_device *);

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


void find_device(void) {
	libusb_device **list;
	libusb_device *mx = NULL;
	libusb_device_handle *handle;
	ssize_t count,
			i;

	count = libusb_get_device_list(NULL, &list);
	if (count < 0){
		fprintf(stderr, "Problem enumerating USB devices\n");
		return;
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
			return;
		}
		if (libusb_set_auto_detach_kernel_driver(handle, 1) != LIBUSB_SUCCESS) {
			fprintf(stderr, "Error setting automatic kernel detachment\n");
		}
	}
	printf("Opened mx device!\n");

	device_info(mx);

	printf("kernel driver on interface 0: %d\n", libusb_kernel_driver_active(handle, 0));
	printf("kernel driver on interface 1: %d\n", libusb_kernel_driver_active(handle, 1));
	printf("kernel driver on interface 2: %d\n", libusb_kernel_driver_active(handle, 2));


	libusb_free_device_list(list, 1);
	libusb_close(handle);
	return;
}

void finish_usb(void) {
	libusb_exit(NULL);
}

static int _is_mx(libusb_device *device) {
	struct libusb_device_descriptor desc;

	if (libusb_get_device_descriptor(device, &desc)){
		fprintf(stderr, "Error getting device desc\n");
		return 0;
	}
	if (desc.idVendor == MX_VENDOR_ID && desc.idProduct == MX_VENDOR_PRODUCT) {
		return 1;
	}

	return 0;
}


static void device_info(libusb_device *device) {
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor *config;
	ssize_t i, j, k, m;

	if (libusb_get_device_descriptor(device, &desc)){
		fprintf(stderr, "Error getting device desc\n");
		return;
	}

	printf("\n\nNum configurations: %d\n"
	       "Device Class: %d\n"
	       "Vendor: 0x%04x "
	       "Product: 0x%04x\n",
	       desc.bNumConfigurations, desc.bDeviceClass,
	       desc.idVendor, desc.idProduct);

	for (i=0; i<desc.bNumConfigurations; i++){
		if (libusb_get_config_descriptor(device,i,&config) != 0){
			fprintf(stderr, "error getting config %d\n", (int)i);
			continue;
		}
		printf("  ↳ Configuration %d (%d)\n"
		       "    Num interfaces: %d\n",
		       (int)i, config->bConfigurationValue,
		       config->bNumInterfaces);
		for (j=0; j<config->bNumInterfaces; j++){
			printf("      ↳ Interface %d\n"
			       "        Num settings: %d\n", 
			       (int)j, config->interface[j].num_altsetting);

			for (k=0; k<config->interface[j].num_altsetting; k++){
				printf("        ↳ Setting %d (%d)\n"
				       "          Num Endpoints: %d\n"
				       "          Interface class: %d\n"
				       "          Interface subclass: %d\n",
				       (int)k,
				       config->interface[j].altsetting[k].bAlternateSetting,
				       config->interface[j].altsetting[k].bNumEndpoints,
				       config->interface[j].altsetting[k].bInterfaceClass,
				       config->interface[j].altsetting[k].bInterfaceSubClass
				       );
				for (m=0; m<config->interface[j].altsetting[k].bNumEndpoints; m++){
					printf("            ↳ Endpoint %d\n"
					       "              Endpoint address: 0x%x\n"
					       "              Attributes: %d\n",
					       (int) m, 
					       config->interface[j].altsetting[k].endpoint[m].bEndpointAddress,
					       config->interface[j].altsetting[k].endpoint[m].bmAttributes
					       );
				}
			}
		}
	}

}
