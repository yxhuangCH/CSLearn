[toc]

# 1.0 Komposable Architecture 的介绍
Komposable Architecture 是一个供了用于搭建适用于各种目的、复杂度的app的一些核心工具的框架。关于它更详细的信息，可以访问这里：
https://github.com/toggl/komposable-architecture

<image src="kompose.excalidraw.png">

上图是 Komposable Architecture 的说明图。

- **State**：即状态，是一个用于描述某个功能的执行逻辑，和渲染界面所需的数据的类。
- **Action**：一个代表在功能中所有可能的动作的类，如用户的行为、提醒，和事件源等。
- **Reducer**：一个用于描述触发「Action」时，如何从当前状态（state）变化到下一个状态的函数，它同时负责返回任何需要被执行的「Effect」，如API请求（通过返回一个「Effect」实例来完成）。
- **SideEffect**: 一个执行数据操作的类，并返回带有 Action 的 Flow。
- **Store**：用于驱动某个功能的运行时（runtime）。将所有用户行为发送到「Store」中，令它运行「Reducer」和「Effects」。同时从「Store」中观测「State」，以更新UI。

下面是它的主要工作流程：

- ①② 是我们在 View 或者 ViewModel 中，会用 Store 发送一个 Action 到 Reducr 中；
- ③④ Reducer 会创建 SideEffect，并在 SideEffect 里面执行 API 请求，并将结果放入到 Action 中返回给 Reducer。
③④不是必须的，如果是 without Effect 就不会有这两步。
- ⑤⑥ SideEffect 将新的 Action 给会 Reducer， Reducer 就会更新 State。
- ⑦ 当 State 发生改变时，在 View/ViewModel 中同步变化，因此刷新 UI。



# 2.0 Komposable Architecture 的使用

为了更好的说明 Komposable Architecture的使用，我用 Komposable Architecture 改造了 Android 官方的 architecture-samples Todo 应用。
如果大家还不熟悉 Todo 的架构和功能，可以先去 Github 下载下来熟悉一下，这里有链接：
https://github.com/android/architecture-samples.

如果是想看本文用 Komposable Architecture 改造后的 Todo 代码，请访问这里： 
https://github.com/yxhuangCH/architecture-samples-custom/tree/feature/komposable

在开始介绍 Komposable Architecture 使用之前，想提前说明它的几个比较难理解的函数。

-  **mapToLocalState: (State) -> ViewState**
   mapToLocalState 函数传进来的是整个 GlobalState, 返回值是子 ViewState,它的作用就是将 AppState 映射到子 ViewState；
-  **mapToLocalAction: (GlobalAction) -> LocalAction**
   mapToLocalAction 函数传进来的是 GlobalAction, 返回值是子 LocalAction，它的作用就是将 GlobalAction 映射会子LocalAction；
-  **mapToGlobalState: (GlobalState, LocalState) -> GlobalState**
   mapToGlobalState 传进来的是 GlobalState 和子的 LocalState, 将 GlobalState 里面对应的的 LocalState 更新，然后再返回 GlobalState；
-  **mapToGlobalAction: (LocalAction) -> GlobalAction**
   mapToGlobalAction 传进来的是子的 LocalAction， 返回值是 GlobalAction，它将传入进来的 LocalAction，映射为 GlobalAction 并返回。

关于这几个函数现在可能理解不了，等把后文看完在回头就能理解了。


## 2.1 Action
Action 它通常是由 View/ViewModel 或者 SideEffect 发给 Reducer 处理的动作意图。
例如在 Statistics 中，我们需要获取所有的 Task, 于是我们定义了一个 `GetTasksAction` .
Statis 它肯定不止只有个获取 task 的意图，还有刷新，加载中等等动作。所有，我们会把关于 Statistics 的所有动作都放到一起，并且在定义为 `slealed class` 它的所有意图都是它的子类。

```kotlin
sealed class StatisticsAction {

    data object RefreshAction : StatisticsAction()

    data object RefreshCompleted : StatisticsAction()

    data object GetTasksAction : StatisticsAction()

    data object Loading : StatisticsAction()

    data object Error : StatisticsAction()

    data class LoadedTasksAction(val uiModel: StatisticsUiModel) : StatisticsAction()
}
```
其中 `LoadedTasksAction` 中的 `StatisticsUiModel` 是数据返回时需要在页面显示的数据结构。

