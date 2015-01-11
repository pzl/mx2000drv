#ifndef MX_EVENT_H
#define MX_EVENT_H

int register_device(void);
void destroy_device(int fd);
int write_character(int fd, int c);

#endif
