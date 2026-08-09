# 定义最终生成的内核模块及其依赖文件
obj-m += droidspace_fix.o
droidspace_fix-objs := main.o

# 指定头文件搜索路径，确保能找到 include/kp_api.h
EXTRA_CFLAGS += -I$(PWD)/include

# 默认编译目标，由 GitHub Action 注入 KDIR 环境变量
# 注意这里新增了 KBUILD_MODPOST_WARN=1 来忽略未定义符号的致命错误
all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm64 CC=clang CROSS_COMPILE=aarch64-linux-gnu- LLVM=1 LLVM_IAS=1 KBUILD_MODPOST_WARN=1 modules

# 清理编译产物
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
