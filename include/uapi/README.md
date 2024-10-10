# UAPI Documention

### 什么是 UAPI

- 在 ACO (A Cool OS) 里面， UAPI 和 Linux 的 UAPI 的意义是一致的， UAPI 即为给 User Program 使用的 API , 具体的有 syscall 的参数的数据类型，比如 struct dents 等等数据类型，这些是 User Program 在没有 LIBC 的 Linux 上任然可以使用的头文件

### 为什么需要 UAPI

- ACO 内核希望能够兼容运行各个 Linux 发行版的 rootfs ，自然会对标 Linux 实现的 syscall 。在所有 Linux 发行版 （内核跑的是 Linux )，默认下面的是 Linux Kernel ,  自然的会在开发的时候使用 Linux 提供的UAPI头文件 （当然， Linux 对标的是 POSIX ）
-  Linux 项目可以使用 make headers_install 去把 uapi 的头文件安装到  Linux 的 rootfs 的 /usr/include 目录里面，供用户程序使用。ACO 内核不用去做这个事情，因为我们不会在用户态的软件做文章，我们只是希望能够借用 Linux 发行版的 rootfs 来跑 ACOS。

### 为什么不直接 COPY Linux 项目的 UAPI

- ACO Kernel 的 UAPI 只用于对接 User Program Syscall 上来的各个参数，并不像真正的 Linux UAPI 那样，是专门给 LInux 发行版的 User 态的用户程序准备的可用头文件

### 一句话概括 UAPI  的作用

- 内核和用户程序共用的头文件，通常是共用数据类型，宏，而非函数。

### 为啥会分ARCH

- 不同 架构ARCH 上的 UAPI 不一样 ( 体现在 asm 里面 )，比如 riscv syscall id table 和  x86 syscall id table 不一样。 ACO Kernel 希望提供统一的 UAPI 头文件使用，不用再去 #if 判断一下架构，去包不同的头文件

### ACO Kernel 视图

```c
|- xxx.h
|- yyy.h
|- ...
|- asm
|	|- aaa.h
|	|- bbb.h
|	|- ...
```



### ACO Kernel 使用 UAPI 的流程

1. 编译内核前
   1. 删除 uapi/asm 目录 （之前的可能遗留物）
   2. 根据选中的 ARCH 将 uapi/${ARCH}/asm 复制到 uapi目录
2. 编译内核中
   1. 内核里面关于用户态过来的 syscall 的参数和返回值的数据结构从 UAPI 中找
3. 编译内核后
   1. clean target 会删除掉产生的 uapi/asm 目录 



### Note : 不同 arch 的uapi 差异全部体现在 asm 目录里面





