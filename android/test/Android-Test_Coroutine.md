Android Test Coroutine



## MainCoroutine

为什么需要 MainCoroutine

```
Exception in thread "Test worker @coroutine#1" java.lang.IllegalStateException: Module with the Main dispatcher had failed to initialize. For tests Dispatchers.setMain from kotlinx-coroutines-test module can be used
	at kotlinx.coroutines.internal.MissingMainCoroutineDispatcher.missing(MainDispatchers.kt:110)
	at kotlinx.coroutines.internal.MissingMainCoroutineDispatcher.isDispatchNeeded(MainDispatchers.kt:91)
	at kotlinx.coroutines.internal.DispatchedContinuationKt.resumeCancellableWith(DispatchedContinuation.kt:329)
	at kotlinx.coroutines.intrinsics.CancellableKt.startCoroutineCancellable(Cancellable.kt:30)
	at kotlinx.coroutines.intrinsics.CancellableKt.startCoroutineCancellable$default(Cancellable.kt:25)
	at kotlinx.coroutines.CoroutineStart.invoke(CoroutineStart.kt:110)
	at kotlinx.coroutines.AbstractCoroutine.start(AbstractCoroutine.kt:126)
	at kotlinx.coroutines.BuildersKt__Builders_commonKt.launch(Builders.common.kt:56)
	at kotlinx.coroutines.BuildersKt.launch(Unknown Source)
	at kotlinx.coroutines.BuildersKt__Builders_commonKt.launch$default(Builders.common.kt:47)
	at kotlinx.coroutines.BuildersKt.launch$default(Unknown Source)
	at com.yxhuang.androiddailydemo.viewmodel.MainViewModel.saveSessionData(MainViewModel.kt:40)

```

