#ifndef USERPROG_SYSCALL_H

#define USERPROG_SYSCALL_H
#define SYSCALL_ARGS_NUM_LIMITS 4

#include "lib/user/syscall.h"

typedef int pid_t;

void check_valid_address(const void *addr);

void syscall_init (void);
void syscall_halt (void);
void syscall_exit (int status);
pid_t syscall_exec (const char *file);
int syscall_wait (pid_t pid);
bool syscall_create (const char *file, unsigned initial_size);
bool syscall_remove (const char *file);
int syscall_open (const char *file);
int syscall_filesize (int fd);
int syscall_read (int fd, void *buffer, unsigned length);
int syscall_write (int fd, const void *buffer, unsigned length);
void syscall_seek (int fd, unsigned position);
unsigned syscall_tell (int fd);
void syscall_close (int fd);

int syscall_fibonacci (int n);
int syscall_max_of_four_int (int a, int b, int c, int d);

#endif /* userprog/syscall.h */
