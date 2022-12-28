#include "filesys/filesys.h"
#include <debug.h>
#include <stdio.h>
#include <string.h>

/* Partition that contains the file system. */
struct block *fs_device;


struct dir *split_path(const char *path, char *file_name) {
  if (path == NULL || file_name == NULL || path[0] == '\0') {
    return NULL;
  }

  char path_copy[PATH_MAX + 1];
  strlcpy(path_copy, path, PATH_MAX + 1);
  struct dir *dir = NULL;

  if (path_copy[0] == '/') {
    dir = dir_open_root();
  } 
  else {
    dir = dir_reopen(thread_current()->current_dir);
  }

  if (dir == NULL || !inode_is_dir(dir_get_inode(dir))) {
    dir_close(dir);
    return NULL;
  }

  char *save_ptr, *token, *next_token;
  token = strtok_r(path_copy, "/", &save_ptr);
  next_token = strtok_r(NULL, "/", &save_ptr);

  while (token != NULL && next_token != NULL) {
    struct inode *inode = NULL;

    if (!dir_lookup(dir, token, &inode)) {
      dir_close(dir);
      return NULL;
    }

    if (!inode_is_dir(inode)) {
      dir_close(dir);
      inode_close(inode);
      return NULL;
    }

    dir_close(dir);

    if (inode == NULL) {
      return NULL;
    }
    
    dir = dir_open(inode);

    token = next_token;
    next_token = strtok_r(NULL, "/", &save_ptr);
  }

  if (token != NULL) {
    strlcpy(file_name, token, PATH_MAX + 1);
  } 
  else {
    strlcpy(file_name, ".", PATH_MAX + 1);
  }

  return dir;
}


/* Initializes the file system module.
   If FORMAT is true, reformats the file system. */
void
filesys_init (bool format) 
{
  fs_device = block_get_role (BLOCK_FILESYS);
  if (fs_device == NULL)
    PANIC ("No file system device found, can't initialize file system.");

  inode_init ();
  free_map_init ();
  buffer_cache_init();

  if (format) 
    do_format ();

  thread_current()->current_dir = dir_open_root();
  free_map_open ();

  if (thread_current()->current_dir == NULL) {
    PANIC ("Can't open root directory.");
  }

  if (!inode_is_dir(dir_get_inode(thread_current()->current_dir))) {
    PANIC ("Root directory is not a directory.");
  }
}

/* Shuts down the file system module, writing any unwritten data
   to disk. */
void
filesys_done (void) 
{
  buffer_cache_terminate();
  free_map_close ();
}

bool 
filesys_add(block_sector_t* inode_sector, const char *name, struct dir *dir, bool is_dir, off_t initial_size) {
  bool success = false;
  if (dir != NULL) {
    success = free_map_allocate(1, inode_sector);

    if (!success) {
      return false;
    }

    if (is_dir) {
      success = dir_create(*inode_sector, initial_size);
    } 
    else {
      success = inode_create(*inode_sector, initial_size, false);
    }

    if (success) {
      success = dir_add(dir, name, *inode_sector);
    }

    if (!success && *inode_sector != 0) {
      free_map_release(*inode_sector, 1);
    }
  }
  return success;
}


/* Creates a file named NAME with the given INITIAL_SIZE.
   Returns true if successful, false otherwise.
   Fails if a file named NAME already exists,
   or if internal memory allocation fails. */
bool
filesys_create (const char *name, off_t initial_size) 
{
  block_sector_t inode_sector = 0;
  char file_name[PATH_MAX + 1];
  bool success = false;

  struct dir *dir = split_path(name, file_name);
  success = filesys_add(&inode_sector, file_name, dir, false, initial_size);
  
  dir_close (dir);
  return success;
}

/* Opens the file with the given NAME.
   Returns the new file if successful or a null pointer
   otherwise.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
struct file *
filesys_open (const char *name)
{
  char file_name[PATH_MAX + 1];
  struct dir *dir = split_path(name, file_name);
  struct inode *inode = NULL;

  if (dir != NULL)
    dir_lookup (dir, file_name, &inode);
  dir_close (dir);

  return file_open (inode);
}

/* Deletes the file named NAME.
   Returns true if successful, false on failure.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
bool
filesys_remove (const char *name) 
{
  char file_name[PATH_MAX + 1], file_name_copy[PATH_MAX + 1];
  struct dir *present_dir = split_path(name, file_name);

  bool success = false;
  struct inode *inode = NULL;

  if (present_dir != NULL && dir_lookup(present_dir, file_name, &inode)){
    if (inode_is_dir(inode)){
      struct dir *dir = NULL;
      if (dir = dir_open(inode)){
        if (!dir_readdir(dir, file_name_copy)){
          if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0){
            success = false;
          }
          else if (present_dir != NULL){
            success = dir_remove (present_dir, file_name);
          }
        }
        dir_close(dir);
      }
    }
    else {
      if (present_dir != NULL){
        success = dir_remove (present_dir, file_name);
      }
    }
  }
  dir_close (present_dir);
  return success;
}


/* Formats the file system. */
static void
do_format (void)
{
  printf ("Formatting file system...");
  free_map_create ();
  if (!dir_create (ROOT_DIR_SECTOR, 16))
    PANIC ("root directory creation failed");

  struct dir *root_dir = dir_open_root();
  dir_add(root_dir, ".", ROOT_DIR_SECTOR);
  dir_add(root_dir, "..", ROOT_DIR_SECTOR);

  dir_close(root_dir);

  free_map_close ();
  printf ("done.\n");
}

bool filesys_chdir(const char *present_dir){
    char file_name[PATH_MAX + 1];
    struct dir *dir = split_path(present_dir, file_name);
    struct inode *inode = NULL;
    bool success = false;

    if (dir != NULL){
        if (dir_lookup(dir, file_name, &inode)){
            if (inode_is_dir(inode)){
                dir_close(thread_current()->current_dir);
                thread_current()->current_dir = dir_open(inode);
                success = true;
            }
        }
    }

    dir_close(dir);
    return success;
}

bool filesys_mkdir(const char* name){
    block_sector_t inode_sector = 0;
    char file_name[PATH_MAX + 1];
    bool success = false;

    struct dir *present_dir = split_path(name, file_name);
    success = filesys_add(&inode_sector, file_name, present_dir, true, 16);

    if (success) {
      struct dir *dir = dir_open(inode_open(inode_sector));
      block_sector_t parent_inode_sector = inode_get_inumber(dir_get_inode(present_dir));
      dir_add(dir, ".", inode_sector);
      dir_add(dir, "..", parent_inode_sector);
      dir_close(dir);
    }

    dir_close(present_dir);

    return success;
}

