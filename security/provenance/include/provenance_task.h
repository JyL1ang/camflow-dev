/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2015-2016 University of Cambridge,
 * Copyright (C) 2016-2017 Harvard University,
 * Copyright (C) 2017-2018 University of Cambridge,
 * Copyright (C) 2018-2021 University of Bristol
 *
 * Author: Thomas Pasquier <thomas.pasquier@bristol.ac.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 */
#ifndef _PROVENANCE_TASK_H
#define _PROVENANCE_TASK_H

#include <linux/cred.h>
#include <linux/binfmts.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
#include <linux/sched/signal.h>
#include <linux/utsname.h>
#include <linux/ipc_namespace.h>
#include <linux/mnt_namespace.h>
#include <linux/mm.h> // used for get_page
#include <net/net_namespace.h>
#include <linux/pid_namespace.h>
#include <linux/sched/cputime.h>
#include "../../../fs/mount.h" // nasty

#include "provenance_relay.h"
#include "provenance_inode.h"
#include "provenance_policy.h"
#include "memcpy_ss.h"

#define KB              1024
#define MB              (1024 * KB)
#define KB_MASK         (~(KB - 1))

/*!
 * @summary The following current_XXX functions are to obtain XXX
 * information of the current process.
 */
#define current_pid()    (current->pid)
static inline uint32_t get_cgroupns(struct task_struct *task)
{
	uint32_t id = 0;
	struct cgroup_namespace *cns;

	if (task->nsproxy) {
		cns = task->nsproxy->cgroup_ns;
		if (cns) {
			get_cgroup_ns(cns);
			id = cns->ns.inum;
			put_cgroup_ns(cns);
		}
	}
	return id;
}

static inline uint32_t get_utsns(struct task_struct *task)
{
	uint32_t id = 0;
	struct uts_namespace *ns;

	if (task->nsproxy) {
		ns = task->nsproxy->uts_ns;
		if (ns) {
			get_uts_ns(ns);
			id = ns->ns.inum;
			put_uts_ns(ns);
		}
	}
	return id;
}

static inline uint32_t get_ipcns(struct task_struct *task)
{
	uint32_t id = 0;
	struct ipc_namespace *ns;

	if (task->nsproxy) {
		ns = task->nsproxy->ipc_ns;
		if (ns) {
			get_ipc_ns(ns);
			id = ns->ns.inum;
			put_ipc_ns(ns);
		}
	}
	return id;
}

static inline uint32_t get_mntns(struct task_struct *task)
{
	uint32_t id = 0;
	struct mnt_namespace *ns;

	if (task->nsproxy) {
		ns = task->nsproxy->mnt_ns;
		if (ns) {
			get_mnt_ns(ns);
			id = ns->ns.inum;
			put_mnt_ns(ns);
		}
	}
	return id;
}

static inline uint32_t get_netns(struct task_struct *task)
{
	uint32_t id = 0;
	struct net *ns;

	if (task->nsproxy) {
		ns = task->nsproxy->net_ns;
		if (ns) {
			get_net(ns);
			id = ns->ns.inum;
			put_net(ns);
		}
	}
	return id;
}

static inline uint32_t get_pidns(struct task_struct *task)
{
	uint32_t id = 0;
	struct pid_namespace *ns;

	ns = task_active_pid_ns(task);
	if (ns)
		id = ns->ns.inum;
	return id;
}

static inline void update_task_namespaces(struct task_struct *task,
					  struct provenance *prov)
{
	task_lock(task);
	prov_elt(prov)->task_info.utsns = get_utsns(task);
	prov_elt(prov)->task_info.ipcns = get_ipcns(task);
	prov_elt(prov)->task_info.mntns = get_mntns(task);
	prov_elt(prov)->task_info.pidns = get_pidns(task);
	prov_elt(prov)->task_info.netns = get_netns(task);
	prov_elt(prov)->task_info.cgroupns = get_cgroupns(task);
	task_unlock(task);
}

#define vm_write(flags) ((flags & VM_WRITE) == VM_WRITE)
#define vm_read(flags) ((flags & VM_READ) == VM_READ)
#define vm_exec(flags) ((flags & VM_EXEC) == VM_EXEC)
#define vm_mayshare(flags) ((flags & (VM_SHARED | VM_MAYSHARE)) != 0)
#define vm_write_mayshare(flags) (vm_write(flags) && vm_mayshare(flags))
#define vm_read_exec_mayshare(flags) \
	((vm_read(flags) || vm_exec(flags)) && vm_mayshare(flags))

