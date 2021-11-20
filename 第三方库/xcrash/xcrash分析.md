
[TOC]

# 简述
XCrash 的崩溃捕获分为, java 崩溃捕获，Native 崩溃捕获 和 Anr 崩溃捕获。

这是 xcrash 是的架构图
<image src="image/architecture.png">
图片来自 [xCrash 的 github](https://github.com/iqiyi/xCrash)

- JavaCrashHandler 捕获 Java 崩溃，handleException 处理 java 崩
- NativeHandler 捕获 native 崩溃
- AnrHandler 捕获 ABI < 21 的 anr ,高于等于 21 的在 native 层捕获


# 1 Java　崩溃的捕获

# 2. Native 崩溃的捕获

Native 崩溃的捕获主要是两方面：
- 一是捕获到崩溃信号
- 二是堆栈的回溯，还原崩溃现场

<image src="image/capture_native_crash.png">

图片来自 [xCrash 的 github](https://github.com/iqiyi/xCrash)

## 2.1 Native 崩溃捕获的时序

<image src="image/xcrash_1.png">

- **初始化**

NativeHandler#initialize
加载 xcrash.so
初始化 nativeInit

```java
int initialize(...){

    ... 
     
    System.loadLibrary("xcrash");  

    ...


    int r = nativeInit(...);
}

```

- **xc_jni.c初始化**

xc_jni.c 文件

```c
static jint xc_jni_init(..){

    ...

    xc_common_init(...)

    ...

    if(crash_enable){
         if(crash_dump_all_threads_whitelist){

             ...

         }

          //crash init
        // 初始化的重点
        r_crash = xc_crash_init(...)
    }

    if(trace_enable)
    {
        //trace init
        r_trace = xc_trace_init(...);
    }

    ...
}
```

这是 Native 的入口
- xc_common_init 
这是初始化一下公共信息，例如版本、 ABI、app_version 版本等信息

- crash_dump_all_threads_whitelist
设置崩溃是 dump 白名单线程

- xc_crash_init
这个是初始化的重点, 后面会详细讲述

- xc_trace_init

## 2.2 xc_crash_init
xc_crash.c

```c
int xc_crash_init(...)
{

    xc_crash_prepared_fd = XCC_UTIL_TEMP_FAILURE_RETRY(open("/dev/null", O_RDWR));
    xc_crash_rethrow = rethrow;

    
    if(NULL == (xc_crash_emergency = calloc(XC_CRASH_EMERGENCY_BUF_LEN, 1))) return XCC_ERRNO_NOMEM;
    if(NULL == (xc_crash_dumper_pathname = xc_util_strdupcat(xc_common_app_lib_dir, "/"XCC_UTIL_XCRASH_DUMPER_FILENAME))) return XCC_ERRNO_NOMEM;

    //init the local unwinder for fallback mode
    // ① 根据不同的 API 版本，初始化堆栈回溯 so
    xcc_unwind_init(xc_common_api_level);

    //init for JNI callback
    // ② 设置回调， 回调到 NativeHandler 的 crashCallback 方法
    xc_crash_init_callback(env);

    //struct info passed to the dumper process
    // dump 时使用的数据结构
    memset(...)

    //③ for clone and fork
#ifndef __i386__
    if(NULL == (xc_crash_child_stack = calloc(XC_CRASH_CHILD_STACK_LEN, 1))) return XCC_ERRNO_NOMEM;
    xc_crash_child_stack = (void *)(((uint8_t *)xc_crash_child_stack) + XC_CRASH_CHILD_STACK_LEN);
#else
    // pipe2 创建管道
    if(0 != pipe2(xc_crash_child_notifier, O_CLOEXEC)) return XCC_ERRNO_SYS;
#endif
    
    //register signal handler
    // ④ 注册信号捕获 handler
    return xcc_signal_crash_register(xc_crash_signal_handler);
}
```
**xc_crash_init 是核心方法**

- xc_crash_prepared_fd 是为崩溃时预留存储文件

- ①  xcc_unwind_init， 根据不同的 API 版本，初始化堆栈回溯 so
    - 16 >= API Level <= 20, 使用 libcorkscrew.so
    - 21 >= API Level <= 23, 使用 libunwind.so 
    - APL Level >= 24, 可以使用 _Unwind_Backtrace 直接调用

- ② xc_crash_init_callback， 设置回调， 回调到 NativeHandler 的 crashCallback 方法

- ③ for clone and fork， 对于 arm 架构来说，是使用 pipe2 创建管道，用来通知崩溃的信号

- ④ xcc_signal_crash_register， 注册信号捕获 handler，handler 是 xc_crash_signal_handler

## 2.3 xcc_signal_crash_register
xcc_signal.c

```c
// 需要监听的信号
static xcc_signal_crash_info_t xcc_signal_crash_info[] =
{
    {.signum = SIGABRT},
    {.signum = SIGBUS},
    {.signum = SIGFPE},
    {.signum = SIGILL},
    {.signum = SIGSEGV},
    {.signum = SIGTRAP},
    {.signum = SIGSYS},
    {.signum = SIGSTKFLT}
};

int xcc_signal_crash_register(void (*handler)(int, siginfo_t *, void *))
{
    // ① 设置而外的栈空间
    stack_t ss;
    if(NULL == (ss.ss_sp = calloc(1, XCC_SIGNAL_CRASH_STACK_SIZE))) return XCC_ERRNO_NOMEM;
    ss.ss_size  = XCC_SIGNAL_CRASH_STACK_SIZE;
    ss.ss_flags = 0;
    if(0 != sigaltstack(&ss, NULL)) return XCC_ERRNO_SYS;

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigfillset(&act.sa_mask);
    act.sa_sigaction = handler;
    act.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
    
    size_t i;
    for(i = 0; i < sizeof(xcc_signal_crash_info) / sizeof(xcc_signal_crash_info[0]); i++)
        if(0 != sigaction(xcc_signal_crash_info[i].signum, &act, &(xcc_signal_crash_info[i].oldact)))
            return XCC_ERRNO_SYS;

    return 0;
}

```
① 设置而外空间
- SIGSEGV 有可能是栈溢出引起的，如果在默认的栈上运行很用可能会破坏程序运行的现场，无法获得正确的上下文。而且当栈满了，系统会在同一个已经满了的栈上调用 SIGSEGV 的信号处理函数，又再一次引起同样的信号。
- 开辟一块新的空间作为运行信号处理函数的栈。可以使用 sigaltstack 在任意线程注册一个可选的栈，保留一下载紧急情况下使用的空间。
出自 [https://mp.weixin.qq.com/s/g-WzYF3wWAljok1XjPoo7w?]


## 2.4 xc_crash_signal_handler

**xc_crash.c#xc_crash_signal_handler**

<image src="image/xcrash_2.png">
<image src="image/xcrash_3.png">

### xc_crash_spot
① 设置 crash 时的信息、包括崩溃时间、崩溃线程等

```c
//set crash spot info
// 设置崩溃的时间
xc_crash_spot.crash_time = xc_crash_time;
// 崩溃的线程 ID
xc_crash_spot.crash_tid = xc_crash_tid;
memcpy(&(xc_crash_spot.siginfo), si, sizeof(siginfo_t));
memcpy(&(xc_crash_spot.ucontext), uc, sizeof(ucontext_t));
// 崩溃log 文件路径长度
xc_crash_spot.log_pathname_len = strlen(xc_crash_log_pathname);
```

### xc_crash_fork
② fork 一个进程都用了 dump 内容 xc_crash_exec_dumper

```c
static int xc_crash_fork(int (*fn)(void *))
{
#ifndef __i386__
    return clone(fn, xc_crash_child_stack, CLONE_VFORK | CLONE_FS | CLONE_UNTRACED, NULL);
#else
    // fork 一个进程
    pid_t dumper_pid = fork();
    ...

#endif
}
```

### xc_crash_exec_dumper
 xc_crash_exec_dumper 是在子进程中执行了，该方法有两个功能：
- 一是, 设置 pipe 管道参数，set args pipe size
- 二是，加载 libxcrash_dumper.so 文件 ③

```c
static int xc_crash_exec_dumper(void *arg)
{
    ...

     //set args pipe size
    // 设置管道参数
    //range: pagesize (4K) ~ /proc/sys/fs/pipe-max-size (1024K)
    int write_len = (int)(sizeof(xcc_spot_t) +
                          xc_crash_spot.log_pathname_len +
                          xc_crash_spot.os_version_len +
                          xc_crash_spot.kernel_version_len +
                          xc_crash_spot.abi_list_len +
                          xc_crash_spot.manufacturer_len +
                          xc_crash_spot.brand_len +
                          xc_crash_spot.model_len +
                          xc_crash_spot.build_fingerprint_len +
                          xc_crash_spot.app_id_len +
                          xc_crash_spot.app_version_len +
                          xc_crash_spot.dump_all_threads_whitelist_len);

    ...

    //escape to the dumper process
    errno = 0;
    // 加载 "libxcrash_dumper.so"
    // XCC_UTIL_XCRASH_DUMPER_FILENAME "libxcrash_dumper.so"
    execl(xc_crash_dumper_pathname, XCC_UTIL_XCRASH_DUMPER_FILENAME, NULL);
    return 100 + errno;
}
```

### xcd_core.c#main; 
④ 从 xcd_core.c 的 main方法开始执行
当加载 libxcrash_dumper.so, 开始执行的方法是 xcd_core.c#main 方法
main 方法里面包含了几个步骤：
- xcc_unwind_init, 初始化堆栈回溯，为了捕获 dump 子进程可能发生的崩溃
- xcc_signal_crash_register, 注册 signal, 为了捕获 dump 子进程可能发生的崩溃
- xcd_process_create，统计崩溃进程的所有线程信息
- xcd_process_suspend_threads, 挂起该进程 的所有线程
- xcd_process_load_info, 获取进程的信息，包含进程、线程和内存映射信息
- xcd_sys_record, 记录系统信息，包含 ABI Version 版本等
- xcd_process_record, 记录进程信息
- xcd_process_resume_threads,恢复所有线程

### xcd_core.c#xcd_process_load_info
xcd_process_load_info 最终通过 xcd_maps_create 方法获取进程的内存映射信息

```c
int xcd_maps_create(xcd_maps_t **self, pid_t pid)
{
    ... 
    snprintf(buf, sizeof(buf), "/proc/%d/maps", pid);

    ...

    xcd_maps_parse_line(buf, &mi)))

    ...
}

```

### xcd_process_record
⑥ 记录进程信息
这个方面里面是记录进程里面的主要信息

```c
int xcd_process_record(...){

    ...

 // 循环
 TAILQ_FOREACH(thd, &(self->thds), link)
    {
        if(thd->t.tid == self->crash_tid)
        {
            // 获取进程的 id, 名称 和 线程 id
            if(0 != (r = xcd_thread_record_info(&(thd->t), log_fd, self->pname))) return r;
            // 获取 signal 信号信息
            if(0 != (r = xcd_process_record_signal_info(self, log_fd))) return r;
            // 获取 Abort message
            if(0 != (r = xcd_process_record_abort_message(self, log_fd, api_level))) return r;
            // 获取寄存器信息
            if(0 != (r = xcd_thread_record_regs(&(thd->t), log_fd))) return r;
            // 获取线程每帧的信息，里面包含 Elf 的解析
            if(0 == xcd_thread_load_frames(&(thd->t), self->maps))
            {
                // 获取 backtrace 堆栈信息
                if(0 != (r = xcd_thread_record_backtrace(&(thd->t), log_fd))) return r;
                // 获取 so 的 build id， 时间
                if(0 != (r = xcd_thread_record_buildid(&(thd->t), log_fd, dump_elf_hash, xcc_util_signal_has_si_addr(self->si) ? (uintptr_t)self->si->si_addr : 0))) return r;
                // 获取 stack 信息
                if(0 != (r = xcd_thread_record_stack(&(thd->t), log_fd))) return r;
                // 获取内存信息
                if(0 != (r = xcd_thread_record_memory(&(thd->t), log_fd))) return r;
            }
            // 内存 map
            if(dump_map) if(0 != (r = xcd_maps_record(self->maps, log_fd))) return r;
            // 系统 logcat 信息
            if(0 != (r = xcc_util_record_logcat(log_fd, self->pid, api_level, logcat_system_lines, logcat_events_lines, logcat_main_lines))) return r;
            // 文件句柄 fd
            if(dump_fds) if(0 != (r = xcc_util_record_fds(log_fd, self->pid))) return r;
            // 网络信息
            if(dump_network_info) if(0 != (r = xcc_util_record_network_info(log_fd, self->pid, api_level))) return r;
            // 内存信息
            if(0 != (r = xcc_meminfo_record(log_fd, self->pid))) return r;

            break;
        }
    }
    ...
}
```
- **获取进程的 id, 名称 和 线程 id**

```
pid: 20501, tid: 20501, name: xcrash.sample  >>> xcrash.sample <<<
```

-  **获取 signal 信号信息**
```
signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0
```
-  **获取 Abort message**

```
Abort message: 'Check failed: key_value != nullptr compiler-filter not found in oat header'
```
- **获取寄存器信息**

```
r0  00000000  r1  00003d49  r2  00000006  r3  a2787e90
r4  a2787ea4  r5  a2787e88  r6  00003d3f  r7  0000016b
r8  a2787ea0  r9  a2787e90  r10 a2787ec0  r11 a2787eb0
ip  00003d49  sp  a2787e60  lr  b29afc33  pc  b29afc46
```
- **获取 backtrace 堆栈信息**

```
backtrace:
    #00 pc 0005ec46  /apex/com.android.runtime/lib/bionic/libc.so (abort+165)
    #01 pc 0037ced1  /apex/com.android.runtime/lib/libart.so (_ZN3art7Runtime5AbortEPKc+1600)
    #02 pc 0000857d  /system/lib/libbase.so (_ZN7android4base10LogMessageD2Ev+412)
    #03 pc 00329937  /apex/com.android.runtime/lib/libart.so (_ZNK3art9OatHeader17GetCompilerFilterEv+170)
    #04 pc 0032f241  /apex/com.android.runtime/lib/libart.so (_ZNK3art7OatFile17GetCompilerFilterEv+20)
    #05 pc 003377e1  /apex/com.android.runtime/lib/libart.so (_ZN3art14OatFileManager14DumpForSigQuitERNSt3__113basic_ostreamIcNS1_11char_traitsIcEEEE+248)
    #06 pc 00387035  /apex/com.android.runtime/lib/libart.so (_ZN3art7Runtime14DumpForSigQuitERNSt3__113basic_ostreamIcNS1_11char_traitsIcEEEE+68)
    #07 pc 00396227  /apex/com.android.runtime/lib/libart.so (_ZN3art13SignalCatcher13HandleSigQuitEv+1026)
    #08 pc 0039567f  /apex/com.android.runtime/lib/libart.so (_ZN3art13SignalCatcher3RunEPv+246)
    #09 pc 000a6077  /apex/com.android.runtime/lib/bionic/libc.so (_ZL15__pthread_startPv+20)
    #10 pc 00060131  /apex/com.android.runtime/lib/bionic/libc.so (__start_thread+30)
```

-  **获取 so 的 build id、md5 和时间**

```
build id:
    /apex/com.android.runtime/lib/bionic/libc.so (BuildId: 00be8c4fc1e3ff439b7de04fef5dc81b. FileSize: 987864. LastModified: 2009-01-01T07:00:00.000+0700. MD5: e8f59e3f6da4b10a0c60dd960c2db4a6)
    /apex/com.android.runtime/lib/libart.so (BuildId: 02ff9b2d73bf74036eb619d1893bf752. FileSize: 6020200. LastModified: 2009-01-01T07:00:00.000+0700. MD5: 50b1a5e915af28db70cd41298ff7b61e)
    /system/lib/libbase.so (BuildId: d61356f7fd581149bcf4b8d8caeb7d8a. FileSize: 63180. LastModified: 2009-01-01T07:00:00.000+0700. MD5: de03c2d4fc8f0659516ae0cbf4686db0)
```

-  **获取 stack 信息**
```
stack:
         a2787e20  00000002
         a2787e24  b2a07260  [anon:.bss]
         a2787e28  00000000
         a2787e2c  6ee08ad0  [anon:libc_malloc]
         a2787e30  b06e1217  /apex/com.android.runtime/lib/libart.so
         a2787e34  09b33a2f  /system/bin/app_process32 (sigprocmask64+70)
         a2787e38  00000000
         a2787e3c  b2a07260  [anon:.bss]
         a2787e40  b47037a4  [anon:libc_malloc]
         a2787e44  345acf15  [anon:dalvik-main space (region space)]
         a2787e48  a2787f70
         a2787e4c  a2787e88
         a2787e50  a2787f70
         a2787e54  a2787e88
         a2787e58  00003d3f
         a2787e5c  b44b8190  [anon:.bss]
    #00  a2787e60  a2787ed0
         a2787e64  a2787ee0
```

- **获取内存信息**

```
memory near r1:
    00003d28 -------- -------- -------- --------  ................
    00003d38 -------- -------- -------- --------  ................
    00003d48 -------- -------- -------- --------  ................
    00003d58 -------- -------- -------- --------  ................
    00003d68 -------- -------- -------- --------  ................
    00003d78 -------- -------- -------- --------  ................
    00003d88 -------- -------- -------- --------  ................
    00003d98 -------- -------- -------- --------  ................
    00003da8 -------- -------- -------- --------  ................
    00003db8 -------- -------- -------- --------  ................
```

- **内存映射 map**

```
memory map:
    00580000-005c0000 rw-        0    40000 [anon:v8]
    005ed000-00dee000 rw-        0   801000 [anon:libc_malloc]
    00dee000-00def000 ---        0     1000 
    00def000-00df0000 ---        0     1000 
    00df0000-00ef6000 rw-        0   106000 
    00ef6000-00ef8000 ---        0     2000 
    00ef8000-00ef9000 ---        0     1000 
    00ef9000-00fff000 rw-        0   106000 
    00fff000-01000000 ---        0     1000 
    01000000-01001000 ---        0     1000 [anon:partition_alloc]
```

- **系统 logcat 信息**

```
logcat:
--------- tail end of log main (/system/bin/logcat -b main -d -v threadtime -t 200 --pid 6875 *:D)
10-11 14:52:04.026  6875  6875 I xcrash.sample: Not late-enabling -Xcheck:jni (already on)
10-11 14:52:04.060  6875  6875 E xcrash.sample: Unknown bits set in runtime_flags: 0x8000
10-11 14:52:04.065  6875  6875 W xcrash.sample: Unexpected CPU variant for X86 using defaults: x86_64
10-11 14:52:04.613  6875  6875 D xcrash_sample: xCrash SDK init: start
10-11 14:52:04.622  6875  6875 D xcrash_sample: xCrash SDK init: end
10-11 14:52:04.676  6875  6906 D libEGL  : Emulator has host GPU support, qemu.gles is set to 1.
```

- **文件句柄 fd**
       
```
open files:
    fd 0: /dev/null
    fd 1: /dev/null
    fd 2: /dev/null
    fd 3: socket:[95117]
    fd 4: /sys/kernel/debug/tracing/trace_marker
    fd 5: /dev/null
    fd 6: /dev/null
    fd 7: /dev/null
    fd 8: /dev/binder
    fd 9: /apex/com.android.runtime/javalib/core-oj.jar
    fd 10: /apex/com.android.runtime/javalib/core-libart.jar
```

- **网络信息**

```
network info:
 TCP over IPv4 (From: /proc/PID/net/tcp)
  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode
  0: 0100007F:CF99 00000000:0000 0A 00000000:00000000 00:00000000 00000000 10133        0 342943 1 00000000 100 0 0 10 0
  1: F326C70A:8B28 5E0AFB8E:01BB 01 00000000:00000000 00:00000000 00000000 10127        0 385678 1 00000000 27 4 29 10 -1
  2: F326C70A:9F62 9DC87D4A:01BB 01 00000000:00000000 00:00000000 00000000 10127        0 384590 1 00000000 25 4 19 10 -1
  3: F326C70A:9F52 9DC87D4A:01BB 01 00000000:00000000 00:00000000 00000000 10127        0 377105 1 00000000 26 4 24 10 -1
```

- **内存信息**

```
memory info:
 System Summary (From: /proc/meminfo)
  MemTotal:        1846008 kB
  MemFree:           39444 kB
  MemAvailable:     468560 kB
  Buffers:           13184 kB
  Cached:           555712 kB
  SwapCached:        46180 kB
  Active:           525916 kB
  Inactive:         534600 kB
  Active(anon):     278644 kB
  Inactive(anon):   283012 kB
  Active(file):     247272 kB
  Inactive(file):   251588 kB
  Unevictable:       62468 kB
  Mlocked:           62468 kB
```

# 3. ANR 的捕获
ANR 的捕获因为 Android 版本的限制，在低于 21 一下是在 AnrHandler 中，通过监听 /data/anr/ 路径下的文件变化来实现的。

```java
// AnrHandler.java
fileObserver = new FileObserver("/data/anr/", CLOSE_WRITE) {
        public void onEvent(int event, String path) {
            try {
                if (path != null) {
                    String filepath = "/data/anr/" + path;
                    if (filepath.contains("trace")) {
                        handleAnr(filepath);
                    }
                }
            } catch (Exception e) {
                XCrash.getLogger().e(Util.TAG, "AnrHandler fileObserver onEvent failed", e);
            }
        }
    };

    try {
        fileObserver.startWatching();
    } catch (Exception e) {
        ...
    }
```
对于大于及 21 版本以上的，则在 xc_trace.c 中

<image src="image/xcrash_4.png">

```c
int xc_trace_init(...)
{
    ...

    if(xc_common_api_level < 21) return 0;

    ...

    //init for JNI callback
    // 设置回调
    xc_trace_init_callback(env);

    //create event FD
    if (0 > (xc_trace_notifier = eventfd(0, EFD_CLOEXEC))) return XCC_ERRNO_SYS;

    //register signal handler
    // 注册监听
    if (0 != (r = xcc_signal_trace_register(xc_trace_handler))) goto err2;

    //create thread for dump trace
    // ②创建 dump 线程
    if (0 != (r = pthread_create(&thd, NULL, xc_trace_dumper, NULL))) goto err1;

    return 0;

    ...
}
```

### xcc_signal_trace_register
xcc_signal_trace_register ① 注册监听 会用 xc_trace_handler 监听

如果有信号过来，xc_trace_handler 里面会写 xc_trace_notifier 的值，通知 dump 线程开始 dump

```c
static void xc_trace_handler(int sig, siginfo_t *si, void *uc)
{
    uint64_t data;
    
    (void)sig;
    (void)si;
    (void)uc;

    if (xc_trace_notifier >= 0) {
        data = 1;
        XCC_UTIL_TEMP_FAILURE_RETRY(write(xc_trace_notifier, &data, sizeof(data)));
    }
}
```

### xc_trace_dumper

```c
static void *xc_trace_dumper(void *arg)
{
    ...

      while (1) {
        //block here, waiting for sigquit
        // 一直等着 xc_trace_notifier 的 信号
        XCC_UTIL_TEMP_FAILURE_RETRY(read(xc_trace_notifier, &data, sizeof(data)));

        ...
        // dump
        xc_trace_libart_runtime_dump(*xc_trace_libart_runtime_instance, xc_trace_libcpp_cerr);
            
       ...
}
```

# 4. 总结

xCrash 捕获的崩溃信息可以在第二次启动的时候上报，比较适合自己搭建的崩溃上报系统。市场上现有的 友盟 、Bugly 做的很好，但是有些产品性质的需要，要搭建自己的崩溃上报，xCrash 是个不错的选择.

另外，xCrash 涉及很多系统调用的函数已经堆栈回溯的技术，这方法知识需要自己后续补上。

# 5. 参考
- [Android 平台 Native 代码的崩溃捕获机制及实现](https://mp.weixin.qq.com/s/g-WzYF3wWAljok1XjPoo7w?)
- [崩溃日志收集库 xCrash 浅析](https://www.dalvik.work/2021/06/22/xcrash/)



