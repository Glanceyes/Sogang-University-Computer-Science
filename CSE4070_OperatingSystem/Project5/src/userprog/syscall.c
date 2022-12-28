#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "lib/kernel/console.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/inode.h"
#include "filesys/directory.h"

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
  [SYS_CHDIR] = 1,
  [SYS_MKDIR] = 1,
  [SYS_READDIR] = 2,
  [SYS_ISDIR] = 1,
  [SYS_INUMBER] = 1,
  [SYS_FIBONACCI] = 1,
  [SYS_MAX_FOUR_INT] = 4,
};

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  lock_init(&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void check_valid_address (const void *addr)
{
  if(!is_user_vaddr(addr) || pagedir_get_page(thread_current()->pagedir, addr) == NULL){
    syscall_exit(-1);
  }
}

void check_valid_file_descriptor (int fd) {
  if (fd < 3 || fd >= MAX_FD_NUM) {
    syscall_exit(-1);
  }

  struct thread* current_thread = thread_current();
  struct file* current_file = current_thread->file_descriptor[fd];

  if (current_file == NULL) {
    syscall_exit(-1);
  }
}

void check_valid_buffer (void *buffer, unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    check_valid_address(buffer + i);
  }
}

static void
get_syscall_args (int syscall_num, void *args[], void *esp)
{
  for(int i = 0; i < syscall_num_args[syscall_num]; i++){
    /* Check if the buffer is valid.*/
    check_valid_address((uint32_t *)esp + (i + 1));
    args[i] = (void*)((uint32_t *)esp + (i + 1));
  }
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  /* Get system call number. */
  int syscall_num = (int)*(uint32_t *)f->esp;

  /* Get system call arguments. */
  void *args[SYSCALL_ARGS_NUM_LIMITS] = { NULL, NULL, NULL, NULL };
  get_syscall_args(syscall_num, args, f->esp);

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
      f->eax = syscall_create((const char *)*(uint32_t *)args[0], (unsigned)*(uint32_t *)args[1]);
      break;

    case SYS_REMOVE:
      f->eax = syscall_remove((const char *)*(uint32_t *)args[0]);
      break;

    case SYS_OPEN:
      f->eax = syscall_open((const char *)*(uint32_t *)args[0]);
      break;

    case SYS_FILESIZE:
      f->eax = syscall_filesize((int)*(uint32_t *)args[0]);
      break;

    case SYS_READ:
      f->eax = syscall_read((int)*(uint32_t *)args[0], (void *)*(uint32_t *)args[1], (unsigned int)*(uint32_t*)args[2]);
      break;

    case SYS_WRITE:
      f->eax = syscall_write((int)*(uint32_t *)args[0], (void *)*(uint32_t *)args[1], (unsigned int)*(uint32_t *)args[2]);
      break;

    case SYS_SEEK:
      syscall_seek((int)*(uint32_t *)args[0], (unsigned)*(uint32_t *)args[1]);
      break;

    case SYS_TELL:
      f->eax = syscall_tell((int)*(uint32_t *)args[0]);
      break;

    case SYS_CLOSE:
      syscall_close((int)*(uint32_t *)args[0]);
      break;

    case SYS_MMAP:
      break;

    case SYS_MUNMAP:
      break;

    case SYS_CHDIR:
      f->eax = syscall_chdir((const char *)*(uint32_t *)args[0]);
      break;

    case SYS_MKDIR:
      f->eax = syscall_mkdir((const char *)*(uint32_t *)args[0]);
      break;

    case SYS_READDIR:
      f->eax = syscall_readdir((int)*(uint32_t *)args[0], (char *)*(uint32_t *)args[1]);
      break;

    case SYS_ISDIR:
      f->eax = syscall_isdir((int)*(uint32_t *)args[0]);
      break;

    case SYS_INUMBER:
      f->eax = syscall_inumber((int)*(uint32_t *)args[0]);
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
}

void syscall_halt (void) {
  shutdown_power_off();
}

void syscall_exit (int status) {
  int exit_status = -1;
  if (status != -1){
    exit_status = status;
  }
  struct thread* current_thread = thread_current();
  char thread_name[256];

  strlcpy(thread_name, (current_thread->name), sizeof(current_thread->name));

  get_command_from_file_name(thread_name);

  printf("%s: exit(%d)\n", thread_name, exit_status);
  current_thread->exit_status = exit_status;

  for (int fd = 3; fd < MAX_FD_NUM; fd++) {
    if (current_thread->file_descriptor[fd] != NULL) {
      struct inode *found_node = file_get_inode(current_thread->file_descriptor[fd]);
      if (found_node != NULL && found_node->deny_write_cnt > 0){
        if (inode_is_dir(found_node)) {
          dir_close(current_thread->file_descriptor[fd]);
        } else {
          syscall_close(fd);
        }
      }
      current_thread->file_descriptor[fd] = NULL;
    }
  }

  if (thread_current()->current_dir){
    dir_close(thread_current()->current_dir);
  }

  struct list_elem *elem = NULL;
  struct thread *child = NULL;

  for (elem = list_begin(&current_thread->child); elem != list_end(&current_thread->child);
       elem = list_next(elem)){
      child = list_entry(elem, struct thread, child_elem);
      process_wait(child->tid);
  }

  thread_exit();
}

