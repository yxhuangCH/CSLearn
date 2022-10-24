
Espresso idling resource

https://developer.android.com/training/testing/espresso/idling-resource

[toc]

这是 Espresso UI 测试系列的第五篇文章。这篇主要是讲述 toast 的测试。

代码来源于 youtube 视频：https://www.youtube.com/watch?v=L037q8MGkGA&list=PLgCYzUzKIBE_ZuZzgts135GuLQNX5eEPk&index=12

# 1.0 知识点

**ToastMatcher**

ToastMatcher 是一个自定义的 Matcher，关于 Matcher, 参考上篇的内容。ToastMatcher 是继承 TypeSafeMatcher，用来匹配 Toast. 它通过  windowToken 和 applicationWindowToken 的配对，来验证是否是 Toast.

代码来源于： Author: http://www.qaautomated.com/2016/01/how-to-test-toast-message-using-espresso.html

```kotlin
class ToastMatcher : TypeSafeMatcher<Root?>() {

    override fun describeTo(description: Description?) {
        description?.appendText("There is toast")
    }

    override fun matchesSafely(item: Root?): Boolean {
        val type: Int?= item?.windowLayoutParams?.get()?.type
        if (type == WindowManager.LayoutParams.TYPE_TOAST){
            val windowToken: IBinder = item.decorView.windowToken
            val appToken: IBinder = item.decorView.applicationWindowToken
            if (windowToken === appToken){
                return true
            }
        }
        return false
    }
}
```


# 2.0 Toast test

## 2.1 需求代码
现在是从一个页面点击 button，弹出一个 toast。


MainActivity.kt

```kotlin

...
   
findViewById<Button>(R.id.btn_show_toast).setOnClickListener {
            showToast(buildToastMessage("test toast"))
}
...

companion object{
    fun buildToastMessage(name: String): String{
        return name
    }
}

```

## 2.2 测试代码

```kotlin
  @Test
    fun test_toast() {
        val activityScenario = ActivityScenario.launch(MainActivity::class.java)
        onView(withId(R.id.btn_show_toast)).perform(click())

        onView(withText(MainActivity.buildToastMessage("test toast"))).inRoot(ToastMatcher())
            .check(matches(isDisplayed()))
    }
```

相关代码已经放置到 Github: https://github.com/yxhuangCH/EspressoDemo/tree/Espresso/toast


