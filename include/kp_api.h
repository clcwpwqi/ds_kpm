#ifndef _KP_API_H
#define _KP_API_H

// APatch 提供的 KPM 接口声明
extern void *kp_kallsyms_lookup_name(const char *name);
extern int kp_hook(void *target, void *hook, void **orig);
extern int kp_unhook(void *target);

#endif // _KP_API_H
