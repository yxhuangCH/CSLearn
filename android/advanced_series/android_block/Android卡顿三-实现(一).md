# Android 进阶系列：
## Android 卡顿（三）：实现（一）
在总述中提到，监控卡顿的方式有两种，一是监控主线程，另外一个计算函数的耗时；在本篇中，先介绍利用监控主线程的方式来检测卡顿。

监控主线程的主要思想是，通过监控主线程执行耗时，当超过阈值时， dump 出当前主线程的执行堆栈，通过分析堆栈找到卡顿原因。

### 1.依赖主线程 Looper, 监控每次 dispatchMessage 的耗时
#### 1.实现原理
Looper#loop 的代码

```java
public static void loop() {
        ...

        for (;;) {
            Message msg = queue.next(); // might block
            if (msg == null) {
                // No message indicates that the message queue is quitting.
                return;
            }

            // This must be in a local variable, in case a UI event sets the logger
            final Printer logging = me.mLogging;
            // 开始 Printer#println 打印
            if (logging != null) {
                logging.println(">>>>> Dispatching to " + msg.target + " " +
                        msg.callback + ": " + msg.what);
            }

            ...

            try {
                msg.target.dispatchMessage(msg);
                dispatchEnd = needEndTime ? SystemClock.uptimeMillis() : 0;
            } finally {
                if (traceTag != 0) {
                    Trace.traceEnd(traceTag);
                }
            }
            ...

            // 结束 Printer#println 打印
            if (logging != null) {
                logging.println("<<<<< Finished to " + msg.target + " " + msg.callback);
            }

            ...

            msg.recycleUnchecked();
        }
    }
}
```

主线程执行的所有任务在 dispatchMessage 中完成，在这个方法前后都会执行 Looper#println 方法。可以通过 Looper#setMessageLogging(@Nullable Printer printer) 设置 Printer, 在通过 Tread#getStackTrace 接口获取主线程的堆栈信息。如果 dispatchMessage 方法超出了阈值、卡顿了，就进行上报。

#### 2.具体实现
- 1. 基础 Printer， 重新 Printer#println 方法

```java
   @Override
    public void println(String x) {
        if (!mPrinting){
            mStartTime = System.currentTimeMillis();
            mStartThreadTime = SystemClock.currentThreadTimeMillis();
            mPrinting = true;
            // 开始采集线程的信息
            mStackSampler.startDump();
        } else {
            final long endTime = System.currentTimeMillis();
            long endThreadTime = SystemClock.currentThreadTimeMillis();
            mPrinting = false;
            if (isBlock(endTime)){
                LogHelper.d(TAG, ">>>>> block <<<<<");
                List<String> entries = mStackSampler.getThreadStackEntries(mStartTime, endTime);
                if (entries.size() > 0){
                    final BlockInfo blockInfo = BlockInfo.newInstance()
                            .setMainThreadTimeCost(mStartTime, endTime, mStartThreadTime, endThreadTime)
                            .setThreadStackEntries(entries)
                            .flush();
                    // 通知广播
                    LogHelper.i(TAG, "block info\n" + blockInfo.toString());
                }
            }
            // 停止采集
            mStackSampler.stopDump();
        }
    }
```

- 2.设置 Looper 的 Printer

```java
 /**
 * 启动检测
 * @param context
 */
public void start(Context context){
    LogHelper.d(TAG, "start block detect");
    if (mIsRunning){
        LogHelper.i(TAG, "start manger is running");
        return;
    }

    if (mMonitorCore == null){
        mMonitorCore = new MonitorCore();
    }
    mIsRunning = true;
    // 设置 Printer
    Looper.getMainLooper().setMessageLogging(mMonitorCore);
}

/**
 * 停止检测
 */
public void stop(){
    if (!mIsRunning){
        LogHelper.i(TAG, "stop manager is not running");
        return;
    }
    // 置空
    Looper.getMainLooper().setMessageLogging(null);
    if (mMonitorCore != null){
        mMonitorCore.shutDown();
        mMonitorCore = null;
    }
    mIsRunning = false;
    LogHelper.d(TAG, "stop block detect");
}
```

- 3. 通过 Thread#getStackTrace 获取主线程的堆栈信息

```java
 // 采集主线程信息
private void dumpInfo(){
    StringBuilder stringBuilder = new StringBuilder();
    Thread thread = Looper.getMainLooper().getThread();
    for (StackTraceElement element : thread.getStackTrace()){
        stringBuilder.append(element.toString()).append(SEPARATOR);
    }
    synchronized (mStackMap){
        if (mStackMap.size() == DEFAULT_MAX_ENTRY_COUNT){
            mStackMap.remove(mStackMap.keySet().iterator().next());
        }
        if (!shouldIngore(stringBuilder)){
            mStackMap.put(System.currentTimeMillis(), stringBuilder.toString());
        }
    }
}

```
完整的代码参考 [滴滴的 DoraemonKit](https://github.com/didi/DoraemonKit/blob/master/Android/doraemonkit/src/main/java/com/didichuxing/doraemonkit/kit/blockmonitor/core/BlockMonitorManager.java)


### 2.利用 Choreographer 模块
#### 1.实现原理
向 Choregrapher 注册一个 FrameCallback 监听对象，同时通过另一条线程循环记录主线程堆栈信息，并在每次 Vsync 事件 doFrame 中循环注册监听。当两次 Vsync 事件的时间间隔超时阈值是，读取堆栈进行上报。

#### 2. 具体实现

```java
  public static void start(){
        Choreographer.getInstance().postFrameCallback(new Choreographer.FrameCallback() {

            @Override
            public void doFrame(long frameTimeNanos) {

                if (lastFrameTimeNanos - frameTimeNanos > mInterval){
                    // 超出时间间隔
                    // 在新开线程去捕获主线程的堆栈信息
                }

                lastFrameTimeNanos = frameTimeNanos;
                Choreographer.getInstance().postFrameCallback(this);
            }
        });
    }
```

### 参考资料
- 1.[微信读书（Android）阅读引擎卡顿监控测试](https://www.infoq.cn/article/weixin-reading-stuck-monitor-and-test)

- 2.[Matrix TraceCanary -- 初恋·卡顿](https://mp.weixin.qq.com/s?__biz=MzAwNDY1ODY2OQ==&mid=2649287054&idx=1&sn=40f1b9935c280547926fc5f799c0b9c2&chksm=8334cd0cb443441aad977bd462df6cafcb20ae55bf9d70c99a7b3045178c848a7e75b6e02aa1&mpshare=1&scene=1&srcid=%23rd)

- 3.[滴滴 DoraemonKit](https://github.com/didi/DoraemonKit)