pid_t syscall_exec(const char *file) {
  if (file == NULL) {
    return -1;
  }
  return process_execute(file);
}

int syscall_wait(pid_t pid) {
  return process_wait(pid);
}

bool syscall_create(const char *file, unsigned initial_size) {
  if (file == NULL) {
    syscall_exit(-1);
  }

  return filesys_create(file, initial_size);
}

bool syscall_remove(const char *file) {
  if (file == NULL) {
    syscall_exit(-1);
  }

  return filesys_remove(file);
}

int syscall_open(const char *file) {
  if (file == NULL) {
    syscall_exit(-1);
  }
  lock_acquire(&filesys_lock);
  struct file* file_opened = filesys_open(file);
  lock_release(&filesys_lock);

  if (file_opened == NULL) {
    return -1;
  }

  struct thread* current_thread = thread_current();
  for (int fd = 3; fd < MAX_FD_NUM; fd++){
    if (current_thread->file_descriptor[fd] == NULL) {
        if (strcmp(file, current_thread->name) == 0) {
            file_deny_write(file_opened);
        }
        current_thread->file_descriptor[fd] = file_opened;
        return fd;
    }
}
return -1;
  
}

int syscall_filesize(int fd) {
  check_valid_file_descriptor(fd);

  struct thread* current_thread = thread_current();
  struct file* current_file = current_thread->file_descriptor[fd];

  return file_length(current_file);
}

int syscall_read(int fd, void *buffer, unsigned length) {
  int ret = -1;
  check_valid_buffer(buffer, length);

  /* Check if the file descriptor is a standard input. */
  if (fd == 0){
    lock_acquire(&filesys_lock);
    for (ret = 0; ret < length; ret++){
      char c = (char)input_getc();
      *(uint8_t *)(buffer + ret) = c;
      if (c == '\0'){
        break;
      }
    }
    lock_release(&filesys_lock);
  }
  else if (fd >= 3 && fd < MAX_FD_NUM){
    check_valid_file_descriptor(fd);
    struct thread* current_thread = thread_current();
    struct file* current_file = current_thread->file_descriptor[fd];
    lock_acquire(&filesys_lock);
    ret = file_read(current_file, buffer, length);
    lock_release(&filesys_lock);
  }
  else {
    syscall_exit(-1);
  }

  
  return ret;
}

int syscall_write (int fd, void *buffer, unsigned size) {
  int ret = -1;
  // check_valid_address(buffer);
  check_valid_buffer(buffer, size);
  

  /* Check if file descriptor is a standard output. */
  if (fd == 1) {
    lock_acquire(&filesys_lock);
    putbuf(buffer, (size_t)size);
    lock_release(&filesys_lock);
    ret = (int)size;
  }
  else if (fd >= 3 && fd < MAX_FD_NUM){
    check_valid_file_descriptor(fd);
    struct thread* current_thread = thread_current();
    struct file* current_file = current_thread->file_descriptor[fd];
    lock_acquire(&filesys_lock);
    ret = file_write(current_file, buffer, size);
    lock_release(&filesys_lock);
  }
  else {
    syscall_exit(-1);
  }
  return ret;
}

void syscall_seek(int fd, unsigned position) {
  check_valid_file_descriptor(fd);

  struct thread* current_thread = thread_current();
  struct file* current_file = current_thread->file_descriptor[fd];
  file_seek(current_file, position);
}

unsigned syscall_tell(int fd) {
  check_valid_file_descriptor(fd);

  struct thread* current_thread = thread_current();
  struct file* current_file = current_thread->file_descriptor[fd];
  return (unsigned)file_tell(current_file);
}

void syscall_close(int fd) {
  check_valid_file_descriptor(fd);

  struct thread* current_thread = thread_current();
  struct file* current_file = current_thread->file_descriptor[fd];

  file_close(current_file);
  current_thread->file_descriptor[fd] = NULL;
}

int syscall_chdir(const char *dir) {
  return filesys_chdir(dir);
}

int syscall_mkdir(const char *dir) {
  return filesys_mkdir(dir);
}

bool syscall_readdir(int fd, char *name) {
  check_valid_file_descriptor(fd);

  struct thread* current_thread = thread_current();
  struct file* current_file = current_thread->file_descriptor[fd];

  struct inode* inode = file_get_inode(current_file);
  if (inode == NULL || !inode_is_dir(inode)) {
    return false;
  }

  struct dir* dir = dir_open(inode);
  dir->pos = file_tell(current_file);
  bool success = dir_readdir(dir, name);
  file_seek(current_file, dir->pos);
  return success;
}

int syscall_isdir(int fd) {
  check_valid_file_descriptor(fd);

  struct thread* current_thread = thread_current();
  struct file* current_file = current_thread->file_descriptor[fd];

  struct inode* inode = file_get_inode(current_file);
  return inode_is_dir(inode);
}

int syscall_inumber(int fd) {
  check_valid_file_descriptor(fd);

  struct thread* current_thread = thread_current();
  struct file* current_file = current_thread->file_descriptor[fd];

  if (current_file == NULL) {
    syscall_exit(-1);
  }

  struct inode* inode = file_get_inode(current_file);
  return inode_get_inumber(inode);
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