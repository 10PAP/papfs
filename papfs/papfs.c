#include "params.h"

#include <unistd.h>
#include <stdlib.h>

#include "log.h"
#include "fs_opers.h"
#include "compressor.h"

int update_fdtable(int fd, int file_type) {
    // update fd table
    int new_id = PAPFS_DATA->opened_N;
    PAPFS_DATA->opened_N++;

    for (int i = 0; i < PAPFS_DATA->opened_N; i++) {
        if (PAPFS_DATA->fd_table[i] == -1) {
            new_id = i;
            PAPFS_DATA->opened_N--;
            break;
        }
    }

    PAPFS_DATA->fd_table[new_id] = fd;
    PAPFS_DATA->metadata[new_id].type_flag = file_type;
    log_print("METADATA: new id for file descr: %d\n", new_id);

    return new_id;
}

int PAPFS_open(const char *path, struct fuse_file_info *fi) {
    int retstat = 0;
    int fd;
    char fpath[PATH_MAX];

    log_print("\nPAPFS_open(path\"%s\", fi=0x%08x)\n", path, fi);
    getFullPath(fpath, path);

    // open file
    
    // TODO : normal flags handling
    fd = open(fpath, O_RDWR);
    if (fd < 0)
        retstat = log_error("PAPFS_open open");
    fi->fh = fd;

    // check file type
    char type_flag;
    char res = read(fd, &type_flag, 1);
    if (res != 1)
        return -1;

    if (type_flag) {
        // 1) compressed file
        log_print("This is compressed file\n");

        // get metadata from file
        load_metadata(fd);
    } else {
        // 2) plain file
        log_print("This is plain file\n");

        update_fdtable(fd, 0);
    }

    return retstat;
}

int PAPFS_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int retstat = 0;

    log_print("\nPAPFS_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n", path, buf, size, offset, fi);

    // get id of file metadata in PAPFS_DATA structure
    int file_id = fd_to_id(fi->fh);

    if(PAPFS_DATA->metadata[file_id].type_flag == 1){
        for (unsigned long i = 0; i < size; i++) {
            int res = random_access_read_symbol(file_id, ((unsigned long) offset) + i);
            if (res == -1) {
                break;
            }
            buf[i] = (char) res;
            retstat++;
        }
    }
    else {
        retstat = pread(fi->fh, buf, size, offset+1);
    }
    return (int) retstat;
}

int PAPFS_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    size_t retstat;
    
    log_print("\nPAPFS_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",path, buf, size, offset, fi);

    // TODO: RANDOM ACCESS WRITE OPERATION

    int file_id = fd_to_id(fi->fh);

    if(PAPFS_DATA->metadata[file_id].type_flag == 0){
      retstat = pwrite(fi->fh, buf, size, offset+1);
      if (retstat < 0){
        retstat = log_error("PAPFS_write pwrite");
      }
    }
    else {
        retstat = log_error("PAPFS_write pwrite: trying to write compressed file");
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

    PAPFS_data->opened_N = 0;

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
