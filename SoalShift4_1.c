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
     
static const char *dirpath ="/home/denny/Documents";
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
    int rem;
    sprintf(fpath, "%s%s", dirpath, path);
     
    (void) offset;
    (void) fi;
     
    dp = opendir(fpath);
    if (dp != NULL){}
    else {return -errno;}
    while ((de = readdir(dp)) != NULL) {
    	struct stat st;
    	memset(&st, 0, sizeof(st));
    	st.st_ino = de->d_ino;
    	st.st_mode = de->d_type << 12;
	rem=filler(buf, de->d_name, &st, 0);
    	if (rem==0){}
	else {break;}
	}     
    closedir(dp);
    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	
	char temp[5];
	int panjang=strlen(fpath);
	int i,count,indeks=3;
	for(i=panjang-1,count=1;count<=4;i--,count++){
		temp[indeks--]=fpath[i];
	}

 	if(strcmp(temp,".doc")==0||strcmp(temp,".txt")==0||strcmp(temp,".pdf")==0){
		char ch, source_file[1000], target_file[1000],command[1000],command2[1000];
		system("mkdir /home/denny/Downloads/rahasia -p");
		sprintf(source_file,"%s",fpath);
		sprintf(target_file,"%s.ditandai",fpath);
		int hasil_rename=rename(source_file,target_file);
		sprintf(command,"chmod --- %s.ditandai",fpath);
		sprintf(command2,"mv %s.ditandai /home/denny/Downloads/rahasia",fpath);
		system(command);
		system("zenity --error --text=\"Terjadi Kesalahan! File berisi konten berbahaya.\n\" --title=\"Warning!\"");
		system(command2);

		return 1;
	}
	else{
		res = open(fpath, fi->flags);
		if (res == -1)
			return -errno;
 	
		close(res);	
		return 0;
	}
}
     
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    int fd;
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);    
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
    .readdir	= xmp_readdir,

    .open	= xmp_open,
    .read	= xmp_read,
};
     
int main(int argc, char *argv[]){
    	return fuse_main(argc, argv, &xmp_oper, NULL);
}