```kotlin
class StatisticsUiModel(
    val isEmpty: Boolean = false,
    val activeTasksPercent: Float = 0f,
    val completedTasksPercent: Float = 0f
)
```

## 2.2 State
为了让我们的 Statistics 的页面展示数据，需要通过 State 来实现.

```kotlin
data class StatisticsUiState(
    val isEmpty: Boolean = false,
    val isLoading: Boolean = false,
    val activeTasksPercent: Float = 0f,
    val completedTasksPercent: Float = 0f
)
```

现在将已经定义好的 `StatisticsUiState` 放入到整个 AppState 中

```kotlin
data class AppState(
    val addEditTaskUiState: AddEditTaskUiState = AddEditTaskUiState(),
    val statisticsUiState: StatisticsUiState = StatisticsUiState(),
    val taskDetailUiState: TaskDetailUiState = TaskDetailUiState(),
    val tasksUiState: TasksUiState = TasksUiState()
)
```

将定义好的 `StatisticsAction` 作为 AppAction 的一个子类。

```
sealed class AppAction {
    data class AddEditTask(val actions: AddEditTaskAction) : AppAction()
    data class Statistics(val actions: StatisticsAction) : AppAction()

    data class TaskDetail(val actions: TaskDetailAction) : AppAction()

    data class TasksAction(val actions: TasksActions) : AppAction()
}
```


## 2.3 SideEffect
在 SideEffect 中，我们会去获取相应的数据，这个数据源可能是本地数据库或者网络端的。

我们需要实现 Effect 接口，在 run 中返回一个 Flow 对象，Flow 这种的数据类型是 `StatisticsAction`

例如 `GetTasksAction` 要从 `TaskRepository` 中获取数据。

<details>
  <summary>StatisticsEffect</summary>
  <pre>
  <code>
class StatisticsEffect(
    private val action: StatisticsAction,
    private val taskRepository: TaskRepository,
    @ApplicationScope
    private val scope: CoroutineScope,
    @IoDispatcher
    private val ioDispatcher: CoroutineDispatcher
) : Effect<StatisticsAction> {
    override fun run(): Flow<StatisticsAction> = channelFlow {
        when (action) {
            is StatisticsAction.GetTasksAction -> {
                Timber.tag(TAG).d("StatisticsAction.GetTasksAction ")
                handleGetTasks(this)
                awaitClose()
            }

            is StatisticsAction.RefreshAction -> {
                Timber.tag(TAG).d("StatisticsAction.RefreshAction ")
                handleRefresh()
            }

            else -> {}
        }
    }

    private fun handleRefresh() {
        scope.launch {
            taskRepository.refresh()
        }
    }

    private fun handleGetTasks(producerScope: ProducerScope<StatisticsAction>) {
        scope.launch {
            taskRepository.getTasksStream()
                .map { Async.Success(it) }
                .catch<Async<List<Task>>> { emit(Async.Error(R.string.loading_tasks_error)) }
                .map { taskAsync ->
                    produceStatistics2Action(taskAsync)
                }.flowOn(ioDispatcher)
                .collect { action ->
                    Timber.tag(TAG).d("handleGetTasks action: $action")
                    producerScope.channel.send(action)
                }
        }
    }

    private fun produceStatistics2Action(taskLoad: Async<List<Task>>) =
        when (taskLoad) {
            Async.Loading -> {
                StatisticsAction.Loading
            }

            is Async.Error -> {
                StatisticsAction.Error
            }

            is Async.Success -> {
                val stats = getActiveAndCompletedStats(taskLoad.data)
                StatisticsAction.LoadedTasksAction(
                    StatisticsUiModel(
                        isEmpty = taskLoad.data.isEmpty(),
                        activeTasksPercent = stats.activeTasksPercent,
                        completedTasksPercent = stats.completedTasksPercent,
                    )
                )
            }
        }

        companion object {
            private const val TAG = "StatisticsEffect"
        }
    } </code>
  </pre>
