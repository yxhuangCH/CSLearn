Android Handler

## 一、基本原理
Handler 机制的五个组成部分: Handler, ThreadLocal, Looper, MessageQueue, Message。

### 1. Handler

```java
public Handler() 

public Handler(@Nullable Callback callback) 

public Handler(@NonNull Looper looper)
 
public Handler(@NonNull Looper looper, @Nullable Callback callback) 

// 创建异步的消息的 Handler
public static Handler createAsync(@NonNull Looper looper) 

// 创建异步的消息的 Handler
public static Handler createAsync(@NonNull Looper looper, @NonNull Callback callback)
```

### 2. ThreadLocal

在 Looper.java 文件中

```java
static final ThreadLocal<Looper> sThreadLocal = new ThreadLocal<Looper>();
 
···
 
private static void prepare(boolean quitAllowed) {
    if (sThreadLocal.get() != null) {
        throw new RuntimeException("Only one Looper may be created per thread");
    }
    sThreadLocal.set(new Looper(quitAllowed));
}
```

### 3. Looper

### 4. MessageQueue

### 5. Message


## 二、Hander 应用 HanderThread

## 三、Hander 应用 IntentService

## 四、Handler.post 和 View.post

