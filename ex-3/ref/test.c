
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>

int main(int argc, char **argv)
{
  int     return_code = 0;

  if (argc == 2) {
    if (chdir(*(argv+1)) == 0) {
      printf("カレントディレクトリの変更に成功しました\n");
    }
    else {
      printf("カレントディレクトリの変更に失敗しました\n");
      perror("");
      return_code = 1;
    }
  }
  else {
    printf("実行時引数の数が不当です\n");
    return_code = 2;
  }

	char path[MAXPATHLEN];
	if(!getcwd(path, sizeof(path)))
		return -1;
	puts(path);
  return return_code;
}
