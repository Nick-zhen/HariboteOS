#include "task.h"
#include "timer.h"
#include "memory.h"
#include "desctbl.h"

struct TaskCtl *taskctl;
struct Timer *task_timer;

struct Task *task_init(struct MemMan *memman)
{
  int i;
  struct Task *task;
  struct SegmentDescriptor *gdt = (struct SegmentDescriptor *) ADR_GDT;
  taskctl = (struct TaskCtl *) memman_alloc_4k(memman, sizeof (struct TaskCtl));
  for (i = 0; i < MAX_TASKS; i++) {
    taskctl->tasks0[i].flags = 0;
    taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
    set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
  }
  task = task_alloc();
  task->flags = 2; /*活动中标志*/
  task->priority = 2; /* 0.02 seconds */
  taskctl->running = 1;
  taskctl->now = 0;
  taskctl->tasks[0] = task;
  load_tr(task->sel);
  task_timer = timer_alloc();
  timer_set_timer(task_timer, task->priority);
  return task;
}

struct Task *task_alloc(void)
{
  int i;
  struct Task *task;
  for (i = 0; i < MAX_TASKS; i++) {
    if (taskctl->tasks0[i].flags == 0) {
      task = &taskctl->tasks0[i];
      task->flags = 1; /*正在使用的标志*/
      task->tss.eflags = 0x00000202; /* IF = 1; */
      task->tss.eax = 0; /*这里先置为0*/
      task->tss.ecx = 0;
      task->tss.edx = 0;
      task->tss.ebx = 0;
      task->tss.ebp = 0;
      task->tss.esi = 0;
      task->tss.edi = 0;
      task->tss.es = 0;
      task->tss.ds = 0;
      task->tss.fs = 0;
      task->tss.gs = 0;
      task->tss.ldtr = 0;
      task->tss.iomap = 0x40000000;
      return task;
    }
  }
  return 0; /*全部正在使用*/
}

void task_run(struct Task *task, int priority)
{
  if (priority > 0) {
    task->priority = priority;
  }
  if (task->flags != 2) {
    task->flags = 2; /*活动中标志*/
    taskctl->tasks[taskctl->running] = task;
    taskctl->running++;
  }
  return;
}

void task_switch(void)
{
  struct Task *task;
  taskctl->now++;
  if (taskctl->now == taskctl->running) {
      taskctl->now = 0;
  }
  task = taskctl->tasks[taskctl->now];
  timer_set_timer(task_timer, task->priority);

  // If there is only one task left, CPU will refuse to excute far_jmp(0, task->sel) since CPU will think
  // this is a bug due to run the task itself.
  if (taskctl->running >= 2) {
    far_jmp(0, task->sel);
  }
  return;
}

void task_sleep(struct Task *task)
{
  int i;
  char ts = 0;
  if (task->flags == 2) { /*如果指定任务处于唤醒状态*/
    if (task == taskctl->tasks[taskctl->now]) {
      ts = 1; /*让自己休眠的话，稍后需要进行任务切换*/
    }
    /*寻找task所在的位置*/
    for (i = 0; i < taskctl->running; i++) {
      if (taskctl->tasks[i] == task) {
        /*在这里*/
        break;
      }
    }
    taskctl->running--;
    if (i < taskctl->now) {
      taskctl->now--; /*需要移动成员，要相应地处理*/
    }
    /*移动成员*/
    for (; i < taskctl->running; i++) {
      taskctl->tasks[i] = taskctl->tasks[i + 1];
    }
    task->flags = 1; /*不工作的状态*/
    if (ts != 0) {
    /*任务切换*/
      if (taskctl->now >= taskctl->running) {
        /*如果now的值出现异常，则进行修正*/
        taskctl->now = 0;
      }
      far_jmp(0, taskctl->tasks[taskctl->now]->sel);
    }
  }
  return;
}