/*!
 * @brief Record shared mmap relations of a process.
 *
 * The function goes through all the mmapped files of the "current" process,
 * and for every shared mmaped file,
 * if the mmapped file has provenance entry,
 * record provenance relation between the mmaped file and the current process
 * based on the permission flags and the action (read, exec, or write).
 * If read/exec, record provenance relation RL_SH_READ by calling
 * "record_relation" function.
 * If write, record provenance relation RL_SH_WRITE by calling "record_relation"
 * function.
 * @param cprov The cred provenance entry pointer of the current process.
 * @param read Whether the operation is read or not.
 * @return 0 if no error occurred or "mm" is NULL; Other error codes inherited
 * from record_relation function or unknown.
 *
 */
static __always_inline int current_update_shst(struct provenance *cprov,
					       bool read)
{
	struct mm_struct *mm = get_task_mm(current);
	struct vm_area_struct *vma;
	struct file *mmapf;
	vm_flags_t flags;
	struct provenance *mmprov;
	int rc = 0;

	if (!mm)
		return rc;
	vma = mm->mmap;
	while (vma) { // We go through all the mmaped files.
		mmapf = vma->vm_file;
		if (mmapf) {
			flags = vma->vm_flags;
			mmprov = get_file_provenance(mmapf, false);
			if (mmprov) {
				if (vm_read_exec_mayshare(flags) && read)
					rc = record_relation(RL_SH_READ,
							     prov_entry(mmprov),
							     prov_entry(cprov),
							     mmapf,
							     flags);
				if (vm_write_mayshare(flags) && !read)
					rc = record_relation(RL_SH_WRITE,
							     prov_entry(cprov),
							     prov_entry(mmprov),
							     mmapf,
							     flags);
			}
		}
		vma = vma->vm_next;
	}
	mmput_async(mm);        // Release the file.
	return rc;
}

/*!
 * @brief Record the name of the task @task, and associate the name to the
 * provenance entry @prov by creating a relation by calling "record_node_name"
 * function.
 *
 * Unless failure occurs or certain criteria are met,
 * we obtain the name of the task from its "mm_exe_file", and create a
 * RL_NAMED_PROCESS relation by calling "record_node_name" function.
 * Criteria to be met so as not to record task name are:
 * 1. The name of the provenance node has already been recorded, or
 * 2. The provenance node itself is not recorded, or
 * 3. The "mm_exe_file"'s provenance is set to be opaque (if so, the @prov
 * itself will be set opaque).
 * @param task The task whose name is to be obtained.
 * @param prov The provenance entry that will be associated with the task name.
 * @return 0 if no error occurred; -ENOMEM if no memory can be allocated for
 * buffer to hold file path.
 *
 */
static inline int record_task_name(struct task_struct *task,
				   struct provenance *prov)
{
	// const struct cred *cred;
	struct provenance *fprov;
	struct mm_struct *mm;
	struct file *exe_file;
	char *buffer;
	char *ptr;
	int rc = 0;

	if (provenance_is_name_recorded(prov_elt(prov)) ||
	    !provenance_is_recorded(prov_elt(prov)))
		return 0;

	mm = get_task_mm(task);
	if (!mm)
		goto out;
	exe_file = get_mm_exe_file(mm);
	mmput_async(mm);
	if (exe_file) {
		fprov = get_file_provenance(exe_file, false);
		if (provenance_is_opaque(prov_elt(fprov))) {
			fput(exe_file); // Release the file.
			set_opaque(prov_elt(prov));
			goto out;
		}

		// Memory allocation not allowed to sleep.
		buffer = kcalloc(PATH_MAX, sizeof(char), GFP_ATOMIC);
		if (!buffer) {
			fput(exe_file); // Release the file.
			rc = -ENOMEM;
			goto out;
		}
		ptr = file_path(exe_file, buffer, PATH_MAX);
		fput(exe_file); // Release the file.
		rc = record_node_name(prov, ptr, false);
		kfree(buffer);
	}
out:
	return rc;
}

/*!
 * @brief Update @prov with process performance information associated with
 * @task.
 *
 * @param task The task whose performance information to be obtained.
 * @param prov The provenance entry to be updated.
 *
 */
static inline void update_task_perf(struct task_struct *task,
				    struct provenance *prov)
{
	struct mm_struct *mm;
	uint64_t utime;
	uint64_t stime;

	if (!task)
		return;

	// time
	task_cputime_adjusted(task, &utime, &stime);
	prov_elt(prov)->task_info.utime = div_u64(utime, NSEC_PER_USEC);
	prov_elt(prov)->task_info.stime = div_u64(stime, NSEC_PER_USEC);

