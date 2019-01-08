## ReentrantLock、synchronized 与 ReadWriteLock

### 一. ReentrantLock
ReentrantLock 并不是内置锁 synchronized 的替代方式，而是当 synchronized 加锁机制不适用时，作为一种可选择的高级功能。
ReentrantLock 是 Lock 的实现类，Lock 提供无条件的、可轮询的、定时的以及可中断的锁获取操作，所有加锁和解锁的方法是显式的，因此 ReentrantLock 也被称为显示锁。

#### 1. Lock 的源码

```java
public interface Lock {
   void lock();  // 加锁
    
   void lockInterruptibly() throws InterruptedException;
   
   boolean tryLock(); // 尝试获取锁，如果加锁成功，返回 true
   
   boolean tryLock(long time, TimeUnit unit) throws InterruptedException; // 在一定时间内获取锁，如果在时间内没有加锁成功，则提前结束
   
   void unlock();  // 解锁
   
   Condition newCondition(); // 返回锁的条件实例
}
```

#### 2. ReentrantLock 的用法
##### 2.1 ReentrantLock 的标准用法

```java
//  Lock 的标准用法
private void luckExp(){
    Lock lock = new ReentrantLock();

    //....

    lock.lock();

    try {
        // 更新对象转态
        // 捕获异常，并在必要时恢复不变性条件
    } finally {
        lock.unlock();  // 一定要在 finally 中解锁
    }
}
```
##### 2.2 ReentrantLock 带时间的加锁

```java
 // 带有时间限制的加锁
// 如果操作不能再指定的时间内给出结果，程序会提前结束
public boolean trySend(String message, long timeout, TimeUnit unit) throws InterruptedException{
    Lock lock = new ReentrantLock();
    long nanosToLock = unit.toNanos(timeout) - estimatedNanosToSend(message);
    if (!lock.tryLock(nanosToLock, TimeUnit.NANOSECONDS)){  // 定时锁
        return false;
    }

    try {
        return sendOnSharedLine(message); // 操作
    }finally {
        lock.unlock();
    }
}
```
##### 2.3 ReentrantLock 的条件变量
条件变量（java.util.concurrent.locks.Condition）将 wait、 notify、notifyAll 等操作转化为相应的队形，将复杂而晦涩的同步操作转为直观可控的对象行为

例子：ArrayBlockingQueue 源码

```java
/** Main lock guarding all access */
final ReentrantLock lock;

/** Condition for waiting takes */
private final Condition notEmpty;

/** Condition for waiting puts */
private final Condition notFull;


public ArrayBlockingQueue(int capacity, boolean fair) {
    if (capacity <= 0)
        throw new IllegalArgumentException();
    this.items = new Object[capacity];
    lock = new ReentrantLock(fair);
    notEmpty = lock.newCondition(); // 条件变量
    notFull =  lock.newCondition(); // 条件变量
}
```
两个条件变量从同一再入锁创建，让后在特定操作中判读和等到条件的满足

```java
public E take() throws InterruptedException {
    final ReentrantLock lock = this.lock;
    lock.lockInterruptibly();
    try {
        while (count == 0){
            notEmpty.await(); // 等待
        }   
        return dequeue();
    } finally {
        lock.unlock();
    }
}
```
当队列为空时，试图 take 的线程会等待，而不是直接返回

```java
private void enqueue(E x) {
    // assert lock.getHoldCount() == 1;
    // assert items[putIndex] == null;
    final Object[] items = this.items;
    items[putIndex] = x;
    if (++putIndex == items.length) putIndex = 0;
    count++;
    notEmpty.signal();  // 通知等待线程，非空条件已经满足
}
```
队列了添加了数据，通知等待线程，条件满足。
通过 signal/ await 的组合，完成条件判断和通知等待线程，顺畅完成了状态的流转。
signal 和 await 必须成对调用，不然线程会一直等待直到被打断（interrup）.

### 二. Synchronized
使用了 synchronized 方法在同一时间只能被一个线程使用，其他线程必须等到这方法释放。
线程在进入了同步代码块之前会自动获得锁，并且在退出同步代码块时自动释放锁。

#### 同步代码块包含两部分
- 一个作为锁的对象引用
- 一个作为由这个锁保护的代码块

#### 两种使用方法
- **以锁的对象使用**

```java
private Object lock = new Object();

// 锁的对象引用
private void lockObject(){
    synchronized (lock){
        // 访问或修改由锁保护的共享状态
    }
}
```

- **锁保护的代码块**

```java
// 锁保护代码块
private synchronized void doSomething(){

}
```

- **Class对象锁**

```java
 public static EventBus getDefault() {
    EventBus instance = defaultInstance;
    if (instance == null) {
        synchronized (EventBus.class) {
            instance = EventBus.defaultInstance;
            if (instance == null) {
                instance = EventBus.defaultInstance = new EventBus();
            }
        }
    }
    return instance;
}
```


### 三. Synchronized 与 ReentrantLock 之间选择
#### synchronized
- 开发人员熟悉，简洁紧凑
- 不用担心解锁的问题
- 在线程转储中能给出在哪些调用帧获得了哪些锁，并能检测和识别发生了死锁的线程

#### ReentrantLock
- 提供了可定时的、可轮询的与可中断的锁
- 如果忘记 unlock， 会发生死锁的问题

#### 选择
在 synchronized 无法满足的情况下才会使用 ReentrantLock

### 3. ReadWriteLock 读写锁
- 当读线程多于写线程时，使用 ReadWrite 线程会更好一些
- ReentrantReadWriteLocks 可以用作提高一些并发容器的性能
- 当锁的持有时间比较长并且大部分操作都不会修改被守护资源是，ReadWriteLock 能提高并发性

- **ReadWriteLock 接口**

```java
public interface ReadWriteLock {
    
    Lock readLock();  // 返回一个读的锁

    Lock writeLock(); // 返回一个写的锁
}
```

- **提高一些并发容器的性能**

```java
/**
 * 当锁的持有较长并且大部分操作都不会修改被守护的资源时， ReadWriteLock 可以提高并发性
 * 避免了 "读-写" 与 "写-写"的冲突，
 * 可以多个同时 "读-读"
 * @param <K>
 * @param <V>
 */
public class ReadWriteMap<K, V> {

    private final Map<K, V> map;
    private final ReadWriteLock lock = new ReentrantReadWriteLock();
    private final Lock readLock = lock.readLock();
    private final Lock writeLock = lock.writeLock();

    public ReadWriteMap(Map<K, V> map) {
        this.map = map;
    }

    public V put(K key, V value){
        writeLock.lock();
        try {
            return map.put(key, value);
        } finally {
            writeLock.unlock();
        }
    }

    // 对 remove(), putAll(), clear() 等方法执行相同的操作

    public V get(K key){
        readLock.lock();
        try {
            return map.get(key);
        } finally {
            readLock.unlock();
        }
    }
}
```


### 四. 参考
- 《Java 并发编程实战》
- 极客时间 《Java 核心技术36讲》第 15 讲

