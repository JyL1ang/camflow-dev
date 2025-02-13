/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * Copyright (C) 2015-2016 University of Cambridge,
 * Copyright (C) 2016-2017 Harvard University,
 * Copyright (C) 2017-2018 University of Cambridge,
 * Copyright (C) 2018-2020 University of Bristol
 *
 * Author: Thomas Pasquier <thomas.pasquier@bristol.ac.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef _UAPI_LINUX_PROVENANCE_TYPES_H
#define _UAPI_LINUX_PROVENANCE_TYPES_H

#ifndef __KERNEL__
#include <stdint.h>
#include <stdbool.h>
#endif


#define TYPE_MASK                               0xFFFF000000000000UL
#define SUBTYPE_MASK                            0x0000FFFFFFFFFFFFUL

#define W3C_TYPE(type)          (type & TYPE_MASK)
#define SUBTYPE(type)           (type & SUBTYPE_MASK)

/* W3C PROV TYPES */
#define DM_RELATION                             0x8000000000000000UL
#define DM_ACTIVITY                             0x4000000000000000UL
#define DM_ENTITY                               0x2000000000000000UL
#define DM_AGENT                                0x1000000000000000UL
/* NODE IS LONG*/
#define ND_LONG                                                   0x0400000000000000UL
/* ALLOWED/DISALLOWED */
#define RL_ALLOWED                              0x0200000000000000UL
#define RL_DISALLOWED                           0x0100000000000000UL
/* SUBTYPES */
/* RELATIONS W3C TYPE*/
#define RL_DERIVED                              (DM_RELATION | 0x0080000000000000ULL)
#define RL_GENERATED                            (DM_RELATION | 0x0040000000000000ULL)
#define RL_USED                                 (DM_RELATION | 0x0020000000000000ULL)
#define RL_INFORMED                             (DM_RELATION | 0x0010000000000000ULL)
#define RL_INFLUENCED                           (DM_RELATION | 0x0008000000000000ULL)
#define RL_ASSOCIATED                           (DM_RELATION | 0x0004000000000000ULL)

/* DERIVED SUBTYPES */
#define RL_NAMED                                (RL_DERIVED   | (0x0000000000000001ULL))
#define RL_VERSION                              (RL_DERIVED   | (0x0000000000000001ULL << 1))
#define RL_SND_PACKET                           (RL_DERIVED   | (0x0000000000000001ULL << 2))
#define RL_SND_UNIX                             (RL_DERIVED   | (0x0000000000000001ULL << 3))
#define RL_RCV_PACKET                           (RL_DERIVED   | (0x0000000000000001ULL << 4))
#define RL_RCV_UNIX                             (RL_DERIVED   | (0x0000000000000001ULL << 5))
#define RL_FREED                                (RL_DERIVED   | (0x0000000000000001ULL << 6))
#define RL_SETATTR_INODE                        (RL_DERIVED   | (0x0000000000000001ULL << 7))
#define RL_ACCEPT_SOCKET                        (RL_DERIVED   | (0x0000000000000001ULL << 8))
#define RL_GETXATTR_INODE                       (RL_DERIVED   | (0x0000000000000001ULL << 9))
#define RL_SETXATTR_INODE                       (RL_DERIVED   | (0x0000000000000001ULL << 10))
#define RL_RMVXATTR_INODE                       (RL_DERIVED   | (0x0000000000000001ULL << 11))
#define RL_EXEC                                 (RL_DERIVED   | (0x0000000000000001ULL << 12))
#define RL_TERMINATE_PROC                       (RL_DERIVED   | (0x0000000000000001ULL << 13))
#define RL_ARG                                  (RL_DERIVED   | (0x0000000000000001ULL << 14))
#define RL_ENV                                  (RL_DERIVED   | (0x0000000000000001ULL << 15))
#define RL_SH_READ                              (RL_DERIVED   | (0x0000000000000001ULL << 16))
#define RL_SH_WRITE                             (RL_DERIVED   | (0x0000000000000001ULL << 17))
#define RL_PCK_CNT                              (RL_DERIVED   | (0x0000000000000001ULL << 18))
#define RL_ADDRESSED                            (RL_DERIVED   | (0x0000000000000001ULL << 19))
#define RL_DERIVED_DISC                         (RL_DERIVED   | (0x0000000000000001ULL << 20))
/* no more than 51!!!! */