</details>

在上面的代码中，当出入的 Action 是 `StatisticsAction.GetTasksAction` 时，会调用 `taskRepository.getTasksStream()` 去获取数据，在经过一番处理后，返回 StatisticsAction。

## 2.4 Reducer

### 2.4.1 子 Reducer

从文中开头的图可以看到， Reducer 在整个 Kompose 中处于中心的位置。当 ViewModel 将一个 Action 发送到 Reducer, Reduer 会创建对应的 SideEffect。另一方面， 当 SideEffect 完成工作会发一个 Action 回到 SideEffect 中，然后更新对应的 State, 从而让数据通过 Flow 更新到 ViewModel 中。


<details>
  <summary>StatisticsReducer</summary>
  <pre>
<code>
  class StatisticsReducer(
    private val taskRepository: TaskRepository,
    @ApplicationScope
    private val scope: CoroutineScope,
    @IoDispatcher
    private val ioDispatcher: CoroutineDispatcher
) : Reducer<StatisticsUiState, StatisticsAction> {
    override fun reduce(
        state: StatisticsUiState,
        action: StatisticsAction
    ): ReduceResult<StatisticsUiState, StatisticsAction> = when (action) {
        is StatisticsAction.RefreshAction,
        is StatisticsAction.GetTasksAction -> {
            Timber.tag(TAG).d("StatisticsAction.GetTasksAction ")
            state.withFlowEffect(
                StatisticsEffect(
                    action = action,
                    taskRepository = taskRepository,
                    scope = scope,
                    ioDispatcher = ioDispatcher
                ).run()
            )
        }

        is StatisticsAction.RefreshCompleted -> {
            Timber.tag(TAG).d("StatisticsAction.RefreshCompleted")
            state.copy(
                isLoading = false,
                isEmpty = false
            ).withoutEffect()
        }

        is StatisticsAction.LoadedTasksAction -> {
            Timber.tag(TAG).d("StatisticsAction.LoadedTasksAction: $action")
            StatisticsUiState(
                isEmpty = action.uiModel.isEmpty,
                isLoading = false,
                activeTasksPercent = action.uiModel.activeTasksPercent,
                completedTasksPercent = action.uiModel.completedTasksPercent
            ).withoutEffect()
        }

        is StatisticsAction.Loading -> {
            Timber.tag(TAG).d("StatisticsAction.Loading")
            state.copy(
                isLoading = true,
                isEmpty = true
            ).withoutEffect()
        }

        is StatisticsAction.Error -> {
            Timber.tag(TAG).d("StatisticsAction.Error")
            state.copy(
                isLoading = false,
                isEmpty = true
            ).withoutEffect()
        }
    }

    companion object {
        private const val TAG = "StatisticsReducer"
    }
}
</code></pre></details>

我们以上面的 `StatisticsReducer` 代码为例，当传过来的 Action 是 `RefreshAction` 和 `GetTasksAction` 时，通过 State 的拓展函数 `withFlowEffect` 返回一个ReduceResult。

```kotlin
fun <State, Action> State.withFlowEffect(flow: Flow<Action>): ReduceResult<State, Action> =
    ReduceResult(this, Effect.fromFlow(flow))
```

除了 `withFlowEffect` 还有多个拓展函数，可以根据自己的需求选择对应的拓展函数。

**withEffect 和  withoutEffect**

在 Reducer 中, 使用 withEffect 代表着需要 SideEffect。 如果不需要，则可以使用 `withoutEffect` 拓展函数，例如我们的 Loading Action 就不需要 SideEffect.

```kotlin
is StatisticsAction.Loading -> {
    Timber.tag(TAG).d("StatisticsAction.Loading")
    state.copy(
        isLoading = true,
        isEmpty = true
    ).withoutEffect()
}
```

