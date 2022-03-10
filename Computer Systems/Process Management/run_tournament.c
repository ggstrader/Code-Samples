#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PRNT 1
#define MSG_LEN  7
static char* MIN_DIST = "40";
static char* MAX_DIST = "45";
enum { READ, WRITE };
enum { SEND, RECV };
enum { WINS, LOSS, FAIL };

static volatile int END_GAME = 0;
static void finishRound(int sig) { END_GAME = 1; }

typedef int PipeSet[2][2]; //pipes have a set of two sets of file descriptors
typedef int RsltSet[3];
typedef struct data {
  uint8_t* skip_set;
  PipeSet* players_fds;
  int* player_pids;
  RsltSet* results;
  char** progs;
  PipeSet game_fds;
  int game_pid;
  int num_players;
  int seed;
} data;

static void run_tournament(int, int, int, char **);
static void runRound(data *);
static void reportResults(int, data *);
static void initGameMaker(data *);
static void closeGameMaker(data *);
static void initPlayerAtStartPos(data *);
static void finalizePlayers(data *, int);

static int makePipeline(PipeSet, char *, char **);
static char *getNString(PipeSet, int, int);
static char *getString(PipeSet);
static void myNWrite(PipeSet, char *, int);
static void myWrite(PipeSet, char *);
static char *itos(int);



int main(int argc, char **argv) {
  int rounds, seed;
  if (argc < 4) {
    fprintf(stderr, "%s: expects <random-seed> <round-count> <player-program> "
            "<player-program> ...\n", argv[0]);
    return 1;
  }

  seed = atoi(argv[1]);
  if (seed < 1) {
    fprintf(stderr, "%s: bad random seed; not a positive number: %s\n",
            argv[0], argv[1]);
    return 1;
  }

  rounds = atoi(argv[2]);
  if (rounds < 1) {
    fprintf(stderr, "%s: bad round count; not a positive number: %s\n",
            argv[0], argv[2]);
    return 1;
  }

  run_tournament(seed, rounds, argc - 3, argv + 3);

  return 0;
}



static void run_tournament(int seed, int rounds, int n, char **progs) {
  uint8_t* skip = calloc(n,sizeof(uint8_t));
  int *player_pids = malloc(n * sizeof(int));
  RsltSet *results = calloc(n, sizeof(RsltSet));
  PipeSet *players_fds = malloc(n * sizeof(PipeSet));

  data d = {skip, players_fds, player_pids, results, progs, {}, 0, n, seed};

  Signal(SIGINT, finishRound);

  int rounds_passed = 0;
  for (; rounds_passed < rounds; rounds_passed++) {
    if (END_GAME) rounds = rounds_passed;
    runRound(&d);
    d.seed++;
  }
  reportResults(rounds_passed, &d);
  free(skip); free(results); free(player_pids); free(players_fds);
}

static void reportResults(int final_round, data* d){
  printf("%d\n",final_round);
  int i;
  for (i = 0; i < d->num_players; i++)
    printf("%d %d %d\n",
      d->results[i][WINS], d->results[i][LOSS], d->results[i][FAIL] );
}



static void runRound(data *d) {
  int pid = 0, done = 0, num_wrong = 0;
  char *string;
  initGameMaker(d);
  initPlayerAtStartPos(d);
  while (!done) {
    if(num_wrong == d->num_players) break;
    for (pid = 0; pid < d->num_players; pid++) {
      if (d->skip_set[pid] == 1) continue;

      string = getString(d->players_fds[pid]);
      myWrite(d->game_fds, string);
      string = getString(d->game_fds);
      if (strcmp(string, "wrong!\n") == 0) {
        d->skip_set[pid] = 1; num_wrong++; continue;
      }

      myWrite(d->players_fds[pid], string);
      if (strcmp(string, "winner\n") == 0) {
        done = 1; break;
      }
    }
  }
  closeGameMaker(d);
  finalizePlayers(d, pid);
  memset(d->skip_set, 0, sizeof(uint8_t)*d->num_players);
}

static void initGameMaker(data* d){
  char* game_args[] =
    { "game_maker", itos(d->seed), itos(d->num_players), MIN_DIST, MAX_DIST, NULL };
  d->game_pid = makePipeline(d->game_fds, "game_maker", game_args);
  free(getString(d->game_fds)); // target comes first and we don't need it
}

static void initPlayerAtStartPos(data* d) {
  int i;
  for (i = 0; i < d->num_players; i++) {
    char *args[] = {d->progs[i], NULL};
    d->player_pids[i] = makePipeline(d->players_fds[i], d->progs[i], args);
    char *start_pos = getString(d->game_fds);
    myWrite(d->players_fds[i], start_pos);
    free(start_pos);
  }
}

static void closeGameMaker(data* d){
  int status;
  Waitpid(d->game_pid, &status,0);
  Close(d->game_fds[RECV][READ]);
  Close(d->game_fds[SEND][WRITE]);
}

static void finalizePlayers(data *d, int winner_id) {
  int status;
  int i;
  for (i = 0; i < d->num_players; i++) {
    if(d->skip_set[i]) d->results[i][FAIL]++;

    if (i == winner_id) {
      Waitpid(d->player_pids[i], &status, 0);
      d->results[i][WINS]++;
      if (WEXITSTATUS(status) > 0 || WIFSIGNALED(status) != 0)
          d->results[i][FAIL]++;
    }
    else {
      Kill(d->player_pids[i], SIGKILL);
      Waitpid(d->player_pids[i], &status, 0);
      d->results[i][LOSS]++;
    }
    Close(d->players_fds[i][RECV][READ]);
    Close(d->players_fds[i][SEND][WRITE]);
  }
}



static int makePipeline(PipeSet ps, char *process, char **args) {
  Pipe(ps[SEND]);
  Pipe(ps[RECV]);
  int pid = Fork();
  if (!pid) {
    sigset_t sigs;
    Sigemptyset(&sigs);
    Sigaddset(&sigs, SIGINT);
    Sigprocmask(SIG_BLOCK, &sigs, NULL);
    // Map child read to parent send pipe
    Dup2(ps[SEND][READ], READ);
    Close(ps[SEND][READ]);
    Close(ps[SEND][WRITE]);
    // Map child write to parent recieve pipe
    Dup2(ps[RECV][WRITE], WRITE);
    Close(ps[RECV][READ]);
    Close(ps[RECV][WRITE]);
    execve(process, args, environ);
  }
  Close(ps[SEND][READ]);  // send pipe isn't for reading
  Close(ps[RECV][WRITE]); // recieve pipe isn't for writing
  return pid;
}

static char *getString(PipeSet fds) { return getNString(fds, MSG_LEN, !PRNT); }
static char *getNString(PipeSet fds, int length, int print_payload) {
  char buffer[MSG_LEN + 1];
  Read(fds[RECV][READ], buffer, length);
  if (print_payload)
    printf("%s", &buffer[0]);
  char *string = calloc(1, length + 1); // calloc adds null terminator
  strncpy(string, buffer, length + 1);
  return string;
}

static void myWrite(PipeSet fds, char *buffer) {
  myNWrite(fds, buffer, MSG_LEN);
}
static void myNWrite(PipeSet fds, char* buffer, int length){
  Rio_writen(fds[SEND][WRITE], buffer, length);
}

static char *itos(int i) {
  int size = snprintf(NULL, 0, "%d", i);
  char *result = malloc(size + 1);
  sprintf(result, "%d", i);
  return result;
}
