
# Main

Srunix86 is an open-source multi-user operating system, a Unix-like system. It is a rework of the original Srunix, which was created a long time ago by the same developers and was subsequently lost.
The operating system is written for the x86 architecture and runs in 64-bit mode.
Srunix86 is a complex project. The operating system itself contains a lot of components; it meets all the requirements to be called an OS. It has its own pseudo file system (bkFS), which is a reimagining of UFS - the Unix File System. The system features a Unix-like login screen, multitasking, TTY, and much more. The system complies with all SUS (Single UNIX Specification) standards but still cannot be called Unix.
Currently, the system lacks utilities for disk partitioning and installation onto a physical hard drive, but it can be installed on a real disk using Linux or BSD LiveCD systems. The original Srunix was distributed under the BSD license; Srunix R.E. continues this tradition and is also distributed under the open-source BSD 3.0 license. The file system supports POSIX standards. The file system, like everything else in the OS, is built completely from scratch, with the exception of GRUB2, which boots the OS. You can write your own program for the OS by simply creating a .h file, placing it in `src/`, linking to it in the kernel, and then compiling it. Currently, the OS is created mainly for interest and is not intended for large-scale tasks.

# History 

The original Srunix was created a long time ago by one person. It was a kernel for YobaOS, which was being developed for printing machines with a processor, but no one had heard of this operating system, let alone its kernel, and development halted. However, in 2022, development began on Srunix86 for personal computers with amd64 processors. Currently, Srunix86 only supports VGA mode and only runs on older or specific PCs with Pentium processors. On modern PCs, the OS is unlikely to boot. The original Srunix was lost forever.

# Development

If you want to help with the development of Srunix R.E., you can write to the lead developer at the email address: [`srunix@yandex.ru`](mailto:srunix@yandex.ru). If you don't know how to code but want to support the project's development, you can simply write a support email or donate via the DonationAlerts platform:
[https://www.donationalerts.com/r/srunix](https://www.donationalerts.com/r/srunix)

The Russian-language Telegram channel for the project, where the developer shares thoughts and news about Srunix:
[https://t.me/srunixre](https://t.me/srunixre)

# ISO

Download the dependencies for compilation: `nasm gcc binutils grub2 xorriso make mtools qemu-system qemu`, then use one of the make files:

`chmod 777 make`

`./make`

After that, you will be able to run Srunix86 via qemu-system-amd64 using the command `qemu-system-amd64 srunix86.iso`


# Srunix86 project logo (in degraded quality):



<img width="1154" height="1121" alt="srunix86 (копия)" src="https://github.com/user-attachments/assets/7d5b2361-60b9-43c4-8884-901fb4037eeb" />
