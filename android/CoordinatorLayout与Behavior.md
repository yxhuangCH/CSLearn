# CoordinatorLayout 与 Behavior


## CoordinatorLayout 的使用
先看官网对 CoordinatorLayout 的介绍
CoordinatorLayout is a super_powered FrameLayout。

CoordinatorLayout is intended for two primary use cases:

1. As a top-level application decor or chrome layout;

2. As a container for a specific interaction with one or more child views

我们常用的是第二种情况居多 

CoordinatorLayout 结合 AppBarLayout,CollapsingToolbarLayout 和 Toolbar 一起使用，可以给我们的应用带来更多的交互效果。
它们的布局关系

```xml
<android.support.design.widget.CoordinatorLayout...>
    <android.support.design.widget.AppBarLayout...>
        <android.support.design.widget.CollapsingToolbarLayout...>
            <!-- your collapsed view -->
            <View.../>
            <android.support.v7.widget.Toolbar.../>
        </android.support.design.widget.CollapsingToolbarLayout>
    </android.support.design.widget.AppBarLayout>

    <!-- Scroll view -->
    <android.support.v7.widget.RecyclerView.../>
</android.support.design.widget.CoordinatorLayout>
```

这是效果图
<img src="mddia/coor_1.gif"  width="70%" height="70%">

先看看它们的几个属性常用的几个布局元素

### CoordinatorLayout 
这几个属性都是直接在子布局中使用的
#### app:layout_behavior
指定子 View 的 behavior， 关于 behavior 后面会有详细的论述

#### app:layout_anchor
指定锚点的 View
#### app:layout_anchorGravity
指相对于锚 view 的布局重心
#### app:layout_keyline

### AppBarLayout
AppBarLayout 一般作为 CoordinatorLayout 的直接子类使用；
AppBarLayout 的子 View 通过设置自身的 srollFlags 进行希望的滑动行为;
如果把 AppBarLayout 放到普通的 ViewGroup 中而不是 CoordinatorLayout 中，AppBarLayout 的功能将不会起作用

####  app:layout_scrollFlags/ setScrollFlags(int)
 app:layout_scrollFlags 标记位是**子布局**设置是否可滑动
 
 - scroll: 滑动
 - enterAlways: 获取屏幕外，向下滑，会重新出现
 - exitUntilCollapsed	滑动一定距离出屏幕，会收叠成 minHeight
 - snap: 在活动停止之后，会自动滑动靠边的一侧
 - enterAlwaysCollapsed：要与 minHeight 和 enterAlways 一起使用， 当 View 达到 minHeight 的高度时，CollapsingToolbarLayout 开始展开展开完之后，才会进行滚动

   ```xml
   <android.support.design.widget.CoordinatorLayout
   
    ...
    
    <android.support.design.widget.CollapsingToolbarLayout
            android:id="@+id/collapsing_toolbar_layout"
            android:layout_width="match_parent"
            android:layout_height="220dp"
            android:fitsSystemWindows="true"
            android:minHeight="100dp"
            app:expandedTitleMarginStart="38dp"
            app:layout_scrollFlags="scroll|enterAlwaysCollapsed|enterAlways"/>
           
    .../>
   ```
   
   
#### app:expanded
设置 AppBarLayout 是否展开

### CollapsingToolbarLayout
CollapsingToolbarLayout 是一个实现了折叠功能包裹 Toolbar 的 View, 它做一位 AppBarLayout 子 View 使用
#### app:layout_collapseMode
- pin 固定，钉住
- parallax 会呈现视觉差，需要collapseParallaxMultiplier(0.0~1.0之间) 视觉差系数一起配合使用在代码中设置

```xml
       <ImageView
            android:id="@+id/img_bg"
            android:layout_width="match_parent"
            android:layout_height="match_parent"
            android:fitsSystemWindows="true"
            android:scaleType="centerCrop"
            android:src="@drawable/coor_bg"
            app:layout_collapseMode="parallax"
            app:layout_collapseParallaxMultiplier="0.7"/>
```

#### app:contentScrim 
折叠之后 toolbar 的颜色

## Behavior
CoordinatorLayout 的子 View 进行一些交互，需要设置 Behavior
如果是 CoordinatorLayout 内部滑动的 View 需要设置已经为我们提供好的 behavior

