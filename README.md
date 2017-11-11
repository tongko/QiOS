Sin Hing Operating System

Folder structure
.sysroot (shos)
|-- bin\
|   |-- boot\
|       |-- grub\
|       |   |-- grub.cfg
|       |-- kernel.elf 
|-- obj\
|   |-- (various obj files)
|-- src\
|   |-- boot\
|   |   |-- arc\
|   |   |   |-- x86\
|   |   |       |-- (various architecture specific kernel source file)
|   |   |-- (various non-arch specific header file)
|   |   
|   |-- inc\
|   |   |-- arch\
|   |   |   |-- x86\
|   |   |       |-- (various arch-specific kernel header files)
|   |   |-- (various non-arch specific kernel header files)