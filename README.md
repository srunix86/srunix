# Main: #
Srunix R.E. - Open multi-user operating system, Unix-like system. It is a reworking of the original Srunix, which was created a long time ago by the same developers, and was later lost.
The operating system is written for i386 architecture, executed in 32 bits.
Srunix R.E. - stands for Srunix Remastered Edition. The operating system itself has a lot of everything; it meets all the requirements to be called an OS. It has its own File System (bkFS), which is a reinterpretation of UFS - Unix File System. The system has a login screen, like in Unix systems, multitasking, tty and much more. The system supports all SUS - Single UNIX Specification standards, but still cannot be called Unix.
There are no programs for partitioning and installation on a real disk in the system yet, but it can be installed on a real disk using Linux or BSD LiveCD systems. The original Srunix was distributed under the BSD license, Srunix R.E. It does not break tradition and is also distributed under the BSD 3.0 open license. The file system supports POSIX standards, the file system, like everything in the OS, is made completely from scratch, except for GRUB2, which loads the OS. You can write your own program for the OS by simply creating a .h file, placing it in src/ and pointing it to the kernel, and then compiling it. So far, the OS has been created just for fun, and is not intended for large-scale things.
# History: #
The original Srunix was created a long time ago by one person. It was a kernel for YobaOS, which was created for typewriters with a processor, but no one had heard of this operating system, much less the kernel, and development stopped. But in 2022, the development of Srunix R.E began for personal PCs on amd64 processors. Now Srunix R.E. supports only VGA mode and runs only on old or specific PCs with x86_64 processors. On modern PCs, the OS is unlikely to be botched. And the original Srunix was lost forever.
# Development: #
If you want to help with the development of Srunix R.E., then you can write to the main developer by email: `srunix@yandex.ru`. If you don’t know how to code, but want to help develop the project, you can simply write a support letter by email or donate through the DonationAlerts platform:
https://www.donationalerts.com/r/srunix
# ISO compilation: #
Download dependencies for compilation: `nasm gcc binutils grub2 xorriso make mtools qemu-system qemu`, then use one of the make files:

`chmod 777 make`

`./make`


After this you will be able to launch Srunix R.E. via qemu-system-i386 using the command `qemu-system-i386 livecd64.iso`