>  app:layout_behavior="@string/appbar_scrolling_view_behavior"

behavior 的方法分为几类,

### 布局相关的方法

```java
 // 给 Behavior 设置 LayoutParams 时会调用
public void onAttachedToLayoutParams(...) {}

// LayoutParams 移除时会调用
public void onDetachedFromLayoutParams() {}

//  CoordinatorLayout 在测量时会回调这个方法
public boolean onMeasureChild(...) {
    return false;
}

//  CoordinatorLayout 在布局时会回调这个方法
public boolean onLayoutChild(...) {
    return false;
}
```

### 事件处理相关的方法

```java
// 是否拦截 CoordinatorLayout 发过了的点击事件
public boolean onInterceptTouchEvent(...) {
    return false;
}

// 接收 CoordinatorLayout 发过了的点击事件
public boolean onTouchEvent(...) {
    return false;
}
```

### 滑动事件相关的方法

```java
// 当 CoordinatorLayout 内有 NestedScrollView 开始滑动的时候回调
public boolean onStartNestedScroll(...) {
    return false;
}

// 当上面的 onStartNestedScroll 返回 true,会回到改方法
public void onNestedScrollAccepted(...) {}

// 当 CoordinatorLayout 内有 NestedScrollView 停止滑动的时候回调
public void onStopNestedScroll(...) {}

    
// 当 CoordinatorLayout 内有 NestedScrollView 滑动过程中的回调
public void onNestedScroll(...) {}

// 在 onNestedScroll 之前回调该方法
public void onNestedPreScroll(...) {}

 // 是否滑动的惯性事件处理
public boolean onNestedFling(...) {
    return false;
}

// 滑动的惯性事件开始的回调
public boolean onNestedPreFling(...) {
    return false;
}
```

### 依赖 View 相关的方法
这也是我们在自定义 Behavior 时一定会重写的方法

```java
// 当前 View 是否依赖指定 View 进行变化
public boolean layoutDependsOn(CoordinatorLayout parent, V child, View dependency) {
    return false;
}

// 依赖的 View(dependency)变化时的回调
public boolean onDependentViewChanged(CoordinatorLayout parent, V child, View dependency) {
    return false;
}

// 依赖的 View 被移除时的回调
public void onDependentViewRemoved(CoordinatorLayout parent, V child, View dependency) {
}
```

下面是 behavior 重要的方法

