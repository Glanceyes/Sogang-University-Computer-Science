#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

struct parsed_args{
  char *argv[256];
  int argc;
};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

void get_command_from_file_name(char *file_name);
void parse_file_name(char* file_name, struct parsed_args *args);
void construct_stack(struct parsed_args *args, void **esp);


#endif /* userprog/process.h */
