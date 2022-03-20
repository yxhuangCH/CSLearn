
[toc]

## 1. 如何包装一个 Call

- **1.包装 request**
将 request 所需的参数用一个 Request 类包住

- **2.如何处理回调**
在 rsp 数据回来之后，调用回调


- **3.分发 Call**
 Call 是一个 Runnable,通过线程池之执行 run 方法，并在这个方法里面进行网络请求，请求回来后，通过 callback 将数据返回去

 - **4.如何同步处理数据**
 底层的网络请求通过 okio 实现，是同步方式的，故可以同步请求网络。但是这样会阻塞发起请求的线程，在 Android 中使用，不能在主线程使用，不然会阻塞主线程

 - **5.连接池**


## 2. 请求的过程

```plantuml
@startuml

App -> OKHttpClient:new
App -> Request: new
OKHttpClient -> RealCall:newCall
RealCall --> RealCall: new 
RealCall --> RealCall: execute()
RealCall --> EventLister: callStart
RealCall -> Dispatcher:execute 同步执行
Dispatcher --> Dispatcher:addCall 加入到队列
RealCall -->RealCall :getResponseWithInterceptorChain 拦截器执行
Dispatcher --> Dispatcher:finished 执行 Call
Dispatcher --> Dispatcher:promoteAndExecute
RealCall --> App:Response 同步返回结果


group 异步请求
RealCall ->Dispatcher:enqueue(Callback)
RealCall -> EventLister: callStart
RealCall --> RealCall: new AsyncCall
RealCall -> Dispatcher:enqueue(AsyncCall)
Dispatcher --> Dispatcher: add，添加到 readyAsyncCalls 队列
Dispatcher --> Dispatcher: promoteAndExecute 执行 
Dispatcher -> AsyncCall : executeOn(ExecutorService)
AsyncCall -> ExecutorService:execute()线程池执行
ExecutorService -> AsyncCall :run 
AsyncCall -> RealCall:getResponseWithInterceptorChain 返回 Response
RealCall -> Callback:onResponse 回调 rsp

AsyncCall ->Dispatcher:finished(AsyncCall)

end


@enduml

```

## 3. 主要类的关系

```plantuml

@startuml


interface Call{
    fun request(): Request
    fun execute(): Response
    fun enqueue(responseCallback: Callback)
    fun cancel()
    fun isExecuted(): Boolean
    fun isCanceled(): Boolean
    fun timeout(): Timeout
}

class Request{
    ...
}

class Response{

}

Call *-> Request
Call *-> Response

class RealCall {
    -  client: OkHttpClient
    - originalRequest: Request
    - forWebSocket: Boolean
    - eventListener: EventListener
    - connectionPool:RealConnectionPool
    - exchangeFinder:ExchangeFinder
    - connection:RealConnection

}

class AsyncCall{
    - responseCallback: Callback

    fun executeOn(executorService: ExecutorService)
    
    fun run()
}


RealCall --|> Call

AsyncCall --|> RealCall
AsyncCall --|> Runnable


@enduml

```


## 4.拦截器


```kotlin
// RealCall.kt
internal fun getResponseWithInterceptorChain(): Response {
    // Build a full stack of interceptors.
    val interceptors = mutableListOf<Interceptor>()
    interceptors += client.interceptors
    interceptors += RetryAndFollowUpInterceptor(client)
    interceptors += BridgeInterceptor(client.cookieJar)
    interceptors += CacheInterceptor(client.cache)
    interceptors += ConnectInterceptor
    if (!forWebSocket) {
      interceptors += client.networkInterceptors
    }
    interceptors += CallServerInterceptor(forWebSocket)

    val chain = RealInterceptorChain(
        call = this,
        interceptors = interceptors,
        index = 0,
        exchange = null,
        request = originalRequest,
        connectTimeoutMillis = client.connectTimeoutMillis,
        readTimeoutMillis = client.readTimeoutMillis,
        writeTimeoutMillis = client.writeTimeoutMillis
    )

    var calledNoMoreExchanges = false
    try {
      val response = chain.proceed(originalRequest)
       
       ...

      return response
    } catch (e: IOException) {
    
     ...
  }


```


## 5.连接池