```java
    public static abstract class Behavior<V extends View>{
        public Behavior() {
        }
        public Behavior(Context context, AttributeSet attrs) {}

       // 给 Behavior 设置 LayoutParams 时会调用
        public void onAttachedToLayoutParams(@NonNull CoordinatorLayout.LayoutParams params) {}

        // LayoutParams 移除时会调用
        public void onDetachedFromLayoutParams() {}

             
        // 是否拦截 CoordinatorLayout 发过了的点击事件
        public boolean onInterceptTouchEvent(CoordinatorLayout parent, V child, MotionEvent ev) {
            return false;
        }

        // 接收 CoordinatorLayout 发过了的点击事件
        public boolean onTouchEvent(CoordinatorLayout parent, V child, MotionEvent ev) {
            return false;
        }

        // 设置 Behavior 所在 View 之外的 View 的蒙层颜色
        @ColorInt
        public int getScrimColor(CoordinatorLayout parent, V child) {
            return Color.BLACK;
        }

        // 设置蒙层的透明度
        @FloatRange(from = 0, to = 1)
        public float getScrimOpacity(CoordinatorLayout parent, V child) {
            return 0.f;
        }

        // 是否对 Behavior 绑定 View 下面的 View 的进行交互，
        // 默认是是根据 getScrimOpacity 的透明度决定的
        public boolean blocksInteractionBelow(CoordinatorLayout parent, V child) {
            return getScrimOpacity(parent, child) > 0.f;
        }

        // 当前 View 是否依赖指定 View 进行变化
        public boolean layoutDependsOn(CoordinatorLayout parent, V child, View dependency) {
            return false;
        }

        // 依赖的 View(dependency)变化时的回调
        public boolean onDependentViewChanged(CoordinatorLayout parent, V child, View dependency) {
            return false;
        }

        // 依赖的 View 被移除时的回调
        public void onDependentViewRemoved(CoordinatorLayout parent, V child, View dependency) {
        }

        //  CoordinatorLayout 在测量时会回调这个方法
        public boolean onMeasureChild(CoordinatorLayout parent, V child,
                int parentWidthMeasureSpec, int widthUsed,
                int parentHeightMeasureSpec, int heightUsed) {
            return false;
        }

        //  CoordinatorLayout 在布局时会回调这个方法
        public boolean onLayoutChild(CoordinatorLayout parent, V child, int layoutDirection) {
            return false;
        }

        // 设置 tag
        public static void setTag(View child, Object tag) {
            final LayoutParams lp = (LayoutParams) child.getLayoutParams();
            lp.mBehaviorTag = tag;
        }

        // 获取 tag
        public static Object getTag(View child) {
            final LayoutParams lp = (LayoutParams) child.getLayoutParams();
            return lp.mBehaviorTag;
        }

        // 当 CoordinatorLayout 内有 NestedScrollView 开始滑动的时候回调
        public boolean onStartNestedScroll(@NonNull CoordinatorLayout coordinatorLayout,
                @NonNull V child, @NonNull View directTargetChild, @NonNull View target,
                @ScrollAxis int axes, @NestedScrollType int type) {
            if (type == ViewCompat.TYPE_TOUCH) {
                return onStartNestedScroll(coordinatorLayout, child, directTargetChild,
                        target, axes);
            }
            return false;
        }

        // 当上面的 onStartNestedScroll 返回 true,会回到改方法
        public void onNestedScrollAccepted(@NonNull CoordinatorLayout coordinatorLayout,
                @NonNull V child, @NonNull View directTargetChild, @NonNull View target,
                @ScrollAxis int axes, @NestedScrollType int type) {
            if (type == ViewCompat.TYPE_TOUCH) {
                onNestedScrollAccepted(coordinatorLayout, child, directTargetChild,
                        target, axes);
            }
        }

        // 当 CoordinatorLayout 内有 NestedScrollView 停止滑动的时候回调
        public void onStopNestedScroll(@NonNull CoordinatorLayout coordinatorLayout,
                @NonNull V child, @NonNull View target, @NestedScrollType int type) {
            if (type == ViewCompat.TYPE_TOUCH) {
                onStopNestedScroll(coordinatorLayout, child, target);
            }
        }

        // 当 CoordinatorLayout 内有 NestedScrollView 滑动过程中的回调
        public void onNestedScroll(@NonNull CoordinatorLayout coordinatorLayout, @NonNull V child,
                @NonNull View target, int dxConsumed, int dyConsumed,
                int dxUnconsumed, int dyUnconsumed, @NestedScrollType int type) {
            if (type == ViewCompat.TYPE_TOUCH) {
                onNestedScroll(coordinatorLayout, child, target, dxConsumed, dyConsumed,
                        dxUnconsumed, dyUnconsumed);
            }
        }

        // 在 onNestedScroll 之前回调该方法
        public void onNestedPreScroll(@NonNull CoordinatorLayout coordinatorLayout,
                @NonNull V child, @NonNull View target, int dx, int dy, @NonNull int[] consumed,
                @NestedScrollType int type) {
            if (type == ViewCompat.TYPE_TOUCH) {
                onNestedPreScroll(coordinatorLayout, child, target, dx, dy, consumed);
            }
        }

         // 是否滑动的惯性事件处理
        public boolean onNestedFling(@NonNull CoordinatorLayout coordinatorLayout,
                @NonNull V child, @NonNull View target, float velocityX, float velocityY,
                boolean consumed) {
            return false;
        }

        // 滑动的惯性事件开始的回调
        public boolean onNestedPreFling(@NonNull CoordinatorLayout coordinatorLayout,
                @NonNull V child, @NonNull View target, float velocityX, float velocityY) {
            return false;
        }

        // 如果给CoordinatorLayout设置了fitSystemWindow=true,可以在这里自己处理WindowInsetsCompat
        @NonNull
        public WindowInsetsCompat onApplyWindowInsets(CoordinatorLayout coordinatorLayout,
                V child, WindowInsetsCompat insets) {
            return insets;
        }

        // 在CoordinatorLayout的requestChildRectangleOnScreen()中被调用
        public boolean onRequestChildRectangleOnScreen(CoordinatorLayout coordinatorLayout,
                V child, Rect rectangle, boolean immediate) {
            return false;
        }  
    }
```

