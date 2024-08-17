Flow Turbine  是一个用来 Flow 测试的第三方库
更多的内容请访问 https://github.com/cashapp/turbine

- 基本测试

```kotlin
@Test
fun `test turbine`() = runTest {
    flowOf("one").test {
        awaitItem() shouldBe "one"
        awaitComplete()
    }
}

@Test
fun `test turbine two`() = runTest {
    turbineScope {
        val turbine1 = flowOf(1).testIn(backgroundScope)
        val turbine2 = flowOf(2).testIn(backgroundScope)

        turbine1.awaitItem() shouldBe 1
        turbine2.awaitItem() shouldBe 2

        turbine1.awaitComplete()
        turbine2.awaitComplete()
    }
}

```

- 多个返回值，只消费一个，多余的取消
  
```kotlin
@Test
fun `test turbine 3`() = runTest {
    flowOf("one", "two").test {
        awaitItem() shouldBe "one"
        val events = cancelAndConsumeRemainingEvents()
        events.forEach { it->
            println("events it: $it")
        }
    }
}

```

- 多个返回值，只消费一个，多余的忽略

```kotlin
@Test
fun `test turbine 4`() = runTest {
    flowOf("one", "two").test {
        awaitItem() shouldBe "one"
        cancelAndIgnoreRemainingEvents()
    }
}
```

- 测试异常

```kotlin
@Test
fun `test turbine 5`() = runTest {
    flow<String> { throw RuntimeException("Broken!") }.test {
        awaitError().message shouldBe "Broken!"
    }
}
```


- 异步

```kotlin
@Test
fun `test turbine 6`() = runTest {
    channelFlow {
        withContext(Dispatchers.IO){
            Thread.sleep(1000)
            send("item 1")
        }
    }.test {
        awaitItem() shouldBe "item 1"
        awaitComplete()
    }
}
```

- shared flow / state flow

```kotlin

class MyViewModel : ViewModel() {
    private val _state = MutableStateFlow("Initial State")
    val state: StateFlow<String> = _state.asStateFlow()

    fun updateState(newState: String) {
        _state.value = newState
    }
}


@Test
fun `test turbine 8`() = runTest {
    val mutableStateFlow = MutableStateFlow(0)
    mutableStateFlow.test {
        awaitItem() shouldBe 0
        mutableStateFlow.emit(1)
        awaitItem() shouldBe 1
    }
}

// timeout
@Test
fun `test turbine 9`() = runTest {
    val mutableStateFlow = MutableStateFlow(0)
    mutableStateFlow.test(timeout = 2.seconds) {
        awaitItem() shouldBe 0
        mutableStateFlow.emit(1)
        awaitItem() shouldBe 1
    }
}

@Test
fun `test viewModel flow`() = runTest {
    val viewModel = MyViewModel()

    viewModel.state.test {
        awaitItem() shouldBe "Initial State"

        viewModel.updateState("New State")

        awaitItem() shouldBe "New State"
        cancelAndConsumeRemainingEvents()
    }
}

```