	// memory
	mm = get_task_mm(task);
	if (mm) {
		// KB
		prov_elt(prov)->task_info.vm =
			mm->total_vm  * PAGE_SIZE / KB;
		prov_elt(prov)->task_info.rss =
			get_mm_rss(mm) * PAGE_SIZE / KB;
		prov_elt(prov)->task_info.hw_vm =
			get_mm_hiwater_vm(mm) * PAGE_SIZE / KB;
		prov_elt(prov)->task_info.hw_rss =
			get_mm_hiwater_rss(mm) * PAGE_SIZE / KB;
		mmput_async(mm);
	}
	// IO
#ifdef CONFIG_TASK_IO_ACCOUNTING
	// KB
	prov_elt(prov)->task_info.rbytes = task->ioac.read_bytes & KB_MASK;
	prov_elt(prov)->task_info.wbytes = task->ioac.write_bytes & KB_MASK;
	prov_elt(prov)->task_info.cancel_wbytes =
		task->ioac.cancelled_write_bytes & KB_MASK;
#else
	// KB
	prov_elt(prov)->task_info.rbytes = task->ioac.rchar & KB_MASK;
	prov_elt(prov)->task_info.wbytes = task->ioac.wchar & KB_MASK;
	prov_elt(prov)->task_info.cancel_wbytes = 0;
#endif
}

/*!
 * @brief Update and return provenance entry of cred structure.
 *
 * This function records the name of the current process and associates it with
 * the cred provenance entry,
 * unless the provenance is set to be opqaue, in which case no update is
 * performed.
 * The cred provenance entry is also updated with UID, GID, namespaces, secid,
 * and perform information.
 * @return The pointer to the cred provenance entry.
 *
 */
static inline struct provenance *get_cred_provenance(void)
{
	// returns provenance pointer of current_cred().
	struct provenance *prov = provenance_cred(current_cred());
	unsigned long irqflags;

	if (provenance_is_opaque(prov_elt(prov)))
		return prov;
	record_task_name(current, prov);
	spin_lock_irqsave_nested(prov_lock(prov),
				 irqflags, PROVENANCE_LOCK_PROC);
	prov_elt(prov)->proc_info.tgid = task_tgid_nr(current);
	prov_elt(prov)->proc_info.uid = __kuid_val(current_uid());
	prov_elt(prov)->proc_info.gid = __kgid_val(current_gid());
	security_task_getsecid_obj(current, &(prov_elt(prov)->proc_info.secid));
	spin_unlock_irqrestore(prov_lock(prov), irqflags);
	return prov;
}

/*!
 * @brief Return the provenance of current process.
 *
 * Get the provenance entry of the current process and update its pid and vpid.
 * We need to update pid and vpid here because when the task is first
 * initialized,
 * these information is not available.
 * @return The provenance entry pointer.
 *
 * @todo We do not want to waste resource to attempt to update pid and vpid
 * every time, since only the first update is needed. Find a better way to do
 * update only once.
 */
static inline struct provenance *get_task_provenance(bool link)
{
	struct provenance *tprov = provenance_task(current);

	prov_elt(tprov)->task_info.pid = task_pid_nr(current);
	prov_elt(tprov)->task_info.vpid = task_pid_vnr(current);
	update_task_perf(current, tprov);
	update_task_namespaces(current, tprov);
	if (!provenance_is_opaque(prov_elt(tprov)) && link)
		record_kernel_link(prov_entry(tprov));
	return tprov;
}

/*!
 * @brief Return process's provenance from @pid.
 *
 * @param pid The pid of the process whose provenance is to be returned.
 * @return The provenance entry pointer or NULL if process does not exist.
 *
 */
static inline struct provenance *prov_from_vpid(pid_t pid)
{
	struct provenance *cprov;
	// Function is in /kernel/pid.c
	struct task_struct *dest = find_task_by_vpid(pid);

	if (!dest)
		return NULL;

	cprov = provenance_cred(__task_cred(dest));
	if (!cprov)
		return NULL;
	return cprov;
}

/*!
 * Helper function used to copy arguments.
 * See fs/exec.c
 *
 */
static inline struct page *__get_arg_page_r(struct linux_binprm *bprm,
					    unsigned long pos)
{
	struct page *page;
	int ret;

	/*
	 * We are doing an exec().  'current' is the process
	 * doing the exec and bprm->mm is the new process's mm.
	 */
	mmap_read_lock(bprm->mm);
	ret = get_user_pages_remote(bprm->mm, pos, 1, FOLL_FORCE,
				    &page, NULL, NULL);
	mmap_read_unlock(bprm->mm);
	if (ret <= 0)
		return NULL;

	return page;
}