## CoordinatorLayout 与 Behavior 的关系
了解 CoordinatorLayout 与 Behavior 的关系，需要进入 CoordinatorLayout 的源码里面去看看。


#### CoordinatorLayout#onMeasure
**CoordinatorLayout#onMeasure 方法里面会调用 Behavior.#onMeasureChild**

```java
@Override
protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
	// 准备工作, 用 DFS 深度遍历算法，对依赖的 View 进行排序
    prepareChildren();
    // 根据情况添加或者移除OnPreDrawListener
    ensurePreDrawListener();

   	...

    final boolean applyInsets = mLastInsets != null && ViewCompat.getFitsSystemWindows(this);

    final int childCount = mDependencySortedChildren.size();
    for (int i = 0; i < childCount; i++) {
    	// 从排好续的集合中依次获取Child Vie
        final View child = mDependencySortedChildren.get(i);
        if (child.getVisibility() == GONE) {
            // If the child is GONE, skip...
            continue;
        }

        final LayoutParams lp = (LayoutParams) child.getLayoutParams();
        ....
      	
        // 处理 FitsSystemWindows
        int childWidthMeasureSpec = widthMeasureSpec;
        int childHeightMeasureSpec = heightMeasureSpec;
        if (applyInsets && !ViewCompat.getFitsSystemWindows(child)) {
            // We're set to handle insets but this child isn't, so we will measure the
            // child as if there are no insets
            ...
        }

        // Behavior 
        final Behavior b = lp.getBehavior();
        // Behavior.onMeasureChild 方法调用
        if (b == null || !b.onMeasureChild(this, child, childWidthMeasureSpec, keylineWidthUsed,
                childHeightMeasureSpec, 0)) {
            onMeasureChild(child, childWidthMeasureSpec, keylineWidthUsed,
                    childHeightMeasureSpec, 0);
        }

       ...
    }

 	...
    // 设置 width，height
    setMeasuredDimension(width, height);
}
```

#### CoordinatorLayout#onLayout
**CoordinatorLayout#onLayout 方法里面会调用 Behavior#onLayoutChild**

```java
@Override
protected void onLayout(boolean changed, int l, int t, int r, int b) {
    final int layoutDirection = ViewCompat.getLayoutDirection(this);
    final int childCount = mDependencySortedChildren.size();
    for (int i = 0; i < childCount; i++) {
        final View child = mDependencySortedChildren.get(i);
        if (child.getVisibility() == GONE) {
            // If the child is GONE, skip...
            continue;
        }

        final LayoutParams lp = (LayoutParams) child.getLayoutParams();
        final Behavior behavior = lp.getBehavior();

        // 调用 Behavior#onLayoutChild, 如果 behavior 不进行测量，则需要 CoordinatorLayout 自己测量
        if (behavior == null || !behavior.onLayoutChild(this, child, layoutDirection)) {
            onLayoutChild(child, layoutDirection);
        }
    }
}

public void onLayoutChild(View child, int layoutDirection) {
    final LayoutParams lp = (LayoutParams) child.getLayoutParams();
    if (lp.checkAnchorChanged()) {
        throw new IllegalStateException("An anchor may not be changed after CoordinatorLayout"
                + " measurement begins before layout is complete.");
    }
    if (lp.mAnchorView != null) {
    	// 设置了 AnchorView  时候的布局， app:layout_anchor
        layoutChildWithAnchor(child, lp.mAnchorView, layoutDirection);
    } else if (lp.keyline >= 0) {
    	// 设置了 keyline 的布局， app:layout_keyline
        layoutChildWithKeyline(child, lp.keyline, layoutDirection);
    } else {
    	// 正常测量，像 FrameLayout 那样布局
        layoutChild(child, layoutDirection);
    }
}
```


#### CoordinatorLayout#onLayout
CoordinatorLayout#onLayout 方法会调用 Behavior#onInterceptTouchEvent 方法询问是否要拦截，也会调用 调用 Behavior#onTouchEvent 处理点击事件

