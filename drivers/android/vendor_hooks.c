// SPDX-License-Identifier: GPL-2.0-only
/* vendor_hook.c
 *
 * Android Vendor Hook Support
 *
 * Copyright 2020 Google LLC
 */

#ifndef __GENKSYMS__
#include <linux/rmap.h>
#endif

#define CREATE_TRACE_POINTS
#include <trace/hooks/vendor_hooks.h>
#include <linux/tracepoint.h>
#include <trace/hooks/sched.h>
#include <trace/hooks/cpu.h>
#include <trace/hooks/fpsimd.h>
#include <trace/hooks/binder.h>
#include <trace/hooks/rwsem.h>
#include <trace/hooks/futex.h>
#include <trace/hooks/dtask.h>
#include <trace/hooks/cpuidle.h>
#include <trace/hooks/topology.h>
#include <trace/hooks/gic.h>
#include <trace/hooks/wqlockup.h>
#include <trace/hooks/debug.h>
#include <trace/hooks/printk.h>
#include <trace/hooks/gic_v3.h>
#include <trace/hooks/epoch.h>
#include <trace/hooks/cpufreq.h>
#include <trace/hooks/fs.h>
#include <trace/hooks/i2c.h>
#include <trace/hooks/mm.h>
#include <trace/hooks/preemptirq.h>
#include <trace/hooks/ftrace_dump.h>
#include <trace/hooks/pm_domain.h>
#include <trace/hooks/bug.h>
#include <trace/hooks/timer.h>
#include <trace/hooks/softlockup.h>
#include <trace/hooks/power.h>
#include <trace/hooks/fault.h>
#include <trace/hooks/iommu.h>
#include <trace/hooks/thermal.h>
#include <trace/hooks/ufshcd.h>
#include <trace/hooks/block.h>
#include <trace/hooks/cgroup.h>
#include <trace/hooks/sys.h>
#include <trace/hooks/traps.h>
#include <trace/hooks/avc.h>
#include <trace/hooks/creds.h>
#include <trace/hooks/module.h>
#include <trace/hooks/memory.h>
#include <trace/hooks/typec.h>
#include <trace/hooks/gpiolib.h>
#include <trace/hooks/signal.h>
#include <trace/hooks/logbuf.h>
#include <trace/hooks/vmscan.h>
#include <trace/hooks/psi.h>
#include <trace/hooks/hung_task.h>
#include <trace/hooks/mmc_core.h>
#include <trace/hooks/v4l2core.h>
#include <trace/hooks/v4l2mc.h>
#include <trace/hooks/scmi.h>
#include <trace/hooks/cpuidle_psci.h>
#include <trace/hooks/fips140.h>
#include <trace/hooks/remoteproc.h>
#include <trace/hooks/shmem_fs.h>
#include <trace/hooks/net.h>
#include <trace/hooks/syscall_check.h>
#include <trace/hooks/usb.h>
#include <trace/hooks/ipv6.h>
#include <trace/hooks/sound.h>
#include <trace/hooks/snd_compr.h>
#include <trace/hooks/ipv4.h>
#include <trace/hooks/pci.h>
#include <trace/hooks/dmabuf.h>

