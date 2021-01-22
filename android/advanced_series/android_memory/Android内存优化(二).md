# Android 内存优化（二）

## 4.监控内存泄露


## 4.1 内存如何泄露
为什么会发生内存泄露。
因为堆内存中的长生命周期的对象持有短生命周期对象的强/软引用，尽管短生命周期对象已经不再需要，但是因为长生命周期对象持有它的引用而导致不能被回收，这就是 Java 中内存泄露的根本原因。

Java 的内存分配是由程序完成的，释放有垃圾回收器完成的。

Java 的内存垃圾回收机制（Garbage Collection）是从程序的主要运行对象开始检测引用链，遍历所有的对象，回收没有不再被引用的对象，进行释放。

GC 过程与对象的引用类型相关

| 级别 | 回收时机  | 用途 | 生存时间 |
| --- | --- | --- | --- |
| StrongReference 强| 从来不会 | 对象的一般状态 | JVM 停止运行时终止|
| SoftReference 软|在内存不足时 |联合 ReferenceQueue 构造有效期短、占内存大,生命周期长的对象的二级高速缓冲器（内存不足才清空） | 内存不足事终止|
| WeakReference 弱|在垃圾回收时|联合 ReferenceQueue 构造有效期短、占内存大,生命周期长的对象的一级高速缓冲器（系统发送 gc 才清空） | GC 运行后终止|
| PhatomReference 虚|在垃圾回收时| 联合 ReferenceQueue 来跟踪对象被垃圾回收器回收的活动|GC 运行后终止 |


## 4.2 监控内存泄露

内存监控常用的有 LeakCanary, Probe 和 KOOM. 也可以利用 Android Studio 自带的 Profiler.


### 4.2.1 LeakCanary
线下监控泄露常用有[leakcanary](https://github.com/square/leakcanary)，
Leackcanary 的检测内存泄露的主要原理是，通过监控泄露，利用了弱引用的特性，为 Activity/Fragment 创建弱引用，弱引用会加入都引用队列中，通过 Activity.onDestroy() 后连续触发两次 GC, 并检查引用队列，判定 Activity/Fragment 是否泄露。

它的优点是实现比较简单，但是因为是要经常 dump 堆栈的 hprof 文件，造成应用的卡顿，所以一般不适合线上使用

### 4.2.2 美团 Probe
美团的 [Probe](https://tech.meituan.com/2019/11/14/crash-oom-probe-practice.html) 在 Leackcanary 的基础上进行了改造.

Probe 重新设置了 OOM 监控模块，在一个后台线程每隔 1s 去获取主进程的内存占用，当内存占用达到设定的阈值时，就去执行 dump 函数，得到内存快照文件。这样就不会像 Leakcanary 那样经常 dump 内存快照，减少 由于 dump 导致的卡顿

得到快照文件后，在一个新的进程使用开源库 HAHA 分析 hprof 文件，通过优化 HAHA 裁剪逻辑。

优点是不会经常 dump 文件，减少卡顿，可以用于线上。但是缺点也是很明显，没有根本上解决 dump 导致的 APP 冻结，并且 HAHA 分析线程自身容易出现 OOM，还有就是 Probe 没有开源，自己实现有一定难度。

### 4.2.3 快手 KOOM
快手的 [KOOM](https://juejin.im/post/6860014199973871624) 在 LeakCanary、美团的 Probe 的基础上进一步做了优化。KOOM 整个组件分成 监控模块、采集模块、解析模块和问题反馈模块，其中 问题反馈模块 是后端侧，没有开源。

在解析 hprof 文件，对关键对象进行可达性分析时，利用 LeakCanary 2.0 新推出的解析组件 Shark 解析。 Shark 做了优化，解析比 HAHA 高效很多，提高了性能。

在解析完 hprof 文件后就可以得到解析结果的 json 文件，将这个文件可以上传到服务端。

KOOM 可以说是 Probe 的实现版本，只不过在 dump 利用了 fork 进程的
 copy_on_write 机制，同时在解析使用了 Shark 替换 HAHA 进行优化。

KOOM 需要在 dump 过程中 hook 虚拟机的 write 函数，缺点是由于不同 Android 版本的虚拟机不一样，目前支持都 Android Q 版本，暂不支持 Android R 版本.

## 4.2.4 Profile
参考 [Android Android Studio Profiler 的使用](https://github.com/yxhuangCH/CSLearn/blob/master/android/advanced_series/android_block/Android%E5%8D%A1%E9%A1%BF-Profiler.md)


## 5.优化内存


### 5.1 排查方法
- 使用 MAT
- 使用 



