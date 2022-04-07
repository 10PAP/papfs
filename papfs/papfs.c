#include "params.h"

#include <unistd.h>
#include <stdlib.h>

#include "log.h"
#include "fs_opers.h"
#include "compressor.h"

int PAPFS_open(const char *path, struct fuse_file_info *fi) {
    int retstat = 0;
    int fd;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_open(path\"%s\", fi=0x%08x)\n", path, fi);
    getFullPath(fpath, path);

    // open file
    fd = open(fpath, fi->flags);
    if (fd < 0)
        retstat = log_error("PAPFS_open open");
    fi->fh = fd;

    // get metadata from file
    load_metadata(fd);

    return retstat;
}

int PAPFS_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    size_t retstat;

    log_print("\nPAPFS_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n", path, buf, size, offset, fi);
    
    retstat = pread(fi->fh, buf, size, offset);

    return (int) retstat;
}

int PAPFS_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    size_t retstat;
    
    log_print("\nPAPFS_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",path, buf, size, offset, fi);

    // TODO: RANDOM ACCESS WRITE OPERATION


    retstat = pwrite(fi->fh, buf, size, offset);
    if (retstat < 0){
	    retstat = log_error("PAPFS_write pwrite");
    }
    
    return (int) retstat;
} 

struct fuse_operations PAPFS_oper = {
  .getattr = PAPFS_getattr,
  .mknod = PAPFS_mknod,
  .mkdir = PAPFS_mkdir,
  .unlink = PAPFS_unlink,
  .rmdir = PAPFS_rmdir,
  .rename = PAPFS_rename,
  .utime = PAPFS_utime,
  .open = PAPFS_open,
  .read = PAPFS_read,
  .write = PAPFS_write,
  .statfs = PAPFS_statfs,
  .release = PAPFS_release,
  .opendir = PAPFS_opendir,
  .readdir = PAPFS_readdir,
  .releasedir = PAPFS_releasedir,
  .init = PAPFS_init,
  .destroy = PAPFS_destroy,
  .access = PAPFS_access,
};

void PAPFS_usage() {
    fprintf(stderr, "usage:  papfs [FUSE and mount options] rootDir mountPoint uID\n");
    abort();
}

int main(int argc, char *argv[]) {
    int fuse_stat;
    struct fs_state *PAPFS_data;

    if ((getuid() == 0) || (geteuid() == 0)) {
	    fprintf(stderr, "Please, don't run this filesystem with root\n");
    	return 1;
    }
    if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-')){
    	PAPFS_usage();
    }

    // init filesystem state
    PAPFS_data = malloc(sizeof(struct fs_state));
    if (PAPFS_data == NULL) {
    	perror("main calloc");
	    abort();
    }

    // add rootdir to private data
    PAPFS_data->rootdir = realpath(argv[argc-2], NULL);
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;
    
    PAPFS_data->logfile = log_open();
    
    fprintf(stderr, "about to call fuse_main\n");
    fuse_stat = fuse_main(argc, argv, &PAPFS_oper, PAPFS_data);
    fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
    
    return fuse_stat;
}
