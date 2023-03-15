# ECE 391 MP3 Bug log

## Checkpoint 1

- **(03/10/23)** Double fault exception every time we boot, after every instruction was properly serviced. The issue came from setting `idtr` register before initializing idt table. Fixed by calling `lidt()` after initializing the table (wonjong3)
- **(03/13/23)** General Protection exception when PIC is enabled. The issue was saving and restoring masks for IRQ. If PIC had irq enabled for devices we did not enable, this makes everything crash I guess. I manually masked all the IRQs at the end of PIC initialization to fix this. (wonjong3)
- **(03/13/23)** Only one interrupt was working for RTC interrupt. The issue was that EOI was only sent to the secondary pic, but not the primary pic. I had to adjust the if statement so that when secondary's irq is ended, EOI is sent to both primary and secondary pic.