/*
 * Export tracepoints that act as a bare tracehook (ie: have no trace event
 * associated with them) to allow external modules to probe them.
 */
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_select_task_rq_fair);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_select_task_rq_rt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_select_fallback_rq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_refrigerator);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_scheduler_tick);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_enqueue_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dequeue_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_can_migrate_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_find_lowest_rq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_rtmutex_prepare_setprio);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_prepare_prio_fork);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_finish_prio_fork);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_user_nice);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_setscheduler);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_arch_set_freq_scale);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_is_fpsimd_save);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_transaction_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_priority_skip);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_set_priority);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_restore_priority);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_wakeup_ilocked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_send_sig_info);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_process_killed);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_killed_process);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_wake);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_write_finished);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alter_rwsem_list_add);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alter_futex_plist_add);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_sleep_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_futex);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wait_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wake_traverse_plist);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wake_this);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wake_up_q_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_wait_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rtmutex_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rtmutex_wait_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_read_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_read_wait_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_write_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_write_wait_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_set_owner);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_set_reader_owned);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_mark_wake_readers);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_up_read_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_up_write_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sched_show_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_shmem_alloc_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpu_idle_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpu_idle_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_find_busiest_group);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_gic_resume);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_wq_lockup_pool);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ipi_stop);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_dump_throttled_rt_tasks);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_printk_hotplug);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_jiffies_update);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_gic_v3_set_affinity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_suspend_epoch_val);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_resume_epoch_val);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_max_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_table_limits);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_resolve_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_fast_switch);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_target);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_newidle_balance);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_nohz_balancer_kick);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_rebalance_domains);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_find_busiest_queue);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_migrate_queued_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_resume_cpus);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_find_energy_efficient_cpu);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_iowait);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_overutilized);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_setaffinity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_update_cpus_allowed);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_skip_swapcache_flags);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_gfp_zone_flags);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_readahead_gfp_mask);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_preempt_disable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_preempt_enable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_irqs_disable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_irqs_enable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_task_cpu);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_try_to_wake_up);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_try_to_wake_up_success);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_fork);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_wake_up_new_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_new_task_stats);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_flush_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_tick_entry);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_schedule);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_cpu_starting);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_cpu_dying);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_account_irq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_place_entity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_build_perf_domains);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_update_cpu_capacity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_update_misfit_status);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_can_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_online);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_fork_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_ttwu_cond);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_schedule_bug);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_exec);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_oops_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_oops_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_size_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_format_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_dump_buffer);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_allow_domain_state);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_map_util_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_report_bug);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_em_cpu_energy);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpu_up);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sched_balance_rt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_timer_calc_index);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_watchdog_timer_softlockup);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_try_to_freeze_todo);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_try_to_freeze_todo_unfrozen);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dequeue_task_idle);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_die_kernel_fault);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_sea);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_mem_abort);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_sp_pc_abort);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_iommu_setup_dma_ops);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_setup_dma_ops);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_alloc_iova);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_iovad_alloc_iova);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_free_iova);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_iovad_free_iova);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_pick_next_entity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_check_preempt_wakeup);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cma_alloc_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cma_alloc_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rmqueue);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_pagecache_get_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_filemap_fault_get_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_filemap_fault_cache_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_enable_thermal_genl_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_thermal_pm_notify_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_fill_prdt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_ufs_complete_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_ufs_reprogram_all_keys);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_prepare_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_update_sysfs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_compl_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_uic_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_tm_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_check_int_errors);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_update_sdev);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_clock_scaling);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_wake_up_sync);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_wake_flags);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_uclamp_eff_get);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpufreq_transition);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cgroup_set_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cgroup_force_kthread_migration);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_syscall_prctl_finished);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_check_preempt_tick);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_replace_next_task_fair);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_sched_yield);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_wait_for_work);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sync_txn_recvd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_build_sched_domains);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alter_mutex_list_add);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_unlock_slowpath);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_unlock_slowpath_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_wake_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_undefinstr);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_ptrauth_fault);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_bad_mode);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_arm64_serror_panic);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_selinux_avc_insert);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_selinux_avc_node_delete);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_selinux_avc_node_replace);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_selinux_avc_lookup);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_blk_alloc_rqs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_blk_rq_ctx_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_commit_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_exit_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_override_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_revert_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_record_mutex_lock_starttime);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_record_rtmutex_lock_starttime);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_record_rwsem_lock_starttime);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_record_pcpu_rwsem_starttime);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_memory_x);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_memory_nx);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_memory_ro);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_memory_rw);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_module_permit_before_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_module_permit_after_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_util_est_update);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_meminfo_proc_show);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_exit_mm);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_get_from_fragment_pool);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_exclude_reserved_zone);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_include_reserved_zone);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_pages_slowpath_begin);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_pages_slowpath_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_mem);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_mapcount_pages);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_traversal_lruvec);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_typec_tcpci_override_toggling);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_typec_tcpci_chk_contaminant);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_typec_tcpci_get_vbus);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_typec_store_partner_src_caps);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_account_task_time);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_irqtime_account_process_tick);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_gpio_block_read);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ep_create_wakeup_source);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_timerfd_create);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_alloc_new_buf_locked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_reply);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_trans);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_binder_transaction);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_preset);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_proc_transaction);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_proc_transaction_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_new_ref);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_del_ref);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_post_init_entity_util_avg);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_typec_tcpm_get_timer);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_typec_tcpm_adj_current_limit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_logbuf);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_logbuf_pr_cont);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_tune_scan_type);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_tune_swappiness);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_shrink_slab_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_handle_failed_page_trylock);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_trylock_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_trylock_clear);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_trylock_get_result);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_page_trylock);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_referenced_check_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_drain_all_pages_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cma_drain_all_pages_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_pcplist_add_cma_pages_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_psi_event);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_psi_group);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpuset_fork);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_cpus_allowed_comm);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sched_setaffinity_early);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_free_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_dup_task_struct);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_after_enqueue_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_after_dequeue_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_enqueue_entity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dequeue_entity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_enqueue_task_fair);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dequeue_task_fair);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sched_stat_runtime_rt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_prepare_update_load_avg_se);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_finish_update_load_avg_se);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_tune_inactive_ratio);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_update_topology_flags_workfn);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_of_i2c_get_board_info);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cgroup_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mm_dirty_limits);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_oom_check_panic);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_uninterruptible_tasks);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_uninterruptible_tasks_dn);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_blk_reset);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_blk_mq_rw_recovery);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sd_update_bus_speed_mode);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_attach_sd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sdhci_get_cd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_gpio_cd_irqt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_save_vmalloc_stack);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_remove_vmalloc_stack);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_stack_hash);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_save_track_hash);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_vmpressure);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_acct_update_power);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_typec_tcpm_log);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_media_device_setup_link);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_media_device_setup_link);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_clear_reserved_fmt_fields);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_fill_ext_fmtdesc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_clear_mask_adjust);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_v4l2subdev_set_selection);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_v4l2subdev_set_selection);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_v4l2subdev_set_fmt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_v4l2subdev_set_fmt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_v4l2subdev_set_frame_interval);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_v4l2subdev_set_frame_interval);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_scmi_timeout_sync);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_find_new_ilb);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_qos_add_request);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_qos_update_request);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_qos_remove_request);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_balance_anon_file_reclaim);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpuidle_psci_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpuidle_psci_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sha256);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_aes_expandkey);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_aes_encrypt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_aes_decrypt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_force_compatible_pre);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_force_compatible_post);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_print_transaction_info);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_setscheduler_uclamp);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rproc_recovery);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rproc_recovery_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ptype_head);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_kfree_skb);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_mmap_file);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_file_open);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_bpf_syscall);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_usb_dev_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_usb_dev_resume);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ipv6_gen_linklocal_addr);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sound_usb_support_cpu_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_snd_compr_use_pause_in_drain);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_snd_soc_card_get_comp_chain);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_show_max_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_tcp_sendmsg_locked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_tcp_recvmsg);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_udp_sendmsg);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_udp_recvmsg);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_tcp_recvmsg_stat);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_pci_d3_sleep);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_kmalloc_slab);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_disable_thermal_cooling_stats);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmap_region);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_update_page_mapcount);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_add_page_to_lrulist);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_del_page_from_lrulist);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_should_be_protected);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mark_page_accessed);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_try_to_unmap_one);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_id_remove);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_css_offline);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_css_online);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_free);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_alloc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_subpage_dma_contig_alloc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_looper_state_registered);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_thread_read);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_free_proc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_thread_release);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_has_work_ilocked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_read_done);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_handle_tlb_conf);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_shrink_node_memcgs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ra_tuning_max_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_handle_pte_fault_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cow_user_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_swapin_add_anon_rmap);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_waiting_for_page_migration);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_migrate_page_states);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_referenced_one_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_count_pswpin);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_count_pswpout);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_count_swpout_vm_event);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_swap_slot_cache_active);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_drain_slots_cache_cpu);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_swap_slot_cache);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_free_swap_slot);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_get_swap_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_madvise_cold_or_pageout);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_isolated_for_reclaim);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_inactive_is_low);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_snapshot_refaults);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_account_swap_pages);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_unuse_swap_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_init_swap_info_struct);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_si_swapinfo);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_si);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_free_pages);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_shmem_page_flag);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sched_pelt_multiplier);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_page_look_around_ref);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_look_around);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_look_around_migrate_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_test_clear_look_around_ref);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dma_buf_stats_teardown);
/*
 * For type visibility
 */
#ifdef CONFIG_ARM64
#include <linux/irqchip/arm-gic-v3.h>
const struct gic_chip_data *GKI_struct_gic_chip_data;
EXPORT_SYMBOL_GPL(GKI_struct_gic_chip_data);
#endif

#include <linux/swap_slots.h>
const struct swap_slots_cache *GKI_struct_swap_slots_cache;
EXPORT_SYMBOL_GPL(GKI_struct_swap_slots_cache);
