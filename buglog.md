# ECE 391 MP3 Bug log

## Checkpoint 1

(03/10/23 9:07pm) Double fault exception every time we boot, after every instruction was properly serviced. The issue came from setting `idtr` register before initializing idt table. Fixed by calling `lidt()` after initializing the table (wonjong3)
