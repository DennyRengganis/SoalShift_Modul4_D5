#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR     
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif     
#ifdef linux
#define _XOPEN_SOURCE 500
#endif     
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
     
static const char *dirpath ="/home/stark/Desktop";
static int xmp_getattr(const char *path, struct stat *stbuf){
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    	res = lstat(fpath, stbuf);
    	if (res == -1)
    		return -errno;
    
    	return 0;
    }
 
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi){
    DIR *dp;
    struct dirent *de;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
     
    (void) offset;
    (void) fi;
     
    dp = opendir(fpath);
    if (dp == NULL)
    	return -errno;
     
    while ((de = readdir(dp)) != NULL) {
    	struct stat st;
    	memset(&st, 0, sizeof(st));
    	st.st_ino = de->d_ino;
    	st.st_mode = de->d_type << 12;
    	if (filler(buf, de->d_name, &st, 0))
    		break;
    	}
     
    closedir(dp);
    return 0;
}
     
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    int fd;
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    //newdir = mkdir("/home/stark/Dekstop/bram", 0777);    
    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
    	return -errno;
    res = pread(fd, buf, size, offset);
    if (res == -1)
    	res = -errno;
    close(fd);
    return res;
}
    
static struct fuse_operations xmp_oper = {
    .getattr	= xmp_getattr,
    .access	= xmp_access,
    .readlink	= xmp_readlink,
    .readdir	= xmp_readdir,
    .mknod	= xmp_mknod,
    .mkdir	= xmp_mkdir,
    .symlink	= xmp_symlink,
    .unlink	= xmp_unlink,
    .rmdir	= xmp_rmdir,
    .rename	= xmp_rename,
    .link	= xmp_link,
    .chmod	= xmp_chmod,
    .chown	= xmp_chown,
    .truncate	= xmp_truncate,
    .utimens	= xmp_utimens,
    .open	= xmp_open,
    .read	= xmp_read,
    .write	= xmp_write,
    .statfs	= xmp_statfs,
    .create     = xmp_create,
    .release	= xmp_release,
    .fsync	= xmp_fsync,
    #ifdef HAVE_SETXATTR
    .setxattr	= xmp_setxattr,
    .getxattr	= xmp_getxattr,
    .listxattr	= xmp_listxattr,
    .removexattr= xmp_removexattr,
    #endif
};
     
int main(int argc, char *argv[]){
    	umask(0);
    	return fuse_main(argc, argv, &xmp_oper, NULL);
}
