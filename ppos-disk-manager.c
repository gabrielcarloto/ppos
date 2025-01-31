#include "ppos-disk-manager.h"
#include "disk-driver.h"
#include "ppos-core-globals.h"
#include "ppos-data.h"
#include "ppos.h"
#include "queue.h"
#include <errno.h>
#include <signal.h>
#include <string.h>

// adicione todas as variaveis globais necessarias para implementar o gerenciado
// do disco

disk_t disk_mgr;

void bodyDiskManager(void *arg);
void diskSignalHandler();

// função para o tratamento de erros dos sinais - usada em disk_mgr_init()
void clean_exit_on_sig(int sig_num) {
  printf("\n ERROR[Signal = %d]: %d \"%s\"", sig_num, errno, strerror(errno));
  exit(errno);
}

void sigusr1_handler() {
  diskrequest_t *next = disk_scheduler(disk_mgr.request_queue);
  queue_remove((queue_t **)&disk_mgr.request_queue, (queue_t *)next);
  sem_up(&disk_mgr.sem);
  /* task_resume(next->requester); */
}

int disk_mgr_init(int *numBlocks, int *blockSize) {
  // coloque o codigo para inicializar o disco aqui
  disk_mgr.request_queue = NULL;

  PPOS_PREEMPT_DISABLE;
  if (disk_cmd(DISK_CMD_INIT, 0, NULL) < 0) {
    PPOS_PREEMPT_ENABLE;
    return -1;
  }

  *numBlocks = disk_cmd(DISK_CMD_DISKSIZE, 0, NULL);
  *blockSize = disk_cmd(DISK_CMD_BLOCKSIZE, 0, NULL);

  if (*numBlocks == -1 || *blockSize == -1) {
    PPOS_PREEMPT_ENABLE;
    return -1;
  }

  if (sem_create(&disk_mgr.sem, 1) < 0) {
    PPOS_PREEMPT_ENABLE;
    return -1;
  }

  signal(SIGUSR1, sigusr1_handler);
  PPOS_PREEMPT_ENABLE;
  // o seu codigo deve terminar ate aqui.
  // As proximas linhas dessa função não devem ser modificadas
  signal(SIGSEGV, clean_exit_on_sig);

  return 0;
}

int disk_block_read(int block, void *buffer) {
  // TODO: escalonador
  sem_down(&disk_mgr.sem);

  diskrequest_t *exec_disk_req = malloc(sizeof(diskrequest_t));
  exec_disk_req->requester = taskExec;
  exec_disk_req->buffer = buffer;
  exec_disk_req->block = block;

  queue_append((queue_t **)&disk_mgr.request_queue, (queue_t *)exec_disk_req);

  diskrequest_t *next_req = disk_scheduler(disk_mgr.request_queue);

  int request = disk_cmd(DISK_CMD_READ, next_req->block, next_req->buffer);

  if (request < 0)
    return -1;

  /* task_suspend(taskExec, &sleepQueue); */

  return 0;
}

int disk_block_write(int block, void *buffer) { return 0; }

// Essa função implemeneta o escalonador de requisicoes de
// leitura/scrita do disco usado pelo gerenciador do disco
// A função implementa a política FCFS.
diskrequest_t *disk_scheduler(diskrequest_t *queue) {
  // FCFS scheduler
  if (queue != NULL) {
    PPOS_PREEMPT_DISABLE;
    diskrequest_t *request = queue;
    PPOS_PREEMPT_ENABLE;
    return request;
  }
  return NULL;
}
