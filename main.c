#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/string.h>
#include "kp_api.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("clcwpwqi");
MODULE_DESCRIPTION("DroidSpace UserNS Fix for GKI 6.6");

// 保存原函数的函数指针
static int (*orig_cap_capable)(const struct cred *cred, struct user_namespace *targ_ns, int cap, unsigned int opts);

// 自定义的 Hook 函数
static int my_cap_capable(const struct cred *cred, struct user_namespace *targ_ns, int cap, unsigned int opts) {
    char comm[TASK_COMM_LEN];
    get_task_comm(comm, current);

    // 匹配 DroidSpace 的包名或进程名
    if (strncmp(comm, "io.droidspace", 13) == 0 || strncmp(comm, "droidspace", 10) == 0) {
        // 放行 UserNS 创建和容器挂载所需的核心权限
        if (cap == CAP_SYS_ADMIN || cap == CAP_SETUID || cap == CAP_SETGID || cap == CAP_SYS_CHROOT) {
            return 0; 
        }
    }

    // 对于其他进程或未涉及的权限请求，调用原内核函数正常处理
    if (orig_cap_capable) {
        return orig_cap_capable(cred, targ_ns, cap, opts);
    }
    
    return -EPERM;
}

static int __init kpm_droidspace_init(void) {
    void *target_addr;
    int ret;

    // 在 6.6 内核中查找函数地址
    target_addr = kp_kallsyms_lookup_name("cap_capable");
    if (!target_addr) {
        pr_err("[DroidSpace_KPM] Failed to find cap_capable address\n");
        return -ENODEV;
    }

    // 执行 Inline Hook
    ret = kp_hook(target_addr, (void *)my_cap_capable, (void **)&orig_cap_capable);
    if (ret < 0) {
        pr_err("[DroidSpace_KPM] Hook failed with error: %d\n", ret);
        return ret;
    }

    pr_info("[DroidSpace_KPM] Successfully hooked cap_capable for Kernel 6.6\n");
    return 0;
}

static void __exit kpm_droidspace_exit(void) {
    void *target_addr = kp_kallsyms_lookup_name("cap_capable");
    if (target_addr) {
        // 解除 Hook
        kp_unhook(target_addr);
        pr_info("[DroidSpace_KPM] Unhooked cap_capable\n");
    }
}

module_init(kpm_droidspace_init);
module_exit(kpm_droidspace_exit);
