
Espresso idling resource


[toc]

这是 Espresso UI 测试系列的第六篇文章。这篇主要是讲述 idling resources 的测试。

代码来源于 youtube 视频：https://www.youtube.com/watch?v=_96FT7E6PL4&list=PLgCYzUzKIBE_ZuZzgts135GuLQNX5eEPk&index=15&t=4s

# 1.0 知识点

**Espresso idling resources**
有时候我们进行异步操作任务，然后用返回的结果更新 UI， 这是常见的情况。这个时候，如果在进行 UI test,由于结果还没返回，这个是就会失败。

为了解决这个问题，就需要用到 idling resources 解决。


**idling resources 常用的场景**

- 从网络或本地加载数据
- 和数据库建立链接或回调
- 在使用到 Service 的时候
- 进行一些比较复杂的业务逻辑的时候

**idling resources 常用的实现类**

- **CountingIdlingResource**
Maintains a counter of active tasks. When the counter is zero, the associated resource is considered idle. This functionality closely resembles that of a Semaphore. In most cases, this implementation is sufficient for managing your app's asynchronous work during testing.

- **UriIdlingResource**
Similar to CountingIdlingResource, but the counter needs to be zero for a specific period of time before the resource is considered idle. This additional waiting period takes consecutive network requests into account, where an app in your thread might make a new request immediately after receiving a response to a previous request.

- **IdlingThreadPoolExecutor**
A custom implementation of ThreadPoolExecutor that keeps track of the total number of running tasks within the created thread pools. This class uses a CountingIdlingResource to maintain the counter of active tasks.

- **IdlingScheduledThreadPoolExecutor**
A custom implementation of ScheduledThreadPoolExecutor. It provides the same functionality and capabilities as the IdlingThreadPoolExecutor class, but it can also keep track of tasks that are scheduled for the future or are scheduled to execute periodically.


# 2.0 Idling resources

创建 EspressoIdlingResource

```kotlin
// EspressoIdlingResource.kt
object EspressoIdlingResource {

    private const val RESOURCE = "GLOBAL"

    @JvmField
    val countingIdlingResource = CountingIdlingResource(RESOURCE)

    fun increment() {
        countingIdlingResource.increment()
    }

    fun decrement() {
        if (!countingIdlingResource.isIdleNow){
            countingIdlingResource.decrement()
        }
    }

}
```

模拟获取网络数据，这是个耗时的操作。数据回来之后，更新 Adapter。如果调用了 getData 函数，就立即去验证 UI, 就会出现失败的场景，所以要加 EspressoIdlingResource。

```kotlin
// MovieListFragment.kt
   private fun getData(){
        EspressoIdlingResource.increment()
        uiCommunicationListener.loading(true)
        val job = GlobalScope.launch(Dispatchers.IO) { // 模拟网络耗时
            delay(FAKE_NETWORK_DELAY)
        }
        job.invokeOnCompletion{
            lifecycleScope.launch {
                uiCommunicationListener.loading(false)
                listAdapter.submitList(moviesDataSource.getMovies()) // 回来刷新数据
                EspressoIdlingResource.decrement()
            }
        }
    }

```
在测试代码需要增加

```kotlin
//MovieListFragmentTest.kt

@Before
fun registerIdlingResource() {
    IdlingRegistry.getInstance().register(EspressoIdlingResource.countingIdlingResource)
}

@After
fun unregisterIdlingResource() {
    IdlingRegistry.getInstance().unregister(EspressoIdlingResource.countingIdlingResource)
}

```

相关代码已经放置到 Github: https://github.com/yxhuangCH/EspressoDemo/tree/Espresso/IdlingResource


# 参考
- https://developer.android.com/training/testing/espresso/idling-resource
- https://github.com/android/testing-samples/tree/main/ui/espresso/IdlingResourceSample
- https://github.com/android/architecture-components-samples/tree/main/GithubBrowserSample