/* GENERATED SUBTYPES */
#define RL_CLONE_MEM                            (RL_GENERATED | (0x0000000000000001ULL))
#define RL_MSG_CREATE                           (RL_GENERATED | (0x0000000000000001ULL << 1))
#define RL_SOCKET_CREATE                        (RL_GENERATED | (0x0000000000000001ULL << 2))
#define RL_SOCKET_PAIR_CREATE                   (RL_GENERATED | (0x0000000000000001ULL << 3))
#define RL_INODE_CREATE                         (RL_GENERATED | (0x0000000000000001ULL << 4))
#define RL_WRITE                                (RL_GENERATED | (0x0000000000000001ULL << 5))
#define RL_WRITE_IOCTL                          (RL_GENERATED | (0x0000000000000001ULL << 6))
#define RL_PROC_WRITE                           (RL_GENERATED | (0x0000000000000001ULL << 7))
#define RL_CONNECT                              (RL_GENERATED | (0x0000000000000001ULL << 8))
#define RL_CONNECT_UNIX_STREAM                  (RL_GENERATED | (0x0000000000000001ULL << 9))
#define RL_LISTEN                               (RL_GENERATED | (0x0000000000000001ULL << 10))
#define RL_BIND                                 (RL_GENERATED | (0x0000000000000001ULL << 11))
#define RL_SND                                  (RL_GENERATED | (0x0000000000000001ULL << 12))
#define RL_SND_MSG                              (RL_GENERATED | (0x0000000000000001ULL << 13))
#define RL_SND_MSG_Q                            (RL_GENERATED | (0x0000000000000001ULL << 14))
#define RL_LINK                                 (RL_GENERATED | (0x0000000000000001ULL << 15))
#define RL_RENAME                               (RL_GENERATED | (0x0000000000000001ULL << 16))
#define RL_UNLINK                               (RL_GENERATED | (0x0000000000000001ULL << 17))
#define RL_SYMLINK                              (RL_GENERATED | (0x0000000000000001ULL << 18))
#define RL_SETATTR                              (RL_GENERATED | (0x0000000000000001ULL << 19))
#define RL_SETXATTR                             (RL_GENERATED | (0x0000000000000001ULL << 20))
#define RL_RMVXATTR                             (RL_GENERATED | (0x0000000000000001ULL << 21))
#define RL_SHMDT                                (RL_GENERATED | (0x0000000000000001ULL << 22))
#define RL_SETUID                               (RL_GENERATED | (0x0000000000000001ULL << 23))
#define RL_SETGID                               (RL_GENERATED | (0x0000000000000001ULL << 24))
#define RL_SH_ATTACH                            (RL_GENERATED | (0x0000000000000001ULL << 25))
#define RL_SH_CREATE                            (RL_GENERATED | (0x0000000000000001ULL << 26))
#define RL_FILE_LOCK                            (RL_GENERATED | (0x0000000000000001ULL << 27))
#define RL_MUNMAP                               (RL_GENERATED | (0x0000000000000001ULL << 28))
#define RL_SPLICE_OUT                           (RL_GENERATED | (0x0000000000000001ULL << 29))
#define RL_EXEC_TASK                            (RL_GENERATED | (0x0000000000000001ULL << 30))
#define RL_PTRACE_ATTACH                        (RL_GENERATED | (0x0000000000000001ULL << 31))
#define RL_GENERATED_DISC                       (RL_GENERATED | (0x0000000000000001ULL << 32))
/* no more than 51!!!! */

/* USED SUBTYPES */
#define RL_READ                                 (RL_USED        | (0x0000000000000001ULL))
#define RL_READ_IOCTL                           (RL_USED        | (0x0000000000000001ULL << 1))
#define RL_PROC_READ                            (RL_USED        | (0x0000000000000001ULL << 2))
#define RL_ACCEPT                               (RL_USED        | (0x0000000000000001ULL << 3))
#define RL_RCV                                  (RL_USED        | (0x0000000000000001ULL << 4))
#define RL_RCV_MSG                              (RL_USED        | (0x0000000000000001ULL << 5))
#define RL_RCV_MSG_Q                            (RL_USED        | (0x0000000000000001ULL << 6))
#define RL_OPEN                                 (RL_USED        | (0x0000000000000001ULL << 7))
#define RL_FILE_RCV                             (RL_USED        | (0x0000000000000001ULL << 8))
#define RL_FILE_SIGIO                           (RL_USED        | (0x0000000000000001ULL << 9))
#define RL_SEARCH                               (RL_USED        | (0x0000000000000001ULL << 10))
#define RL_GETATTR                              (RL_USED        | (0x0000000000000001ULL << 11))
#define RL_READ_LINK                            (RL_USED        | (0x0000000000000001ULL << 12))
#define RL_GETXATTR                             (RL_USED        | (0x0000000000000001ULL << 13))
#define RL_LSTXATTR                             (RL_USED        | (0x0000000000000001ULL << 14))
#define RL_LOG                                  (RL_USED        | (0x0000000000000001ULL << 15))
#define RL_PERM                                 (RL_USED        | (0x0000000000000001ULL << 16))
#define RL_GETGID                               (RL_USED        | (0x0000000000000001ULL << 17))
#define RL_SPLICE_IN                            (RL_USED        | (0x0000000000000001ULL << 18))
#define RL_MMAP                                 (RL_USED        | (0x0000000000000001ULL << 19))
#define RL_MMAP_PRIVATE                                         (RL_USED        | (0x0000000000000001ULL << 20))
#define RL_LOAD_FILE                            (RL_USED        | (0x0000000000000001ULL << 21))
#define RL_PTRACE_READ                          (RL_USED        | (0x0000000000000001ULL << 22))
#define RL_USED_DISC                            (RL_USED        | (0x0000000000000001ULL << 23))
/* no more than 51!!!! */