/*!
 * Copy bprm arguments. Helper function.
 * See fs/exec.c
 *
 */
static inline int copy_argv_bprm(struct linux_binprm *bprm, char *buff,
				 size_t len)
{
	int rv = 0;
	unsigned long ofs, bytes;
	struct page *page = NULL, *new_page;
	const char *kaddr;
	unsigned long pos;

	pos = bprm->p;
	ofs = pos % PAGE_SIZE;
	while (len) {
		new_page = __get_arg_page_r(bprm, pos);
		if (!new_page) {
			rv = -E2BIG;
			goto out;
		}
		if (page) {
			flush_dcache_page(page);
			kunmap(page);
			put_page(page);
		}
		page = new_page;
		kaddr = kmap(page);

		flush_cache_page(bprm->vma, pos, page_to_pfn(page));

		bytes = min_t(unsigned int, len, PAGE_SIZE - ofs);
		__memcpy_ss(buff, len, kaddr + ofs, bytes);
		pos += bytes;
		buff += bytes;
		len -= bytes;
		ofs = 0;
	}
	rv = pos - bprm->p;

out:
	if (page) {
		flush_dcache_page(page);
		kunmap(page);
		put_page(page);
	}
	return rv;
}

/*!
 * @brief Record ARG/ENV and create a relation betwene bprm->cred (in hooks.c)
 * and the args.
 *
 * This is a helper funtion used by record_args function.
 * It records @arg by:
 * 1. Creating a long provenance entry of type @vtype
 * (either ENT_ARG or ENT_ENV), and
 * 2. Recording a provenance relation @etype (either RL_ARG or RL_ENV depending
 * on @vtype) between the @arg and @prov
 * The length of the argument should not be longer than PATH_MAX, otherwise we
 * have to truncate the argument.
 * Note that the provenance entry is short-lived.
 * After we record the relation, we will free the long provenance entry.
 * @param prov The provenance entry pointer to which @arg has a relation.
 * @param vtype The type of the newly created long provenance entry.
 * @param etype The relation between @prov and @arg.
 * @param arg The value of the argument.
 * @param len The length of the argument.
 * @return 0 if no error occurred; -ENOMEM if no memory can be allocated from
 * long provenance cache; Other error codes inherited from record_relation
 * function or unknown.
 *
 */
static __always_inline int record_arg(struct provenance *prov,
				      uint64_t vtype,
				      uint64_t etype,
				      const char *arg,
				      size_t len)
{
	union long_prov_elt *aprov;
	int rc = 0;

	aprov = alloc_long_provenance(vtype, 0);
	if (!aprov)
		return -ENOMEM;
	aprov->arg_info.length = len;
	if (len >= PATH_MAX)
		aprov->arg_info.truncated = PROV_TRUNCATED;
	strscpy(aprov->arg_info.value, arg, PATH_MAX);

	rc = record_relation(etype, aprov, prov_entry(prov), NULL, 0);
	free_long_provenance(aprov);
	return rc;
}

/*!
 * @brief Record all arguments to @prov.
 *
 * We will only record all the arguments if @prov is tracked or capture all is
 * set.
 * We record both ENT_ARG and ENT_ENV types of arguments and relations RL_ARG
 * and RL_ENV between those arguments and @prov,
 * by calling record_arg function.
 * @param prov The provenance entry pointer where arguments should be associated
 * with.
 * @param bprm The binary parameter structure.
 * @return 0 if no error occurred; -ENOMEM if no memory available to copy
 * arguments. Other error codes unknown.
 *
 */
static inline int record_args(struct provenance *prov,
			      struct linux_binprm *bprm)
{
	char *argv;
	char *ptr;
	unsigned long len;
	size_t size;
	int rc = 0;
	int argc;
	int envc;

	if (!provenance_is_tracked(prov_elt(prov)) && !prov_policy.prov_all)
		return 0;
	len = bprm->exec - bprm->p;
	argv = kzalloc(len, GFP_KERNEL);
	if (!argv)
		return -ENOMEM;
	rc = copy_argv_bprm(bprm, argv, len);
	if (rc < 0)
		return -ENOMEM;
	argc = bprm->argc;
	envc = bprm->envc;
	ptr = argv;
	while (argc-- > 0) {
		size = strnlen(ptr, len);
		record_arg(prov, ENT_ARG, RL_ARG, ptr, size);
		ptr += size + 1;
	}
	while (envc-- > 0) {
		size = strnlen(ptr, len);
		record_arg(prov, ENT_ENV, RL_ENV, ptr, size);
		ptr += size + 1;
	}
	kfree(argv);
	return 0;
}
#endif