```java
 @Override
public boolean onInterceptTouchEvent(MotionEvent ev) {
    MotionEvent cancelEvent = null;

    final int action = ev.getActionMasked();

    // Make sure we reset in case we had missed a previous important event.
    // 重置 Behavior 
    if (action == MotionEvent.ACTION_DOWN) {
        resetTouchBehaviors(true);
    }

    // performIntercept 进行判断是否拦截
    final boolean intercepted = performIntercept(ev, TYPE_ON_INTERCEPT);

    if (cancelEvent != null) {
        cancelEvent.recycle();
    }

    if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL) {
        resetTouchBehaviors(true);
    }

    return intercepted;
}

private boolean performIntercept(MotionEvent ev, final int type) {
    boolean intercepted = false;
    boolean newBlock = false;

    MotionEvent cancelEvent = null;

    final int action = ev.getActionMasked();

    final List<View> topmostChildList = mTempList1;
    // View 按照 z-order 进行排序
    getTopSortedChildren(topmostChildList);

    // Let topmost child views inspect first
    final int childCount = topmostChildList.size();
    for (int i = 0; i < childCount; i++) {
        final View child = topmostChildList.get(i);
        final LayoutParams lp = (LayoutParams) child.getLayoutParams();
        final Behavior b = lp.getBehavior();

        if ((intercepted || newBlock) && action != MotionEvent.ACTION_DOWN) {
            // Cancel all behaviors beneath the one that intercepted.
            // If the event is "down" then we don't have anything to cancel yet.
            // 如果一个 View 把事件拦截了，则把重叠于它之下的 behavior 事件都取消
            if (b != null) {
                if (cancelEvent == null) {
                    final long now = SystemClock.uptimeMillis();
                    cancelEvent = MotionEvent.obtain(now, now,
                            MotionEvent.ACTION_CANCEL, 0.0f, 0.0f, 0);
                }
                switch (type) {
                    case TYPE_ON_INTERCEPT:
                        b.onInterceptTouchEvent(this, child, cancelEvent);
                        break;
                    case TYPE_ON_TOUCH:
                        b.onTouchEvent(this, child, cancelEvent);
                        break;
                }
            }
            continue;
        }

        if (!intercepted && b != null) {
            switch (type) {
                case TYPE_ON_INTERCEPT:
                	// 调用 Behavior#onInterceptTouchEvent 方法询问是否要拦截
                    intercepted = b.onInterceptTouchEvent(this, child, ev);
                    break;
                case TYPE_ON_TOUCH:
                	// 调用 Behavior#onTouchEvent 处理点击事件 
                    intercepted = b.onTouchEvent(this, child, ev);
                    break;
            }
            if (intercepted) {
                mBehaviorTouchView = child;
            }
        }

        ...
    }

    topmostChildList.clear();

    return intercepted;
}
```

#### CoordinatorLayout#onTouchEvent
在 CoordinatorLayout#onTouchEvent 方法里面会调用 Behavior#onTouchEvent 是否进行拦截判断  

```java
@Override
    public boolean onTouchEvent(MotionEvent ev) {
        boolean handled = false;
        boolean cancelSuper = false;
        MotionEvent cancelEvent = null;

        final int action = ev.getActionMasked();

        // 如果是 Behavior 拦截了，则把点击事件教给 Behavior#onTouchEvent 处理
        if (mBehaviorTouchView != null || (cancelSuper = performIntercept(ev, TYPE_ON_TOUCH))) {
            // Safe since performIntercept guarantees that
            // mBehaviorTouchView != null if it returns true
            final LayoutParams lp = (LayoutParams) mBehaviorTouchView.getLayoutParams();
            final Behavior b = lp.getBehavior();
            if (b != null) {
                handled = b.onTouchEvent(this, mBehaviorTouchView, ev);
            }
        }

        // Keep the super implementation correct
       // 如果是 Behaivor 不拦截，则交给 CoordinatorLayout 的父类处理，按照事件传递流程处理
        if (mBehaviorTouchView == null) {
            handled |= super.onTouchEvent(ev);
        } else if (cancelSuper) {
            if (cancelEvent == null) {
                final long now = SystemClock.uptimeMillis();
                cancelEvent = MotionEvent.obtain(now, now,
                        MotionEvent.ACTION_CANCEL, 0.0f, 0.0f, 0);
            }
            super.onTouchEvent(cancelEvent);
        }

        if (!handled && action == MotionEvent.ACTION_DOWN) {

        }

        if (cancelEvent != null) {
            cancelEvent.recycle();
        }

        // ACTION_UP 事件重置 Behavior
        if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL) {
            resetTouchBehaviors(false);
        }

        return handled;
    }
```

