#include "params.h"
#include "log.h"

#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int fd_to_id(int fd) {
    for (int i = 0; i < PAPFS_DATA->opened_N; i++) {
        if (PAPFS_DATA->fd_table[i] == fd)
            return i;
    }
    exit(1);
}

// In some calls we need to get full path of file/directory
// We store root path (path to mount point) in our private data
void getFullPath(char fpath[PATH_MAX], const char *path) {
    strcpy(fpath, PAPFS_DATA->rootdir);
    strncat(fpath, path, PATH_MAX);
    log_print("    getFullPath:  rootdir = \"%s\", path = \"%s\", fpath = \"%s\"\n",
              PAPFS_DATA->rootdir, path, fpath);
}

int PAPFS_getattr(const char *path, struct stat *statbuf) {
    int retstat;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_getattr(path=\"%s\", statbuf=0x%08x)\n", path, statbuf);
    getFullPath(fpath, path);

    retstat = lstat(fpath, statbuf);
    if (retstat != 0)
        retstat = log_error("PAPFS_getattr lstat");

    return retstat;
}

// shouldn't that comment be "if" there is no.... ?
int PAPFS_mknod(const char *path, mode_t mode, dev_t dev) {
    int retstat;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n", path, mode, dev);
    getFullPath(fpath, path);

    retstat = mknod(fpath, mode, dev);
    if (retstat < 0)
        retstat = log_error("PAPFS_mknod mknod");

    int fd;
    fd = open(fpath, O_WRONLY);
    if (fd < 0)
        fd = log_error("PAPFS_mknod mknod");

    char type_flag = 0;
    write(fd, &type_flag, 1);

    close(fd);

    return retstat;
}

/** Create a directory */
int PAPFS_mkdir(const char *path, mode_t mode) {
    int retstat;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_mkdir(path=\"%s\", mode=0%3o)\n", path, mode);
    getFullPath(fpath, path);

    retstat = mkdir(fpath, mode);
    if (retstat < 0)
        retstat = log_error("PAPFS_mkdir mkdir");
    return retstat;
}

/** Remove a file */
int PAPFS_unlink(const char *path) {
    int retstat;
    char fpath[PATH_MAX];

    log_print("PAPFS_unlink(path=\"%s\")\n", path);
    getFullPath(fpath, path);

    retstat = unlink(fpath);
    if (retstat < 0)
        retstat = log_error("PAPFS_unlink unlink");
    return retstat;
}

int PAPFS_rmdir(const char *path) {
    int retstat;
    char fpath[PATH_MAX];

    log_print("PAPFS_rmdir(path=\"%s\")\n", path);
    getFullPath(fpath, path);

    retstat = rmdir(fpath);
    if (retstat < 0)
        retstat = log_error("PAPFS_rmdir rmdir");
    return retstat;
}

// both path and newpath are fs-relative
int PAPFS_rename(const char *path, const char *newpath) {
    int retstat;
    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];

    log_print("\nPAPFS_rename(fpath=\"%s\", newpath=\"%s\")\n", path, newpath);
    getFullPath(fpath, path);
    getFullPath(fnewpath, newpath);

    retstat = rename(fpath, fnewpath);
    if (retstat < 0)
        retstat = log_error("PAPFS_rename rename");
    return retstat;
}

// Change the access and/or modification times of a file
int PAPFS_utime(const char *path, struct utimbuf *ubuf) {
    int retstat;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_utime(path=\"%s\", ubuf=0x%08x)\n", path, ubuf);
    getFullPath(fpath, path);

    retstat = utime(fpath, ubuf);
    if (retstat < 0)
        retstat = log_error("PAPFS_utime utime");
    return retstat;
}

int PAPFS_statfs(const char *path, struct statvfs *statv) {
    int retstat;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_statfs(path=\"%s\", statv=0x%08x)\n", path, statv);
    getFullPath(fpath, path);

    // get stats for underlying filesystem
    retstat = statvfs(fpath, statv);
    if (retstat < 0)
        retstat = log_error("PAPFS_statfs statvfs");
    return retstat;
}

int PAPFS_release(const char *path, struct fuse_file_info *fi) {
    int retstat;
    log_print("\nPAPFS_release(path=\"%s\", fi=0x%08x)\n", path, fi);

    // clear metadata
    int id = fd_to_id(fi->fh);
    if (id == PAPFS_DATA->opened_N-1) {
        PAPFS_DATA->fd_table[id] = -1;
        PAPFS_DATA->opened_N--;
    } else {
        PAPFS_DATA->fd_table[id] = -1;
    }

    retstat = close(fi->fh);
    return retstat;
}

int PAPFS_opendir(const char *path, struct fuse_file_info *fi) {
    DIR *dp;
    int retstat = 0;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_opendir(path=\"%s\", fi=0x%08x)\n", path, fi);
    getFullPath(fpath, path);

    dp = opendir(fpath);
    if (dp == NULL)
        retstat = log_error("PAPFS_opendir opendir");

    fi->fh = (intptr_t) dp;
    return retstat;
}

int PAPFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    int retstat = 0;
    DIR *dp;
    struct dirent *de;

    log_print("\nPAPFS_readdir(path=\"%s\", buf=0x%08x, filler=0x%08x, offset=%lld, fi=0x%08x)\n",
              path, buf, filler, offset, fi);
    // once again, no need for fullpath -- but note that I need to cast fi->fh
    dp = (DIR *) (uintptr_t) fi->fh;

    de = readdir(dp);
    if (de == NULL) {
        retstat = log_error("PAPFS_readdir readdir");
        return retstat;
    }

    // This will copy the entire directory into the buffer.  The loop exits
    // when either the system readdir() returns NULL, or filler()
    // returns something non-zero.  The first case just means I've
    // read the whole directory; the second means the buffer is full.
    do {
        log_print("calling filler with name %s\n", de->d_name);
        if (filler(buf, de->d_name, NULL, 0) != 0) {
            log_print("    ERROR PAPFS_readdir filler:  buffer full");
            return -ENOMEM;
        }
    } while ((de = readdir(dp)) != NULL);
    return retstat;
}

int PAPFS_releasedir(const char *path, struct fuse_file_info *fi) {
    int retstat;

    log_print("\nPAPFS_releasedir(path=\"%s\", fi=0x%08x)\n", path, fi);

    retstat = closedir((DIR *) (uintptr_t) fi->fh);
    if (retstat != 0) {
        retstat = log_error("PAPFS_releasedir closedir");
    }
    return retstat;
}

int PAPFS_access(const char *path, int mask) {
    int retstat;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_access(path=\"%s\", mask=0%o)\n", path, mask);
    getFullPath(fpath, path);

    retstat = access(fpath, mask);

    if (retstat < 0)
        retstat = log_error("PAPFS_access access");
    return retstat;
}

void * PAPFS_init(struct fuse_conn_info *conn) {
    log_print("Filesystem initialization...");
    return PAPFS_DATA;
}

void PAPFS_destroy(void *userdata) {
    log_print("\nFilesystem was finished\n", userdata);
}