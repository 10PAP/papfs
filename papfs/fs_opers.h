#ifndef _FS_OPERS_H_
#define _FS_OPERS_H_

void getFullPath(char fpath[PATH_MAX], const char *path);

int PAPFS_getattr(const char *path, struct stat *statbuf);
int PAPFS_mknod(const char *path, mode_t mode, dev_t dev);
int PAPFS_unlink(const char *path);
int PAPFS_rename(const char *path, const char *newpath);
int PAPFS_utime(const char *path, struct utimbuf *ubuf);
int PAPFS_open(const char *path, struct fuse_file_info *fi);
int PAPFS_release(const char *path, struct fuse_file_info *fi);
int PAPFS_access(const char *path, int mask);

void * PAPFS_init(struct fuse_conn_info *conn);
void PAPFS_destroy(void *userdata);
int PAPFS_statfs(const char *path, struct statvfs *statv);

// DIRECTORY
int PAPFS_mkdir(const char *path, mode_t mode);
int PAPFS_opendir(const char *path, struct fuse_file_info *fi);
int PAPFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int PAPFS_rmdir(const char *path);
int PAPFS_releasedir(const char *path, struct fuse_file_info *fi);

#endif