[TOC]

#Koltin 集合常用的操作符

记录 Kotlin　常用的集合操作符的使用

例子的数据

```kotlin
interface Human {
   fun getTime():Int
}


class Student(val times:Int, val name:String) : Human {

    override fun getTime(): Int {
        return this.times
    }

    override fun toString(): String {
        return "Student(times=$times, name='$name')"
    }
}

class Teacher(val times:Int, val age:Int): Human {

    override fun getTime():Int {
        return times
    }

    override fun toString(): String {
        return "Teacher(times=$times, age=$age)"
    }
}
```


### 1.sumBy 某个属性求和

sumby 是将集合里面某个属性求和 

源码

```kotlin
/**
 * Returns the sum of all values produced by [selector] function applied to each element in the collection.
 */
public inline fun <T> Iterable<T>.sumBy(selector: (T) -> Int): Int {
    var sum: Int = 0
    for (element in this) {
        sum += selector(element)
    }
    return sum
}
```

例子:

```kotlin
    println("-------- sumBy")
    val sumBy = allHumans.sumBy {
        if (it is Teacher){
            it.age
        } else {
            0
        }
    }
    println("-------- sumBy $sumBy")
```

输出是

> -------- sumBy 54

### 2.distinctBy 相同条件的元素去重

distinctBy 是将集合里面某个相同条件的元素去重，它里面是一个 set

源码

```kotlin
/**
 * Returns a list containing only elements from the given collection
 * having distinct keys returned by the given [selector] function.
 * 
 * The elements in the resulting list are in the same order as they were in the source collection.
 */
public inline fun <T, K> Iterable<T>.distinctBy(selector: (T) -> K): List<T> {
    val set = HashSet<K>()
    val list = ArrayList<T>()
    for (e in this) {
        val key = selector(e)
        if (set.add(key))
            list.add(e)
    }
    return list
}
```
例子：
我们将根据学生的 name 去重

```kotlin
val students = ArrayList<Student>()
students.add(Student(12, "add"))
students.add(Student(14, "bbb"))
students.add(Student(146, "add"))

teachers.add(Teacher(10, 26))
teachers.add(Teacher(18, 28))

val distinctByList = allHumans.distinctBy {
    if (it is Student) {
        it.name
    } else {
        it
    }
}
distinctByList.forEach {
    println(it)
}
```
输出
> -------- distinctBy 
Student(times=12, name='add')
Student(times=14, name='bbb')
Teacher(times=2, age=2)
Teacher(times=10, age=26)
Teacher(times=18, age=28)

可以看到 名称为 ‘add’ 的 Student 只剩下一个了

### 3.sortWith compareBy
sortWith 是安装集合里面元素的某个属性按升排序
如果是降序是 sortByDescending

源码

```kotlin
/**
 * Sorts elements in the list in-place according to the order specified with [comparator].
 */
@kotlin.jvm.JvmVersion
public fun <T> MutableList<T>.sortWith(comparator: Comparator<in T>): Unit {
    if (size > 1) java.util.Collections.sort(this, comparator)
}
```

例子

```kotlin
allHumans.sortWith(compareBy { it.getTime() })
allHumans.forEach {
    println(it)
}
```

输出
> -------- sortWith compareBy 
Teacher(times=2, age=2)
Teacher(times=10, age=26)
Student(times=12, name='add')
Student(times=14, name='bbb')
Teacher(times=18, age=28)
Student(times=146, name='add')

### 4.filterIsInstance 过滤集合中是某一类元素
filterIsInstance 是过滤集合中是某一类元素

源码

```kotlin
/**
 * Returns a list containing all elements that are instances of specified type parameter R.
 */
public inline fun <reified R> Iterable<*>.filterIsInstance(): List<@kotlin.internal.NoInfer R> {
    return filterIsInstanceTo(ArrayList<R>())
}

/**
 * Appends all elements that are instances of specified class to the given [destination].
 */
@kotlin.jvm.JvmVersion
public fun <C : MutableCollection<in R>, R> Iterable<*>.filterIsInstanceTo(destination: C, klass: Class<R>): C {
    @Suppress("UNCHECKED_CAST")
    for (element in this) if (klass.isInstance(element)) destination.add(element as R)
    return destination
}
```

例子： 过滤学生的实例

```kotlin
   println("-------- filter ")
    allHumans.filterIsInstance<Student>().forEach {
        println(it)
    }
```
输出：

>-------- filter 
Student(times=146, name='add')
Student(times=14, name='bbb')
Student(times=12, name='add')

### 5.filterNot 过滤不是条件的元素

filterNot 过滤掉集合总不是给定添加的元素

源码

```kotlin
/**
 * Returns a list containing all elements not matching the given [predicate].
 */
public inline fun <T> Iterable<T>.filterNot(predicate: (T) -> Boolean): List<T> {
    return filterNotTo(ArrayList<T>(), predicate)
}
/**
 * Appends all elements not matching the given [predicate] to the given [destination].
 */
public inline fun <T, C : MutableCollection<in T>> Iterable<T>.filterNotTo(destination: C, predicate: (T) -> Boolean): C {
    for (element in this) if (!predicate(element)) destination.add(element)
    return destination
}
```

例子

```kotlin
println("-------- filter 2")
allHumans.filterNot {
     it is Student
}.forEach {
    println(it)
}
```

输出
> -------- filter 2
Teacher(times=18, age=28)
Teacher(times=10, age=26)
Teacher(times=2, age=2)

# 完整代码

```koltin

package com.yxhuang.kotlin.sorted


fun main(args: Array<String>) {

    val students = ArrayList<Student>()
    students.add(Student(12, "add"))
    students.add(Student(14, "bbb"))
    students.add(Student(146, "add"))

    val teachers = ArrayList<Teacher>()
    teachers.add(Teacher(10, 26))
    teachers.add(Teacher(18, 28))


    val allHumans = ArrayList<Human>()
    allHumans.addAll(students)
    allHumans.addAll(teachers)


    println("-------- forEach")
    allHumans.forEach {
        println(it)
    }
    
    println("-------- sumBy")
    val sumBy = allHumans.sumBy {
        if (it is Teacher){
            it.age
        } else {
            0
        }
    }
    println("-------- sumBy $sumBy")
    
    println("-------- distinctBy ")
    val distinctByList = allHumans.distinctBy {
        if (it is Student) {
            it.name
        } else {
            it
        }
    }
    distinctByList.forEach {
        println(it)
    }

    println("-------- compareBy ")
    allHumans.sortWith(compareBy { it.getTime() })
    allHumans.forEach {
        println(it)
    }

    println("-------- filter ")
    allHumans.filterIsInstance<Student>().forEach {
        println(it)
    }
    
    println("-------- filter 2")
    allHumans.filterNot {
         it is Student
    }.forEach {
        println(it)
    }
}
```

