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
