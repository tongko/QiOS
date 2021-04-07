**Important notes**
===================

**Issues and solutions**
------------------------

1. ***crtbegin.o*** - relocation truncate to fit...

   This only happen when trying to place kernel image at higher-half, but not within -2GB. To solved this issue, first build libgcc with ***-mcmodel=kernel***, then change the
   virtual load address in linker script to within -2GB. E.g. 0xFFFFFFFF80000000 (x86_64). Finaly, link the kernel image with ***-mcmodel=kernel***. See [OSDev.org](https://wiki.osdev.org/Building_libgcc_for_mcmodel=kernel) for details.
2. ***no-red-zone*** - for x86_64 only.

   Compile libgcc with ***no-red-zone*** flag. See [OSDev.org](https://wiki.osdev.org/Libgcc_without_red_zone#:~:text=The%20red%2Dzone%20is%20a,or%20any%20running%20interrupt%20handler.) for detials.