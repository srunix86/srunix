# Main: #
Srunix R.E. - Open multi-user operating system, Unix-like system. It is a reworking of the original Srunix, which was created a long time ago by the same developers, and was later lost.
Srunix R.E. - stands for Srunix Remastered Edition. The operating system itself has a lot of everything; it meets all the requirements to be called an OS. It has its own File System (bkFS), which is a reinterpretation of UFS - Unix File System. The system has a login screen, like in Unix systems, multitasking, tty and much more. The system supports all SUS - Single UNIX Specification standards, but still cannot be called Unix.
There are no programs for partitioning and installation on a real disk in the system yet, but it can be installed on a real disk using Linux or BSD LiveCD systems. The original Srunix was distributed under the BSD license, Srunix R.E. It does not break tradition and is also distributed under the BSD 3.0 open license. The file system supports POSIX standards, the file system, like everything in the OS, is made completely from scratch, except for GRUB2, which loads the OS. You can write your own program for the OS by simply creating a .h file, placing it in src/ and pointing it to the kernel, and then compiling it. So far, the OS has been created just for fun, and is not intended for large-scale things.
# History: #
The original Srunix was created a long time ago by one person, Fedor Shimor. It was a kernel for YobaOS, which was created for typewriters with a processor, but no one had heard of this operating system, much less the kernel, and development stopped. But in 2022, the development of Srunix R.E began for personal PCs on amd64 processors. Now Srunix R.E. supports only VGA mode and runs only on old or specific PCs with x86_64 processors. On modern PCs, the OS is unlikely to be botched. And the original Srunix was lost forever.
# Development: #
If you want to help with the development of Srunix R.E., then you can write to the main developer by email: `srunix@yandex.ru`. If you don’t know how to code, but want to help develop the project, you can simply write a support letter by email or donate through the DonationAlerts platform:
https://www.donationalerts.com/r/srunix
# ISO compilation: #
Download dependencies for compilation: `nasm, gcc, binutils, grub2, xorriso, make`, then use one of the make files:

`./make # for bash`
or
`make # if bash is not installed.`

After this you will be able to launch Srunix R.E. via qemu-system-x86_64 using the command `qemu-system-x86_64 -boot d -cdrom srunix.iso`

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `

` `


6505 5857 4129 4993 5641 2833 _ 6505 3697 7153 2617 2833 6073 _ 4777 4993 6505 1969 7153 1969 _ 6505 2833 5641 5857 3697 1969 , _ 6505 _ 4129 4993 6073 4993 5641 4993 7153 _ 7153 1969 _ 2617 4993 2185 1969 6505 4345 7153 6289 _ 4129 6289 2401 3481 6289 _ 6505 5857 2833 3265 4993 , _ 2617 1969 7369 3481 2833 _ 3265 5641 1969 3049 3697 4129 6289