**State 的返回**
在 Reducer 中，可以返回一个新的 State。 
例如：
```
is StatisticsAction.LoadedTasksAction -> {
        Timber.tag(TAG).d("StatisticsAction.LoadedTasksAction: $action")
        StatisticsUiState(
            isEmpty = action.uiModel.isEmpty,
            isLoading = false,
            activeTasksPercent = action.uiModel.activeTasksPercent,
            completedTasksPercent = action.uiModel.completedTasksPercent
        ).withoutEffect()
    }
```
也可以返回传进来的 State, 用 copy 函数更新对应的字段数据。
例如：

```
is StatisticsAction.Loading -> {
    Timber.tag(TAG).d("StatisticsAction.Loading")
    state.copy(
        isLoading = true,
        isEmpty = true
    ).withoutEffect()
}
```
如果想在 ViewModel 那边使用上一次的数据，仅仅只是更新一部分的数据信息，就用 copy。如果想要一个全新的 State, 就返回一个新的 State。

### 2.4.2 AppReducer
在建立好子 Reducer 之后，需要对它们进行整合，以便后续在创建 AppStore 的时候使用。

<details>

  <summary>AppReducer</summary>


  <pre><code>

    @Singleton
    @Provides
    fun provideAppReducer(
        addEditTaskReducer: AddEditTaskReducer,
        statisticsReducer: StatisticsReducer,
        taskDetailReducer: TaskDetailReducer,
        tasksReducer: TasksReducer
    ): Reducer<AppState, AppAction> {
        return combine(
            addEditTaskReducer.pullback(
                mapToLocalState = { it.addEditTaskUiState},
                mapToLocalAction = { (it as? AppAction.AddEditTask)?.actions},
                mapToGlobalState = { globalState, addEditTaskUiState ->
                    globalState.copy(addEditTaskUiState = addEditTaskUiState)
                },
                mapToGlobalAction = { AppAction.AddEditTask(it) }
            ),
            statisticsReducer.pullback(
                mapToLocalState = { it.statisticsUiState},
                mapToLocalAction = { (it as? AppAction.Statistics)?.actions},
                mapToGlobalState = { globalState, statisticsUiState ->
                    globalState.copy(statisticsUiState = statisticsUiState) // Update app state
                },
                mapToGlobalAction = {
                    println("MutableStateFlowStore KomposableModule mapToGlobalAction: $it")
                    AppAction.Statistics(it)
                } // Update  AppAction
            ),
            taskDetailReducer.pullback(
                mapToLocalState = { it.taskDetailUiState},
                mapToLocalAction = { (it as? AppAction.TaskDetail)?.actions},
                mapToGlobalState = { globalState, taskDetailUiState ->
                    globalState.copy(taskDetailUiState = taskDetailUiState)
                },
                mapToGlobalAction = { AppAction.TaskDetail(it) }
            ),
            tasksReducer.pullback(
                mapToLocalState = { it.tasksUiState},
                mapToLocalAction = { (it as? AppAction.TasksAction)?.actions},
                mapToGlobalState = { globalState, tasksUiState ->
                    globalState.copy(tasksUiState = tasksUiState)
                },
                mapToGlobalAction = { AppAction.TasksAction(it) }
            )
        )
    }</code></pre>
</details>

## 2.5 Store

Store 在 Kompose 框架中处于核心地位，它持有 Reducer, 将 Action 分发到相应的位置，并且执行 SideEffect 的 功能。

### 2.5.1 创建 AppStore
通过拓展函数 `createStore` 来创建 AppStore


```kotlin
@Singleton
@Provides
fun provideAppStore(
    appReducer: Reducer<AppState, AppAction>,
    storeScopeProvider: StoreScopeProvider,
    dispatcherProvider: DispatcherProvider
): Store<AppState, AppAction> = createStore(
    initialState = AppState(),
    reducer = appReducer,
    storeScopeProvider = storeScopeProvider,
    dispatcherProvider = dispatcherProvider,
)
```

其中需要的参数
- initialState 是 AppState；
- reducer 是 appReducer，将所有 Reducer 整合的 CompositeReducer
- storeScopeProvider, 提供 Scrope 运行整个 Store
- dispatcherProvider, 提供 CoroutineDispatchers 在使用 Store 

下面是它们的具体代码

