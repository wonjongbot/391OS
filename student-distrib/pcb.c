#include "pcb.h"

static file_ops_t stdin_ops = {stdin_read, stdin_write, terminal_open, terminal_close};
static file_ops_t stdout_ops = {stdout_read, stdout_write, terminal_open, terminal_close};

static int32_t process_using[MAX_PROCESS_NUM] = {0, 0, 0, 0, 0, 0};

/*
 * pid_alloc
 *   DESCRIPTION: allocate a pid for the process
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: the process id,  if successful
 *                 -1 if fail to allocate
 *   SIDE EFFECTS: none
 */
int32_t pid_alloc() {
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++) {
        if (process_using[i] == 0) {
            process_using[i] = 1;
            return i;
        }
    }
    return -1;
}

/*
 * pid_peek
 *   DESCRIPTION: see if process can be allocated
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: the process id,  if successful
 *                 -1 if fail to allocate
 *   SIDE EFFECTS: none
 */
int32_t pid_peek() {
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++) {
        if (process_using[i] == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * pid_dealloc
 *   DESCRIPTION: deallocate a pid for the process
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: deallocate pid specified;
 */
void pid_dealloc(int32_t pid) {
    if (pid >= MAX_PROCESS_NUM || pid < 0) {
        return;
    }
    process_using[pid] = 0;
}

void unload(pcb_t* pcb, uint32_t ret) {
    int i;
    // close all the file descriptor
    for (i = 2; i < 8; i++) {
        syscall_close(i);
    }
    pid_dealloc(pcb->pid);
    pcb->status = 0;
    if (pcb->parent != NULL) {
        active_process_idx = pcb->parent->pid;
        asm volatile(
                "movl %0, %%esp        \n\t"
                "movl %1, %%ebp        \n\t"
                "movl %2, %%eax        \n\t"
                :
                : "r"(pcb->parent->save_esp), "r"(pcb->parent->save_ebp), "r"(ret)
                : "cc"
                );
    }
}

/*
 * current_thread_PCB
 *   DESCRIPTION: point to the current process
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: the pointer to the current process
 *   SIDE EFFECTS: none
 */
inline pcb_t
*

current_thread_PCB() {
    pcb_t * ptr = 0;
    asm volatile(
            "movl $-8192, %0        \n\t"
            "andl %%esp, %0          \n\t"
            : "+r" (ptr)
            :
            : "cc"
            );
    return ptr;
}

pcb_t* PCB(uint32_t pid) {
    return (pcb_t*) (BOTTOM - PCB_size * (pid + 1));
}

/*
 * PCB_init
 *   DESCRIPTION: initialize the pcb.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if success
 *                 -1 if failure
 *   SIDE EFFECTS: none
 */
int32_t PCB_init(pcb_t* pcb) {
    pcb->pid = pid_alloc();     // assign a available pid
    pcb->status = 1;       //update the pcb's status
    pcb->parent = NULL;         //initialize the pcb's parent
    pcb->argc = 0;
    memset(pcb->argv, '\0', ARGV_MAX_LEN + 1);

    int i;
    // init stdin
    pcb->filearray[0].file_position = 0;
    pcb->filearray[0].flags = 1;
    pcb->filearray[0].inode_index = -1;
    pcb->filearray[0].ops = &stdin_ops;
    // init stdout
    pcb->filearray[1].file_position = 0;
    pcb->filearray[1].flags = 1;
    pcb->filearray[1].inode_index = -1;
    pcb->filearray[1].ops = &stdout_ops;
    // init memory map
    pcb->physical_mem_start = 0;
    // init fd from 2-7
    for (i = 2; i < FILEARR_SIZE; i++) {
        pcb->filearray[i].file_position = 0;
        pcb->filearray[i].flags = 0;
        pcb->filearray[i].inode_index = -1;
        pcb->filearray[i].ops = NULL;
    }
    // init threads
    pcb->thread_flag = 0;
    pcb->thread_num = 0;
    pcb->global_thread_index = 0;
    for (i = 0; i < MAX_THREAD_FOR_PCB; i++) {
        pcb->threads[i] = -1;
    }
    return 0;
}

void print_proc() {
    uint32_t i;
    printf("\n");
    for (i = 0; i < 6; i++) {
        if (i == sched_terminal_idx) set_attrib(0x3);
        if (i == active_terminal_idx) set_attrib(0x19);
        printf("%d", process_using[i]);
        set_attrib(0x7);
        printf(" ");
    }
    printf("\n");
}

int32_t sched_set(pcb_t* pcb){
    //printf("call 1\n");
    if(pcb == NULL) return -1;
    int32_t index = pcb->term_idx;
    if(index == -1) return -1;
    pid_to_sched_unit[index] = pcb->pid;
    return 0;
}

int32_t sched_free_pid(pcb_t* pcb){
    //printf("call 2\n");
    if(pcb == NULL) return -1;
    int32_t index = pcb->term_idx;
    if(index == -1) return -1;
    pid_to_sched_unit[index] = -1;
    return 0;
}
