## Android 进阶系列： Android 卡顿（一）：总述
Android 卡顿系列是根据极客时间的专栏《Android开发高手课》 整理而成的。


卡顿的原因一般是主线程执行繁重的 UI 绘制、大量的计算或者 I/O 等耗时操作。
监控卡顿需要获取卡顿时的堆栈和当时 CPU 运行的一些信息，卡顿的最后都会反映到 CPU 时间上；

监控卡顿方法一般分为两个类型，一是监控主线程，另外一个计算函数的耗时；

#### 一、监控主线程
监控主线程的主要思想是，通过监控主线程执行耗时，当超过阈值时， dump 出当前主线程的执行堆栈，通过分析堆栈找到卡顿原因。
例如开源的 BlockCannary, ArgusAPM, LogMonitor.

根据这个思想实现的方法实现有两种：

1. 依赖主线程 Looper, 监控每次 dispatchMessage 的耗时。
    例如 [BlockCanary](https://github.com/markzhai/AndroidPerformanceMonitor), [滴滴的 DoraemonKit](https://github.com/didi/DoraemonKit)
2. 依赖 Choreographer 模块，监控相邻两次 Vsync 事件通知的时间差。
   例如 [360的ArgusAPM](https://github.com/Qihoo360/ArgusAPM/blob/master/argus-apm/argus-apm-main/src/main/java/com/argusapm/android/core/job/fps/FpsTask.java)，[微信读书（Android）阅读引擎卡顿监控测试](https://infoq.cn/article/weixin-reading-stuck-monitor-and-test)
    
- 优点： 方便回去到卡顿的堆栈
- 缺点： 无法去到各个函数的执行耗时

#### 二、计算函数的耗时
主要思想是对函数进行打点插桩。这种方案不进能准确捕获卡顿堆栈，也可以计算出各个函数的执行耗时情况。但是，它比较复杂。

主要有两种方法：

 1. 使用 Trace 功能(Debug.startMethodTracing)
 
 在应用启动时, 默认打开 Trace 功能（Debug.startMethodTracing）,应用内所有函数在执行前后都会经过该函数，通过 hack 手段代理该函数，在每个执行方法前后进行打点记录。
 
     - 优点：能统计到包括系统函数在内的所有函数，对代码或字节码不用做任何修改；
     - 缺点：方式比较 hack ，在兼容性和安全性上存在一定的风险；
          
 2. 修改字节码的方式，在编译期修改所有 class 文件中的函数入口，对所有函数前后进行打点插桩。
    - 优点：兼容性比较好
    - 缺点：无法统计系统函数；对略微增加 apk 的函数数量，对增大 apk 的包。
    - 例子： 微信的 [Matrix-TranceCannary](https://github.com/Tencent/matrix)

#### 三、卡顿排查的工具
##### 1.  TraceView
使用 [TraceView](https://developer.android.com/studio/profile/generate-trace-logs) 创建跟踪日志，在希望开始记录的位置调用 startMethodTracing

- 优点：可以获取所有函数的调用时间
- 缺点：应用的运行速度会变慢，一般用于调试阶段
- 应用场景：分析整个程序执行流程的耗时
- 例子：

```java
    // 1. 保存日志
    // 开始记录的地方
    SimpleDateFormat date = new SimpleDateFormat("dd_MM_yyyy_hh_mm_ss");
    String logDate = date.format(new Date());
    Debug.startMethodTracing("simple-" + logDate);

    ...

   // 记录结束的地方
    Debug.stopMethodTracing();
```
查看日志：
- 如果是在模拟机上，产生的日志在 sdcard/android/data/包名/files/ 路径下，双击即可打开
- 如果是在真机上，则可以使用命令将文件复制出来
> adb pull 文件在设备上的路径/xxx.trace

##### 2. Nanoscope
Uber 开源的 [Nanoscope](https://github.com/uber/nanoscope)
原理是直接修改 Android 虚拟机源码，在 ArtMethod 执行入口和执行结束位置增加埋点代码，将所有的信息先写到内存，等到 trace 结束后才统一生成结果文件。

- 优点：
    - 性能损耗比较少；
    - 可用于分析任意一个应用；
- 缺点：
    - 需要自己刷 ROM
    - 默认只支持主线程采集，其他线程需要手动设置代码
    
##### 3. Systrace 
 [systrace](https://source.android.com/devices/tech/debug/systrace?hl=zh-cn) 可以跟踪系统的 I/O 操作、 CPU负载、 Sufrace 渲染、 GC 等事件
 
 - 优点：
    - 性能开销非常低
 
 - 缺点：
    - 只能监控特点系统调用耗时情况， 不支持应用程序代码的耗时分析
    
 - 应用场景：需要分析系统调用    
 
##### 4. Simpleperf 
[Simpleperf](https://android.googlesource.com/platform/system/extras/+/master/simpleperf/doc/README.md) 是 Android 5.0 新增的性能分析工具， 利用 CPU 的性能监控监控单元（PMU）提供的硬件 perf 事件， 它可以看到所有 Native 代码的耗时；

- 应用场景：需要分析 Native 代码耗时的选择

##### 5. Profiler 
[Profiler](https://developer.android.com/studio/profile?hl=zh-cn) 是 Android Studio 3.2 提供的可视化的性能分析工具，它可以分析 CPU 的活动活动和函数跟踪、网络的检测以及电池的使用分析。

#### 四、参考
1.[极客时间，卡顿优化（上）：你要掌握的卡顿分析方法](https://time.geekbang.org/column/article/71982)



       