/* INFORMED SUBTYPES */
#define RL_CLONE                                (RL_INFORMED  | (0x0000000000000001ULL))
#define RL_VERSION_TASK                         (RL_INFORMED  | (0x0000000000000001ULL << 1))
#define RL_TERMINATE_TASK                       (RL_INFORMED  | (0x0000000000000001ULL << 2))
#define RL_PTRACE_ATTACH_TASK                   (RL_INFORMED  | (0x0000000000000001ULL << 3))
#define RL_PTRACE_READ_TASK                     (RL_INFORMED  | (0x0000000000000001ULL << 4))
#define RL_PTRACE_TRACEME                       (RL_INFORMED  | (0x0000000000000001ULL << 5))
#define RL_INFORMED_DISC                        (RL_INFORMED  | (0x0000000000000001ULL << 6))
/* no more than 51!!!! */

/* INFLUENCED  SUBTYPES */
#define RL_LOAD_UNKNOWN                         (RL_INFLUENCED | (0x0000000000000001ULL))
#define RL_LOAD_FIRMWARE                        (RL_INFLUENCED | (0x0000000000000001ULL << 1))
#define RL_LOAD_MODULE                          (RL_INFLUENCED | (0x0000000000000001ULL << 2))
#define RL_LOAD_KEXEC_IMAGE                     (RL_INFLUENCED | (0x0000000000000001ULL << 3))
#define RL_LOAD_KEXEC_INITRAMFS                 (RL_INFLUENCED | (0x0000000000000001ULL << 4))
#define RL_LOAD_POLICY                          (RL_INFLUENCED | (0x0000000000000001ULL << 5))
#define RL_LOAD_CERTIFICATE                     (RL_INFLUENCED | (0x0000000000000001ULL << 6))
#define RL_LOAD_UNDEFINED                       (RL_INFLUENCED | (0x0000000000000001ULL << 7))
#define RL_INFLUENCED_DISC                      (RL_INFLUENCED | (0x0000000000000001ULL << 8))

/* ASSOCIATED  SUBTYPES */
#define RL_RAN_ON                               (RL_ASSOCIATED | (0x0000000000000001ULL))
#define RL_ASSOCIATED_DISC                      (RL_ASSOCIATED | (0x0000000000000001ULL << 1))

/* ACTIVITY SUBTYPES */
#define ACT_TASK                                (DM_ACTIVITY  | 0x0000000000000001ULL)

/* LONG NODE */
/* DISCLOSED TYPE */
#define ACT_DISC                                (DM_ACTIVITY | ND_LONG | (0x0000000000000001ULL << 1))

/* AGENT SUBTYPES */
#define AGT_USR                                 (DM_AGENT       | (0x0000000000000001ULL << 2))
#define AGT_GRP                                 (DM_AGENT       | (0x0000000000000001ULL << 3))

/* LONG NODE */
#define AGT_MACHINE                             (DM_AGENT | ND_LONG | (0x0000000000000001ULL << 4))
/* DISCLOSED TYPE */
#define AGT_DISC                                (DM_AGENT | ND_LONG | (0x0000000000000001ULL << 5))

/* ENTITY SUBTYPES */
#define ENT_INODE_UNKNOWN                       (DM_ENTITY    | (0x0000000000000001ULL << 6))
#define ENT_INODE_LINK                          (DM_ENTITY    | (0x0000000000000001ULL << 7))
#define ENT_INODE_FILE                          (DM_ENTITY    | (0x0000000000000001ULL << 8))
#define ENT_INODE_DIRECTORY                     (DM_ENTITY    | (0x0000000000000001ULL << 9))
#define ENT_INODE_CHAR                          (DM_ENTITY    | (0x0000000000000001ULL << 10))
#define ENT_INODE_BLOCK                         (DM_ENTITY    | (0x0000000000000001ULL << 11))
#define ENT_INODE_PIPE                          (DM_ENTITY    | (0x0000000000000001ULL << 12))
#define ENT_INODE_SOCKET                        (DM_ENTITY    | (0x0000000000000001ULL << 13))
#define ENT_MSG                                 (DM_ENTITY    | (0x0000000000000001ULL << 14))
#define ENT_SHM                                 (DM_ENTITY    | (0x0000000000000001ULL << 15))
#define ENT_SBLCK                               (DM_ENTITY    | (0x0000000000000001ULL << 16))
#define ENT_PACKET                              (DM_ENTITY    | (0x0000000000000001ULL << 17))
#define ENT_IATTR                               (DM_ENTITY    | (0x0000000000000001ULL << 18))
#define ENT_PROC                                (DM_ENTITY    | (0x0000000000000001ULL << 19))

