# FutureTask
- FutureTask 表示一种抽象的可生成结果的计算；
- FutureTask 表示的计算是通过 Callable 来实现的，相当于一种可生成结果的 Runnable.
- FutureTask 有三种状态：等待运行、正在运行、运行完成

#### Future.get 
Future.get 方法返回的任务状态有两种：

- 任务完成，返回结果
- 任务未完成，阻塞直到任务完成，返回结果或抛出异常

#### FutureTask 的源码