## CoordinatorLayout 的嵌套滑动
CoordinatorLayout 是实现了 NestedScrollingParent2，NestedScrollingParent2 继承了 NestedScrollingParent


NestedScrollingParent2.java 的接口

```java
// This interface should be implemented by ViewGroup  
// subclasses that wish to support scrolling operations 
// delegated by a nested child view
public interface NestedScrollingParent2 extends NestedScrollingParent {

     boolean onStartNestedScroll(@NonNull View child, @NonNull View target, @ScrollAxis int axes,
            @NestedScrollType int type);
            
     boolean onStartNestedScroll(@NonNull View child, @NonNull View target, @ScrollAxis int axes,
            @NestedScrollType int type);
            
     void onNestedScrollAccepted(@NonNull View child, @NonNull View target, @ScrollAxis int axes,
            @NestedScrollType int type);
            
    void onStopNestedScroll(@NonNull View target, @NestedScrollType int type);
    
    void onNestedScroll(@NonNull View target, int dxConsumed, int dyConsumed,
            int dxUnconsumed, int dyUnconsumed, @NestedScrollType int type);
            
    void onNestedPreScroll(@NonNull View target, int dx, int dy, @Nullable int[] consumed,
            @NestedScrollType int type)
            
}

```
在 CoordinatorLayout 滑动的时候，实现这些滑动方法，都会直接传入到 Behavior 中对应的方法，例如  CoordinatorLayout#onStartNestedScroll 会分发到 Behavior#onStartNestedScroll

```java

@Override
public boolean onStartNestedScroll(View child, View target, int axes, int type) {
    boolean handled = false;

    final int childCount = getChildCount();
    for (int i = 0; i < childCount; i++) {
        final View view = getChildAt(i);
        if (view.getVisibility() == View.GONE) {
            // If it's GONE, don't dispatch
            continue;
        }
        final LayoutParams lp = (LayoutParams) view.getLayoutParams();
        final Behavior viewBehavior = lp.getBehavior();
        if (viewBehavior != null) {
            // 分发到 Behavior#onStartNestedScroll
            final boolean accepted = viewBehavior.onStartNestedScroll(this, view, child,
                    target, axes, type);
            handled |= accepted;
            lp.setNestedScrollAccepted(type, accepted);
        } else {
            lp.setNestedScrollAccepted(type, false);
        }
    }
    return handled;
}
```

关于嵌套滑动，在后续文章中再细说。

当手指滑动的范围在 AppBarLayout 内滑动，CoordinatorLayout 会通过 NestScrollView 的 Behavior 分发事件，让 NestScrollView 产生滑动；
当手指滑动的范围在 NestScrollView 内滑动，CoordinatorLayout 会通过 AppBarLayout 的 Behavior 分发事件，让 AppBarLayout 产生滑；AppBarLayout 默认的 AppBarLayout#Behavior， 不需要显式指定 Behavior。


## AppBarLayout 滑动监听


自定义 AppBarStateChangeListener, 同时定义  AppBarLayout 的状态 EXPANDED 展开，COLLAPSED 折叠 和 IDLE 转态

```java
public abstract class AppBarStateChangeListener implements AppBarLayout.OnOffsetChangedListener {

    private State mCurrentState = State.IDLE;

    @Override
    public final void onOffsetChanged(AppBarLayout appBarLayout, int verticalOffset) {
        if (verticalOffset == 0) {
            if (mCurrentState != State.EXPANDED) {
                onStateChanged(appBarLayout, State.EXPANDED, verticalOffset);
            }
            mCurrentState = State.EXPANDED;
        } else if (Math.abs(verticalOffset) >= appBarLayout.getTotalScrollRange()) {
            if (mCurrentState != State.COLLAPSED) {
                onStateChanged(appBarLayout, State.COLLAPSED, verticalOffset);
            }
            mCurrentState = State.COLLAPSED;
        } else {
            if (mCurrentState != State.IDLE) {
                onStateChanged(appBarLayout, State.IDLE, verticalOffset);
            }
            mCurrentState = State.IDLE;

        }
    }

    public abstract void onStateChanged(AppBarLayout appBarLayout, State state, int verticalOffset);
}

enum State {
    EXPANDED,
    COLLAPSED,
    IDLE
}
```

