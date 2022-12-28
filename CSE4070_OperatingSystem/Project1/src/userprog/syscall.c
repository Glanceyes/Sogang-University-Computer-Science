#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "lib/kernel/console.h"
#include "filesys/filesys.h"

static int syscall_num_args[] = {
  [SYS_HALT] = 0,
  [SYS_EXIT] = 1,
  [SYS_EXEC] = 1,
  [SYS_WAIT] = 1,
  [SYS_CREATE] = 2,
  [SYS_REMOVE] = 1,
  [SYS_OPEN] = 1,
  [SYS_FILESIZE] = 1,
  [SYS_READ] = 3,
  [SYS_WRITE] = 3,
  [SYS_SEEK] = 2,
  [SYS_TELL] = 1,
  [SYS_CLOSE] = 1,
  [SYS_FIBONACCI] = 1,
  [SYS_MAX_FOUR_INT] = 4,
};

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void check_valid_address(const void *addr)
{
  if(!is_user_vaddr(addr) || is_kernel_vaddr(addr)){
    syscall_exit(-1);
  }
}

static void
syscall_get_args(int syscall_num, void *args[], void *esp){
  for(int i = 0; i < syscall_num_args[syscall_num]; i++){
    /* Check if the buffer is valid.*/
    check_valid_address((uint32_t *)esp + (i + 1));
    args[i] = (void*)((uint32_t *)esp + (i + 1));
  }
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  /* Debug system call number. */
  // printf("system call number: %d\n", *(uint32_t *)f->esp);

  /* Get system call number. */
  int syscall_num = (int)*(uint32_t *)f->esp;

  /* Get system call arguments. */
  void *args[SYSCALL_ARGS_NUM_LIMITS] = { NULL, NULL, NULL, NULL };
  syscall_get_args(syscall_num, args, f->esp);

  switch(syscall_num) {
    case SYS_HALT:
      syscall_halt();
      break;

    case SYS_EXIT:
      syscall_exit(*(int *)(args[0]));
      break;

    case SYS_EXEC:
      f->eax = syscall_exec((const char *)*(uint32_t*)args[0]);
      break;

    case SYS_WAIT:
      f->eax = syscall_wait((pid_t)*(uint32_t *)args[0]);
      break;

    case SYS_CREATE:
      break;

    case SYS_REMOVE:
      break;

    case SYS_OPEN:
      break;

    case SYS_FILESIZE:
      break;

    case SYS_READ:
      f->eax = syscall_read((int)*(uint32_t *)args[0], (void *)*(uint32_t *)args[1], (unsigned int)*(uint32_t*)args[2]);
      break;

    case SYS_WRITE:
      f->eax = syscall_write((int)*(uint32_t *)args[0], (const void *)*(uint32_t *)args[1], (unsigned int)*(uint32_t *)args[2]);
      break;
    case SYS_SEEK:
      break;

    case SYS_TELL:
      break;

    case SYS_CLOSE:
      break;

    case SYS_MMAP:
      break;

    case SYS_MUNMAP:
      break;

    case SYS_CHDIR:
      break;

    case SYS_MKDIR:
      break;

    case SYS_READDIR:
      break;

    case SYS_ISDIR:
      break;

    case SYS_INUMBER:
      break;

    case SYS_FIBONACCI:
      f->eax = syscall_fibonacci((int)*(uint32_t *)args[0]);
      break;

    case SYS_MAX_FOUR_INT:
      f->eax = syscall_max_of_four_int((int)*(uint32_t *)args[0], (int)*(uint32_t *)args[1], (int)*(uint32_t *)args[2], (int)*(uint32_t *)args[3]);
      break;

    default:
      break;
  }

  return;
  // thread_exit ();
}

void syscall_halt (void) {
  shutdown_power_off();
}

void syscall_exit (int status) {
  struct thread* current_thread = thread_current();
  char thread_name[256];

  strlcpy(thread_name, (current_thread->name), sizeof(current_thread->name));

  get_command_from_file_name(thread_name);

  printf("%s: exit(%d)\n", thread_name, status);
  current_thread->exit_status = status;

  // struct thread *cur = thread_current();
  // struct list_elem *elem = NULL;
  // struct thread *child = NULL;
  // int exit_status = -1;

  // for (elem = list_begin(&cur->child); elem != list_end(&cur->child);
  //      elem = list_next(elem)){
  //     child = list_entry(elem, struct thread, child_elem);
  //     process_wait(child->tid);
  // }

  thread_exit();
}

pid_t syscall_exec(const char *file) {
  return process_execute(file);
}

int syscall_wait(pid_t pid) {
  return process_wait(pid);
}

int syscall_read(int fd, void *buffer, unsigned length) {
  unsigned cnt = -1;
  /* Check if the file descriptor is a standard input. */
  if (fd == 0){
    for (cnt = 0; cnt < length; cnt++){
      char c = (char)input_getc();
      *(uint8_t *)(buffer + cnt) = c;
      if (c == '\0'){
        break;
      }
    }
  }
  return cnt;
}

int syscall_write (int fd, const void *buffer, unsigned size) {
  int ret = -1;

  /* Check if file descriptor is a standard output. */
  if (fd == 1) {
    putbuf(buffer, (size_t)size);
    ret = (int)size;
  }
  return ret;
}

int syscall_fibonacci(int x){
  int x0 = 1, x1 = 1;
  
  if (x == 1 || x == 2){
    return 1;
  }

  for (int i = 3; i <= x; i++){
    int temp = x1;
    x1 = x0 + x1;
    x0 = temp;
  }

  return x1;
}

int syscall_max_of_four_int(int a, int b, int c, int d){
  int max_value = a;
  if (b > max_value){
    max_value = b;
  }
  if (c > max_value){
    max_value = c;
  }
  if (d > max_value){
    max_value = d;
  }
  return max_value;
}