```kotlin
@Singleton
@Provides
fun provideDispatcherProvider(): DispatcherProvider = DispatcherProvider(
    io = Dispatchers.IO,
    computation = Dispatchers.Default,
    main = Dispatchers.Main,
)

@Singleton
@Provides
fun provideCoroutineScope(
    dispatcherProvider: DispatcherProvider
): CoroutineScope = object : CoroutineScope {
    override val coroutineContext: CoroutineContext
        get() = dispatcherProvider.main
}

@Singleton
@Provides
fun provideStoreScopeProvider(
    coroutineScope: CoroutineScope
): StoreScopeProvider = StoreScopeProvider { coroutineScope }

```
 需要说明的时候 DispatcherProvider 的 Main Dispatcher 不一定要使用 `Dispatchers.Main` 如果 Store 里面有耗时的任务，可能会影响整个 UI, 导致卡顿。为此，我们可以使用一个线程的线程池代替, XIA

 ``` kotlin
@Provides
@Singleton
fun provideSingleThreadDispatcher(): CoroutineDispatcher {
    return Executors.newSingleThreadExecutor().asCoroutineDispatcher()
}

@Singleton
@Provides
fun provideDispatcherProvider(
    singleThreadDispatcher: CoroutineDispatcher
): DispatcherProvider = DispatcherProvider(
    io = Dispatchers.IO,
    computation = Dispatchers.Default,
    main = singleThreadDispatcher,
)
```

### 2.5.2 创建子 Store

Store 提供 `view` 和 `optionalView` 创建子 Store, 它们的区别是 optionalView 函数重点  mapToLocalState 参数返回值可以为空

```kotlin
fun <ViewState, ViewAction : Any> view(
    mapToLocalState: (State) -> ViewState,
    mapToGlobalAction: (ViewAction) -> Action?,
): Store<ViewState, ViewAction>

fun <ViewState : Any, ViewAction : Any> optionalView(
    mapToLocalState: (State) -> ViewState?,
    mapToGlobalAction: (ViewAction) -> Action?,
): Store<ViewState, ViewAction>
```

在 Todo app 中使用 `AppStoreManager` 统一管理子 Store

<details>
  <summary>AppStoreManager</summary>
  <pre>
  <code>
class AppStoreManager @Inject constructor() {
        <br>
        @Inject
        lateinit var appStore: Store<AppState, AppAction>

        fun addEditTaskStore(): Store<AddEditTaskUiState, AddEditTaskAction> = appStore.view(
            mapToLocalState = { it.addEditTaskUiState },
            mapToGlobalAction = { AppAction.AddEditTask(it) }
        )

        fun statisticsStore(): Store<StatisticsUiState, StatisticsAction> = appStore.view(
            mapToLocalState = { it.statisticsUiState },
            mapToGlobalAction = { AppAction.Statistics(it) }
        )

        fun taskDetailStore(): Store<TaskDetailUiState, TaskDetailAction> = appStore.view(
            mapToLocalState = { it.taskDetailUiState },
            mapToGlobalAction = { AppAction.TaskDetail(it) }
        )

        fun tasksStore(): Store<TasksUiState, TasksActions> = appStore.view(
            mapToLocalState = { it.tasksUiState },
            mapToGlobalAction = { AppAction.TasksAction(it) }
        )
}</code></pre>
</details>

# 3.0 Komposable architecture test
对应 Komposable 的单元测试，它提供了 test 库，可以方便更好的进行测试。

## 3.1 Reducer Unit Test
Reducer 的 UnitTest 主要针对两种情况进行写单元测试代码.
第一种是 withEffect, 另一种是 withoutEffect。

针对 Reducer 的单元测试, Komposable architecture test 在ReducerTestExtensions.kt 中提供了多个拓展函数方便进行测试， 例如 testReduce，testReduceException， testReduceState 等。

### 3.1.1 withEffect Unit Test


### 3.1.2 withoutEffect Unit Test
例如针对 Loading action, 它是 withoutEffect 的，

