#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/dir.h>
#include <pwd.h>
#include <grp.h>
#include <uuid/uuid.h>
#include <time.h>

#define MAXCHAR 256
void permission(struct stat fileStat, char *buf);
char *month(int num);
void myls(char *ans, int argc, char *argv[])
{
	DIR *dp;
	char dirname[MAXCHAR] = ".";
	dp = opendir(dirname);


	struct dirent *dir;
	while ((dir = readdir(dp)) != NULL) {

			struct stat sb;
			if (stat(dir->d_name, &sb) < 0) {
				perror("stat");
				exit(1);
			}

			//permission st_mode to string
			char *pms;
			pms = (char *)malloc(sizeof(char) * MAXCHAR);
			memset(pms, 0, MAXCHAR);
			permission(sb, pms);
			struct passwd *pws = getpwuid(sb.st_uid);
			struct group *gpws = getgrgid(sb.st_gid);
			struct tm *tmp = localtime(&sb.st_mtime);
			sprintf(ans, "%s %2d %s  %s %6d ", pms, sb.st_nlink, pws->pw_name, gpws->gr_name, (int)sb.st_size);

			sprintf(ans, "%s %2d %2d:%2d ", month(tmp->tm_mon), tmp->tm_mday, tmp->tm_hour, tmp->tm_min);
			sprintf(ans, "%s ", dir->d_name);

			printf("\n");
			free(pms);


	}

	closedir(dp);

}

int getfile(char *ans, char *cmd, char *dirname)
{
	DIR *dp;

	dp = opendir(dirname);

	struct dirent *dir;

	while ((dir = readdir(dp)) != NULL) {
		char pathname[MAXCHAR];
		sprintf(pathname, "%s%s", dirname, cmd);
		int fp = open("pathname", O_RDONLY);
		printf("fp%d\n", fp);
		if (fp != -1 && fp != 9) {
			sprintf(ans, "%s%s", dirname, cmd);

			return 1;
		}
	}


	closedir(dp);

	return -1;

}


void permission(struct stat fileStat, char *buf)
{
	int i = 0;
    buf[i++] =  (S_ISDIR(fileStat.st_mode)) ? 'd' : '-';
    buf[i++] =  (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
    buf[i++] =  (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
    buf[i++] =  (fileStat.st_mode & S_IXUSR) ? 'x' : '-';
    buf[i++] =  (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
    buf[i++] =  (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
    buf[i++] =  (fileStat.st_mode & S_IXGRP) ? 'x' : '-';
    buf[i++] =  (fileStat.st_mode & S_IROTH) ? 'r' : '-';
    buf[i++] =  (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
    buf[i++] =  (fileStat.st_mode & S_IXOTH) ? 'x' : '-';
}

char *month(int num)
{
	num++;
	switch (num) {
		case 1:
			return "Jan";
		case 2:
			return "Feb";
		case 3:
			return "Mar";
		case 4:
			return "Apr";
		case 5:
			return "May";
		case 6:
			return "Jun";
		case 7:
			return "Jul";
		case 8:
			return "Aug";
		case 9:
			return "Sep";
		case 10:
			return "Oct";
		case 11:
			return "Nov";
		case 12:
			return "Dec";
		default:
			return "xxx";
	}
}