/* LONG NODE */
#define ENT_STR                                 (DM_ENTITY | ND_LONG | (0x0000000000000001ULL << 20))
#define ENT_ADDR                                (DM_ENTITY | ND_LONG | (0x0000000000000001ULL << 21))
#define ENT_PATH                                (DM_ENTITY | ND_LONG | (0x0000000000000001ULL << 22))
#define ENT_XATTR                               (DM_ENTITY | ND_LONG | (0x0000000000000001ULL << 23))
#define ENT_PCKCNT                              (DM_ENTITY | ND_LONG | (0x0000000000000001ULL << 24))
#define ENT_ARG                                 (DM_ENTITY | ND_LONG | (0x0000000000000001ULL << 25))
#define ENT_ENV                                 (DM_ENTITY | ND_LONG | (0x0000000000000001ULL << 26))
/* DISCLOSED TYPE */
#define ENT_DISC                                (DM_ENTITY | ND_LONG | (0x0000000000000001ULL << 27))

#define prov_type(prov)                 ((prov)->node_info.identifier.node_id.type)
#define node_type(node)                 prov_type(node)
#define edge_type(edge)                 prov_type(edge)
#define prov_is_relation(prov)          ((relation_identifier(prov).type & DM_RELATION) != 0)
#define prov_is_node(prov)              ((node_identifier(prov).type & DM_RELATION) == 0)
#define prov_is_packet(prov)            (node_type(prov) == ENT_PACKET)

#define prov_is_type(val, type)         ((val & type) == type)
#define prov_type_is_relation(val)      prov_is_type(val, DM_RELATION)
#define prov_type_is_node(val)          (!prov_is_type(val, DM_RELATION))
#define prov_type_is_long(val)          (prov_is_type(val, ND_LONG) && prov_type_is_node(val))
#define prov_is_used(val)               prov_is_type(val, RL_USED)
#define prov_is_informed(val)           prov_is_type(val, RL_INFORMED)
#define prov_is_influenced(val)         prov_is_type(val, RL_INFLUENCED)
#define prov_is_associated(val)         prov_is_type(val, RL_ASSOCIATED)
#define prov_is_generated(val)          prov_is_type(val, RL_GENERATED)
#define prov_is_derived(val)            prov_is_type(val, RL_DERIVED)

#define prov_is_close(val)              (val == RL_TERMINATE_TASK    \
					 || val == RL_TERMINATE_PROC \
					 || val == RL_FREED)

static inline bool prov_has_uidgid(uint64_t type)
{
	switch (type) {
	case ENT_PROC:
	case ENT_INODE_UNKNOWN:
	case ENT_INODE_LINK:
	case ENT_INODE_FILE:
	case ENT_INODE_DIRECTORY:
	case ENT_INODE_CHAR:
	case ENT_INODE_BLOCK:
	case ENT_INODE_PIPE:
	case ENT_INODE_SOCKET:
		return true;
	default: return false;
	}
}

static inline bool prov_is_inode(uint64_t type)
{
	switch (type) {
	case ENT_INODE_UNKNOWN:
	case ENT_INODE_LINK:
	case ENT_INODE_FILE:
	case ENT_INODE_DIRECTORY:
	case ENT_INODE_CHAR:
	case ENT_INODE_BLOCK:
	case ENT_INODE_PIPE:
	case ENT_INODE_SOCKET:
		return true;
	default: return false;
	}
}

static inline bool prov_has_secid(uint64_t type)
{
	switch (type) {
	case ENT_PROC:
	case ENT_INODE_UNKNOWN:
	case ENT_INODE_LINK:
	case ENT_INODE_FILE:
	case ENT_INODE_DIRECTORY:
	case ENT_INODE_CHAR:
	case ENT_INODE_BLOCK:
	case ENT_INODE_PIPE:
	case ENT_INODE_SOCKET:
		return true;
	default: return false;
	}
}

#define PROV_TYPE_STR_MAX_LEN 256
struct prov_type {
	uint64_t id;
	char str[PROV_TYPE_STR_MAX_LEN];
	uint8_t is_relation;
};

#endif /* _UAPI_LINUX_PROVENANCE_TYPES_H */