然后在 AppBarLayout 中设置监听

```java
appBarLayout.addOnOffsetChangedListener(new AppBarLayout.OnOffsetChangedListener() {
        @Override
        public void onOffsetChanged(AppBarLayout appBarLayout, int verticalOffset) {
            // 进行操作
    });
```

这个回调是在 AppBarLayout$Behavior#setHeaderTopBottomOffset 方法中回调的。当 AppBarLayout 在进行滑动的时候，会调用 setHeaderTopBottomOffset 方法，在 setHeaderTopBottomOffset在中 

```java
@Override
    int setHeaderTopBottomOffset(CoordinatorLayout coordinatorLayout,
            AppBarLayout appBarLayout, int newOffset, int minOffset, int maxOffset) {
        final int curOffset = getTopBottomOffsetForScrollingSibling();
        int consumed = 0;

        if (minOffset != 0 && curOffset >= minOffset && curOffset <= maxOffset) {
            // If we have some scrolling range, and we're currently within the min and max
            // offsets, calculate a new offset
            newOffset = MathUtils.clamp(newOffset, minOffset, maxOffset);
            if (curOffset != newOffset) {
                final int interpolatedOffset = appBarLayout.hasChildWithInterpolator()
                        ? interpolateOffset(appBarLayout, newOffset)
                        : newOffset;

                final boolean offsetChanged = setTopAndBottomOffset(interpolatedOffset);

                // Update how much dy we have consumed
                consumed = curOffset - newOffset;
                // Update the stored sibling offset
                mOffsetDelta = newOffset - interpolatedOffset;

                if (!offsetChanged && appBarLayout.hasChildWithInterpolator()) {
                    // If the offset hasn't changed and we're using an interpolated scroll
                    // then we need to keep any dependent views updated. CoL will do this for
                    // us when we move, but we need to do it manually when we don't (as an
                    // interpolated scroll may finish early).
                    coordinatorLayout.dispatchDependentViewsChanged(appBarLayout);
                }

// 分发到监听
appBarLayout.dispatchOffsetUpdates(getTopAndBottomOffset());

                // Update the AppBarLayout's drawable state (for any elevation changes)
                updateAppBarLayoutDrawableState(coordinatorLayout, appBarLayout, newOffset,
                        newOffset < curOffset ? -1 : 1, false);
            }
        } else {
            // Reset the offset delta
            mOffsetDelta = 0;
        }

        return consumed;
    }    
}    

// 分发到所有的监听器
void dispatchOffsetUpdates(int offset) {
if (mListeners != null) {
    for (int i = 0, z = mListeners.size(); i < z; i++) {
        final OnOffsetChangedListener listener = mListeners.get(i);
        if (listener != null) {
            listener.onOffsetChanged(this, offset);
        }
    }
}
```

## 注意事项
1. 如果 CoordinatorLayout 里面使用了 ScrollView 或者 ViewPager, 需要指定 Bebeavior

```xml
<android.support.design.widget.CoordinatorLayout...>
    <android.support.design.widget.AppBarLayout...>
        <android.support.design.widget.CollapsingToolbarLayout...>
            <!-- your collapsed view -->
            <View.../>
            <android.support.v7.widget.Toolbar.../>
        </android.support.design.widget.CollapsingToolbarLayout>
    </android.support.design.widget.AppBarLayout>

    <!-- 需要指定 appbar_scrolling_view_behavior -->
   <android.support.v4.view.ViewPager
        android:id="@+id/viewpager"
        android:layout_width="match_parent"
        android:layout_height="wrap_content"
        app:layout_behavior="@string/appbar_scrolling_view_behavior"/>
</android.support.design.widget.CoordinatorLayout>
```


## 参考
- [CoordinatorLayout的使用](https://blog.csdn.net/victor_fang/article/details/54691957)
- [NestedScrolling笔记](https://coolegos.github.io/2017/11/01/NestedScrolling%E7%AC%94%E8%AE%B0/)
- [CoordinatorLayout的使用（四）——通过AppBarLayout源码分析联动机制](https://www.twblogs.net/a/5c1899e8bd9eee5e40bbfa45/zh-cn)





