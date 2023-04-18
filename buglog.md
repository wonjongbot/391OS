# ECE 391 MP3 Bug log

## Checkpoint 1

- **(03/10/23)** Double fault exception every time we boot, after every instruction was properly serviced. The issue came from setting `idtr` register before initializing idt table. Fixed by calling `lidt()` after initializing the table (wonjong3)
- **(03/13/23)** General Protection exception when PIC is enabled. The issue was saving and restoring masks for IRQ. If PIC had irq enabled for devices we did not enable, this makes everything crash I guess. I manually masked all the IRQs at the end of PIC initialization to fix this. (wonjong3)
- **(03/13/23)** Only one interrupt was working for RTC interrupt. The issue was that EOI was only sent to the secondary pic, but not the primary pic. I had to adjust the if statement so that when secondary's irq is ended, EOI is sent to both primary and secondary pic.
- **(3/21/23)** testing VGA page's upper bound address yields pagefault. This is because we accessed the address by pointers of integer instead of char, so it accessed 4 consecutive bytes from the starting address! We fixed it by using a pointer of char to access page address.
- **(3/21/23)** the paging in the info memory only show the kernal page and the first page  and many unused pages. This is because the present flag in the init of page table the first page directory.So just make these page present flag to zero.
- **(3/21/23)** our testing suite had a graphical bug when we tried to assign colors to PASS and FAIL flags. Turns out we were calling test functions twice when we are changing attribute value based on input test function.

## Checkpoint 2

- **(3/26/23)** we allowed file names greater than 32 chars by truncating them before searching. The behavior was changed to not allow that.
- **(3/26/23)** we were not null-checking dentry
- **(3/27/23)** when testing the d_read in read_dir_all, we can't print the names. This is because in pcb, directory's filearray[fd] != file's filearray[fd] so can't find right position, and pcb isn't requested in cp2, so just reinit the filearray[fd] in cp2 test by ourself, and improve this part in cp3.5. (jialins5)
- **(3/28/23)** rtc write wasn't working because the flags werent set correctly. Had to make the flag volatile and reset when target coutner is met.
- **(3/28/23)** terminal history had a bug that you had to press arrow keys twice when you are on the edge of the history buffer. Had to reset up_arrow_flag and down_arrow_flag after each history handler call
- **(3/28/23)** terminal_write couldn't handle formatted string with `\n` in the middle because it is terminated on newline character. Had to print nbytes regardless of buffer content.

## Checkpoint 3

- **(4/10/23)** syscall open's string length depended on the input fname instead of directory's name. User's string will always be longer (due to \n) so size of comparison had to be set to length of current dentry object's name.
- **(4/10/23)** matching the starting chars of a dentry was matching it...
- **(4/10/23)** exiting the base shell was executing with the wrong parent pid
- **(4/10/23)** halt skips to pid 0 no matter what. esp0 was not being set correctly (should be saved TOS of kernel instead of bottom of kernel stack).
- **(4/10/23)** was failing syserr test 3 and 5, basically testing behavior of incorrect file open and close. We failed this because we were opneing the exe in child's process instead of parent's process. So we had 1 less spot for new files to be open, and 1 more extra file to be closed wihtout error. We fixed this by closing the fd right after we use it to gain user space return address and copying to user space in the child's kernel stack. We also open the exe in parent kernel stack before switching the stack and close the opened exe after kernel is switched back to parent during halt.
- **(4/10/23)** was returning 0 from halt all the time, so no matter the reason for halt, the OS thinks it halted due to normal reasons. Had to do some special formatting since our interrupt handlers return negative of its exception numbers.
- **(4/9/23)** when runing ls.c in shell, after print the file name that longer than 32,the page fault. This is because when the file name longer than 32, in d-read, the return value will larger than 32 which is wrong, so fix all the return value longer than 32 to 32
- **(4/9/23)** status input of halt can't distinguish abnormal termination or eception because both can be 255. We set it so that exception cannot return 255, so only abnormal crash generates status of 255 (0xff, -1, whatever u like).

## Checkpoint 4

- **(4/15/23)** vidmap page was always enabled whenever execute is done. Changed so that vidmap page's present bit is set to 1 only when syscall_vidmap is called by user
- **(4/15/23)** `cat rtc` should terminate after some time (because it opens rtc and waits until rtc_read returns), but it returned immediately. Issue was that our counter to virtualize the rtc is set to 0, which is the condition rtc_read considers to be interrupt. So we had to set the rtc_open's starting counter value as 1 to avoid this.
- **(4/18/23)** vidmap created some weird pages mapped at 0x0 - 0x1000, and 0xcb8000-cb9000. Issue was that we used the same page table as previous checkpoints, which caused pages to overlap and duplicated. So we created a new page table for vidmap to fix this
