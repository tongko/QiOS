/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 * ****************************************************************************/
#ifndef __KERNEL_ERRORS_H_
#define __KERNEL_ERRORS_H_

#include <stdint.h>

extern uint32_t errno;

#define ERROR_SUCESS 0

#define ERROR_INVALID_FUNCTION 0x00000001           //	Incorrect function.
#define ERROR_FILE_NOT_FOUND 0x00000002             //	The system cannot find the file specified.
#define ERROR_PATH_NOT_FOUND 0x00000003             //	The system cannot find the path specified.
#define ERROR_TOO_MANY_OPEN_FILES 0x00000004        //	The system cannot open the file.
#define ERROR_ACCESS_DENIED 0x00000005              //	Access is denied.
#define ERROR_INVALID_HANDLE 0x00000006             //	The handle is invalid.
#define ERROR_ARENA_TRASHED 0x00000007              //	The storage control blocks were destroyed.
#define ERROR_NOT_ENOUGH_MEMORY 0x00000008          //	Not enough storage is available to process this command.
#define ERROR_INVALID_BLOCK 0x00000009              //	The storage control block address is invalid.
#define ERROR_BAD_ENVIRONMENT 0x0000000A            //	The environment is incorrect.
#define ERROR_BAD_FORMAT 0x0000000B                 //	An attempt was made to load a program with an incorrect format.
#define ERROR_INVALID_ACCESS 0x0000000C             //	The access code is invalid.
#define ERROR_INVALID_DATA 0x0000000D               //	The data is invalid.
#define ERROR_OUTOFMEMORY 0x0000000E                //	Not enough storage is available to complete this operation.
#define ERROR_INVALID_DRIVE 0x0000000F              //	The system cannot find the drive specified.
#define ERROR_CURRENT_DIRECTORY 0x00000010          //	The directory cannot be removed.
#define ERROR_NOT_SAME_DEVICE 0x00000011            //	The system cannot move the file to a different disk drive.
#define ERROR_NO_MORE_FILES 0x00000012              //	There are no more files.
#define ERROR_WRITE_PROTECT 0x00000013              //	The media is write-protected.
#define ERROR_BAD_UNIT 0x00000014                   //	The system cannot find the device specified.
#define ERROR_NOT_READY 0x00000015                  //	The device is not ready.
#define ERROR_BAD_COMMAND 0x00000016                //	The device does not recognize the command.
#define ERROR_CRC 0x00000017                        //	Data error (cyclic redundancy check).
#define ERROR_BAD_LENGTH 0x00000018                 //	The program issued a command but the command length is incorrect.
#define ERROR_SEEK 0x00000019                       //	The drive cannot locate a specific area or track on the disk.
#define ERROR_NOT_DOS_DISK 0x0000001A               //	The specified disk cannot be accessed.
#define ERROR_SECTOR_NOT_FOUND 0x0000001B           //	The drive cannot find the sector requested.
#define ERROR_OUT_OF_PAPER 0x0000001C               //	The printer is out of paper.
#define ERROR_WRITE_FAULT 0x0000001D                //	The system cannot write to the specified device.
#define ERROR_READ_FAULT 0x0000001E                 //	The system cannot read from the specified device.
#define ERROR_GEN_FAILURE 0x0000001F                //	A device attached to the system is not functioning.
#define ERROR_SHARING_VIOLATION 0x00000020          //	The process cannot access the file because it is being used by another process.
#define ERROR_LOCK_VIOLATION 0x00000021             //	The process cannot access the file because another process has locked a portion of the file.
#define ERROR_WRONG_DISK 0x00000022                 //	The wrong disk is in the drive. Insert %2 (Volume Serial Number: %3) into drive %1.
#define ERROR_SHARING_BUFFER_EXCEEDED 0x00000024    //	Too many files opened for sharing.
#define ERROR_HANDLE_EOF 0x00000026                 //	Reached the end of the file.
#define ERROR_HANDLE_DISK_FULL 0x00000027           //	The disk is full.
#define ERROR_NOT_SUPPORTED 0x00000032              //	The request is not supported.
#define ERROR_REM_NOT_LIST 0x00000033               //	Windows cannot find the network path.Verify that the network path is correct and the destination computer is not busy or turned off.If Windows still cannot find the network path, contact your network administrator.
#define ERROR_DUP_NAME 0x00000034                   //	You were not connected because a duplicate name exists on the network.Go to System in Control Panel to change the computer name, and then try again.
#define ERROR_BAD_NETPATH 0x00000035                //	The network path was not found.
#define ERROR_NETWORK_BUSY 0x00000036               //	The network is busy.
#define ERROR_DEV_NOT_EXIST 0x00000037              //	The specified network resource or device is no longer available.
#define ERROR_TOO_MANY_CMDS 0x00000038              //	The network BIOS command limit has been reached.
#define ERROR_ADAP_HDW_ERR 0x00000039               //	A network adapter hardware error occurred.
#define ERROR_BAD_NET_RESP 0x0000003A               //	The specified server cannot perform the requested operation.
#define ERROR_UNEXP_NET_ERR 0x0000003B              //	An unexpected network error occurred.
#define ERROR_BAD_REM_ADAP 0x0000003C               //	The remote adapter is not compatible.
#define ERROR_PRINTQ_FULL 0x0000003D                //	The print queue is full.
#define ERROR_NO_SPOOL_SPACE 0x0000003E             //	Space to store the file waiting to be printed is not available on the server.
#define ERROR_PRINT_CANCELLED 0x0000003F            //	Your file waiting to be printed was deleted.
#define ERROR_NETNAME_DELETED 0x00000040            //	The specified network name is no longer available.
#define ERROR_NETWORK_ACCESS_DENIED 0x00000041      //	Network access is denied.
#define ERROR_BAD_DEV_TYPE 0x00000042               //	The network resource type is not correct.
#define ERROR_BAD_NET_NAME 0x00000043               //	The network name cannot be found.
#define ERROR_TOO_MANY_NAMES 0x00000044             //	The name limit for the local computer network adapter card was exceeded.
#define ERROR_TOO_MANY_SESS 0x00000045              //	The network BIOS session limit was exceeded.
#define ERROR_SHARING_PAUSED 0x00000046             //	The remote server has been paused or is in the process of being started.
#define ERROR_REQ_NOT_ACCEP 0x00000047              //	No more connections can be made to this remote computer at this time because the computer has accepted the maximum number of connections.
#define ERROR_REDIR_PAUSED 0x00000048               //	The specified printer or disk device has been paused.
#define ERROR_FILE_EXISTS 0x00000050                //	The file exists.
#define ERROR_CANNOT_MAKE 0x00000052                //	The directory or file cannot be created.
#define ERROR_FAIL_I24 0x00000053                   //	Fail on INT 24.
#define ERROR_OUT_OF_STRUCTURES 0x00000054          //	Storage to process this request is not available.
#define ERROR_ALREADY_ASSIGNED 0x00000055           //	The local device name is already in use.
#define ERROR_INVALID_PASSWORD 0x00000056           //	The specified network password is not correct.
#define ERROR_INVALID_PARAMETER 0x00000057          //	The parameter is incorrect.
#define ERROR_NET_WRITE_FAULT 0x00000058            //	A write fault occurred on the network.
#define ERROR_NO_PROC_SLOTS 0x00000059              //	The system cannot start another process at this time.
#define ERROR_TOO_MANY_SEMAPHORES 0x00000064        //	Cannot create another system semaphore.
#define ERROR_EXCL_SEM_ALREADY_OWNED 0x00000065     //	The exclusive semaphore is owned by another process.
#define ERROR_SEM_IS_SET 0x00000066                 //	The semaphore is set and cannot be closed.
#define ERROR_TOO_MANY_SEM_REQUESTS 0x00000067      //	The semaphore cannot be set again.
#define ERROR_INVALID_AT_INTERRUPT_TIME 0x00000068  //	Cannot request exclusive semaphores at interrupt time.
#define ERROR_SEM_OWNER_DIED 0x00000069             //	The previous ownership of this semaphore has ended.
#define ERROR_SEM_USER_LIMIT 0x0000006A             //	Insert the disk for drive %1.
#define ERROR_DISK_CHANGE 0x0000006B                //	The program stopped because an alternate disk was not inserted.
#define ERROR_DRIVE_LOCKED 0x0000006C               //	The disk is in use or locked by another process.
#define ERROR_BROKEN_PIPE 0x0000006D                //	The pipe has been ended.
#define ERROR_OPEN_FAILED 0x0000006E                //	The system cannot open the device or file specified.
#define ERROR_BUFFER_OVERFLOW 0x0000006F            //	The file name is too long.
#define ERROR_DISK_FULL 0x00000070                  //	There is not enough space on the disk.
#define ERROR_NO_MORE_SEARCH_HANDLES 0x00000071     //	No more internal file identifiers are available.
#define ERROR_INVALID_TARGET_HANDLE 0x00000072      //	The target internal file identifier is incorrect.
#define ERROR_INVALID_CATEGORY 0x00000075           //	The Input Output Control (IOCTL) call made by the application program is not correct.
#define ERROR_INVALID_VERIFY_SWITCH 0x00000076      //	The verify-on-write switch parameter value is not correct.
#define ERROR_BAD_DRIVER_LEVEL 0x00000077           //	The system does not support the command requested.
#define ERROR_CALL_NOT_IMPLEMENTED 0x00000078       //	This function is not supported on this system.
#define ERROR_SEM_TIMEOUT 0x00000079                //	The semaphore time-out period has expired.
#define ERROR_INSUFFICIENT_BUFFER 0x0000007A        //	The data area passed to a system call is too small.
#define ERROR_INVALID_NAME 0x0000007B               //	The file name, directory name, or volume label syntax is incorrect.
#define ERROR_INVALID_LEVEL 0x0000007C              //	The system call level is not correct.

#endif  //	__KERNEL_ERRORS_H_