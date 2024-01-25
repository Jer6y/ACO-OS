# LOS_xv6-like-system
write a OS like xv6 step by step <br>
bro 这个仓库是相当随意的 <br>
这是Jer6y 的个人练手项目 [或许有那么一点含金量(至少我是这么认为的)] <br>
意义在于完善Jer6y关于一个软件的架构到设计的全过程 <br>
2024-01-22 开启第二阶段 - 坚持不咕 去年咕掉了 <br>
还有就是，在第二阶段，jer6y熟悉RTOS 所以会发现此项目的结构会和RTOS非常像 <br>
可能以后会有第三阶段啥的 坚决不咕 <br>
硬件 : riscv64 virt SMP架构 核心数为4 <br>
软件支持: <br>
CLINT驱动      √ <br>
PLIC驱动       √ <br>
UART驱动       √ <br>
spinlock       √ <br>
内存管理        √ 【buddy 来分配大块页 slab 分配小对象 具体查看 jerry的buddy_system仓库】 <br>
tick中断处理    √ 【全部代理给核心号为3的核做，只运行在M态】 <br>
软件定时器      √ <br>


