#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void abrt_handler(int sig, siginfo_t *info, void *ctx);
volatile sig_atomic_t eflag = 0;

int main() {
  printf("start %s\n", __func__);

  struct sigaction sa_sigabrt;
  memset(&sa_sigabrt, 0, sizeof(sa_sigabrt));
  sa_sigabrt.sa_sigaction = abrt_handler;
  sa_sigabrt.sa_flags = SA_SIGINFO;

  if ( sigaction(SIGINT, &sa_sigabrt, NULL) < 0 ) {
    exit(1);
  }

  while ( !eflag ) {}

  printf("end %s\n", __func__);
  return 0;
}

void abrt_handler(int sig, siginfo_t *info, void *ctx) {
  // siginfo_tの値が取得できているか確認するために、printfで表示している。
  // 本来、printfは非同期安全でないためここで使用するべきではない。
  printf("si_signo:%d\nsi_code:%d\n", info->si_signo, info->si_code);
  printf("si_pid:%d\nsi_uid:%d\n", (int)info->si_pid, (int)info->si_uid);
  eflag = 1;
}