```kotlin
is AddEditTaskAction.Loading -> {
    Timber.tag(TAG).d("AddEditTaskAction.Loading")
    state.copy(isLoading = true).withoutEffect()
}
```
针对这个 case, 需要判断返回的 state 是传进去的 state, 它仅仅是 isLoading 设成 true; 返回的 Effect 是 NoEffect。
因此，使用 `testReduce` 函数测试。

```kotlin
@Test
fun `loading`() = runTest {
    reducer.testReduce(
        initialState = initialState,
        action = AddEditTaskAction.Loading
    ) { state, effect ->
        println("AddEditTaskAction Loading state: $state \neffect: $effect")
        state shouldBe initialState.copy(isLoading = true)
        effect shouldBe NoEffect
    }
}
```

## 3.2 SideEffect Unit Test
withEffect 的情况下的 Test 比较复杂，因为它涉及到 Flow 的测试。好在，对 Flow 的测试可以用 turbine, 关于 turbine 请访问 https://github.com/cashapp/turbine

AddEditTaskReducer.kt
``` kotlin
is AddEditTaskAction.FetchTask -> {
Timber.tag(TAG).d("AddEditTaskAction.LoadEditTask")
state.withFlowEffect(
    AddEditTaskSideEffect(action = action, taskRepository = taskRepository).run()
  )
}
```
在 AddEditTaskReducer 中，FetchTask 会使用 withFlowEffect，创建一个新的 AddEditTaskSideEffect。

AddEditTaskSideEffect.kt

```kotlin
override fun run(): Flow<AddEditTaskAction> = flow {
    when (action) {
        is AddEditTaskAction.FetchTask -> {
            Timber.tag(TAG).d("AddEditTaskAction.LoadEditTask")
            emit(AddEditTaskAction.Loading)
            handleLoadTask(action, this)
        }
    }
}

private suspend fun handleLoadTask(
    action: AddEditTaskAction.FetchTask,
    collector: FlowCollector<AddEditTaskAction>
) {
    val task = taskRepository.getTask(action.taskId)

    val loadedAction = if (task != null) {
        AddEditTaskAction.LoadedEditTask(
            AddEditTaskUiMode(
                title = task.title,
                description = task.description,
                isTaskCompleted = task.isCompleted,
                isLoading = false
            )
        )
    } else {
        AddEditTaskAction.LoadedEditTask(
            AddEditTaskUiMode(
                isLoading = false
            )
        )
    }
    collector.emit(loadedAction)  // Emit safely within the suspend context
}
```
在 AddEditTaskSideEffect， 当接收的 Action 是 FetchTask 时，会先返回一个 Loading 的 action, 然后等 taskRepository.getTask 执行完，再返回 LoadedEditTask 的 action.
所以，我们在写这种 case 的时，需要判断先返回的 Loading， 在判断后返回的 LoadedEditTask。

```kotlin
@Test
fun `AddEditTaskAction FetchTask return no empty AddEditTaskUiMode`() = runTest {
    tasksRepository.addTasks(
        Task(
            title = "title1",
            description = "description1",
            id = TASK_ID,
            isCompleted = true
        )
    )
    reducer.testReduce(
        initialState = initialState,
        action = AddEditTaskAction.FetchTask(TASK_ID)
    ) { state, effect ->
        state shouldBe initialState
        effect.run().test {
            val item1 = awaitItem()
            item1 shouldBe AddEditTaskAction.Loading

            val item2 = awaitItem()
            item2 shouldBe AddEditTaskAction.LoadedEditTask(
                AddEditTaskUiMode(
                    title = "title1",
                    description = "description1",
                    isTaskCompleted = true,
                    isLoading = false
                )
            )
            awaitComplete()
        }
    }
}

```

# 4.0 Komposable Architecture优缺点
我们先说一下它的优点
Komposable Architecture 在 Android 中使用，可以更好的实践数据的单行流动的原则，而且可以做的很好权责分离，刚有利于代码的维护与拓展。

缺点就是学习这个新的框架需要一定的学习成本，另一方面 Store 持有整个 AppState, State 里面持有很多数据的时候，会增加整个应用的内存开销。

总的来说，Komposable Architecture 的出现，为沉寂了很久的 Android 开发领域带来新的东西，如果大家感兴趣，建议去学一下。

