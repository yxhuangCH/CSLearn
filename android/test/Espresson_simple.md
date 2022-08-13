Espresso_simple

[toc]

Espresso 这个系列的内容，代码是参考视频 [UI Testing for Beginners (Espresso and androidx.test)](https://www.youtube.com/watch?v=j8jQq-kYgDU&list=PLgCYzUzKIBE_ZuZzgts135GuLQNX5eEPk)

## 知识点
### ActivityScenario.launch

ActivityScenario provides APIs to start and drive an Activity's lifecycle state for testing. It works with arbitrary activities and works consistently across different versions of the Android framework.

说明链接： https://developer.android.com/reference/androidx/test/core/app/ActivityScenario



### withEffectiveVisibility

Returns a matcher that matches Views that have "effective" visibility set to the given value.

Effective visibility takes into account not only the view's visibility value, but also that of its ancestors. In case of ERROR(/View.VISIBLE), this means that the view and all of its ancestors have visibility=VISIBLE. In case of GONE and INVISIBLE, it's the opposite - any GONE or INVISIBLE parent will make all of its children have their effective visibility.

Note: Contrary to what the name may imply, view visibility does not directly translate into whether the view is displayed on screen (use isDisplayed() for that). For example, the view and all of its ancestors can have visibility=VISIBLE, but the view may need to be scrolled to in order to be actually visible to the user. Unless you're specifically targeting the visibility value with your test, use isDisplayed().

链接： https://developer.android.com/reference/androidx/test/espresso/matcher/ViewMatchers

### withText

Returns a matcher that matches TextViews based on text property value.

Note: A View text property is never null. If you call TextView.setText(CharSequence) with a null value it will still be "" (empty string). Do not use a null matcher.

链接：https://developer.android.com/reference/androidx/test/espresso/matcher/ViewMatchers

## 1. Espresso 依赖

```gradle
dependencies {

    implementation 'androidx.core:core-ktx:1.7.0'
    implementation 'androidx.appcompat:appcompat:1.4.2'
    implementation 'com.google.android.material:material:1.4.0'
    implementation 'androidx.constraintlayout:constraintlayout:2.1.4'
    testImplementation 'junit:junit:4.13.2'

    androidTestImplementation 'androidx.test.ext:junit:1.1.3'

    // Espresso
    androidTestImplementation 'androidx.test.espresso:espresso-core:3.4.0'

```

## 2. 创建的页面

创建的页面有两个，一个是 MainActivity 和 SecondActivity

- MainActivity

 这个页面只有一个 TextView 和一个 Button
 
- SecondActivity

 这个页面只有一个 TextView 和一个 Button
 
 具体的代码已经放到 Github 上 https://github.com/yxhuangCH/EspressoDemo/tree/Espresso/simple
 
 
## 3. 创建测试代码
 
 **MainActivityTest**
 
 ```kotlin
@RunWith(AndroidJUnit4ClassRunner::class)
class MainActivityTest{

    @Test
    fun test_activity_inView() {
        val activityScenario = ActivityScenario.launch(MainActivity::class.java)

        onView(withId(R.id.main)).check(matches(isDisplayed()))
    }

    @Test
    fun testVisibility_title_nextButton() {
        val activityScenario = ActivityScenario.launch(MainActivity::class.java)

        // 方法 1
        onView(withId(R.id.activity_main_title))
            .check(matches(isDisplayed()))

        // 方法 2
        onView(withId(R.id.activity_main_title))
            .check(matches(withEffectiveVisibility(Visibility.VISIBLE)))

        onView(withId(R.id.button_next_activity))
            .check(matches(isDisplayed()))
    }

    @Test
    fun testTitleTextDisplayed() {
        val activityScenario = ActivityScenario.launch(MainActivity::class.java)

        onView(withId(R.id.activity_main_title))
            .check(matches(withText(R.string.text_mainactivity)))

    }
}
 ```

MainActivityTest **通过 `ActivityScenario.launch` 启动一个 Activity**

- `test_activity_inView` 

    测试页面已经展示

- `testVisibility_title_nextButton`  

    测试 title 和 button 显示。其中 `withEffectiveVisibility` 函数也是可以判断 View 及其祖先 View 是否都是可见状态。
    方法 1 和 方法 2 是等价。
    
- `testTitleTextDisplayed`

    通过 `withText` 测试 title 的展示的文案和 `R.string.text_mainactivity` 一致


