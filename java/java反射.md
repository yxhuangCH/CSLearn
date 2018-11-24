#Java 反射
Java 反射是在 java.lang.reflect 包类，包含 Constructor、Method、Field 的反射。

| 域 | 涉及的方法 | 说明 |
| :-- | :-- | :-- |
| 创建类 Constructor | Class.getConstructor( ) Constructor.newInstance(args) | newInstance( ) 方法可以为无参调用 |
| 方法 Method | Class.getMethods( ) Class.getDeclaredMethods( ) | getMethods( ) 返回自身和父类的方法; getDeclaredMethods( ) 只返回自身的方法 |
| 变量 Field | Class.getFields( ) Class.getDeclaredFields( ) | getFields() 获取这个类和它父类的 public 成员变量; getDeclaredFields() 获取这个类和它父类的全部成员变量 |

### 1.  通过反射创建类 Constructor
#### 无参构造

```java
Class clazz = Class.forName("类的路径");
Constructor constructor = cl.getConstructor();
Object object = constructor.newInstance();
```

#### 有参构造

```java
Class classType = Class.forName(classPath);
Constructor constructor = classType.getConstructor(intArgsClass);
constructor.setAccessible(true);  // 压制 java 检查，防止当构造函数时 private 时拿不到
Object object = constructor.newInstance(intArgs);
```
#### 说明
- getConstructor（Class[]）Class[] 是构造函数参数类型

> 例如 Employee(String name, long salary), 则  Class[] 就是 new Class[]{String.class, long.class}

- onstructor.newInstance(Object[]) Object[] 是构造函数的具体参数值

> 例如 Employee(String name, long salary) 则 Object[] 就是 new Object[]{"张三", 5000}

- 如果是无参构造函数使用 Class.newInstance( ) 即可，可不用反射

#### 例子
#####    调用了无参构造函数
```java
private Object createEmployeeByClassPath(String classPath){
    try {
        Class cl = Class.forName(classPath);
        Constructor constructor = cl.getConstructor();
        Object object = constructor.newInstance();
        return object;
    } catch (Exception e) {
        e.printStackTrace();
    }
    return null;
}
```
##### 如果是 private 的构造函数

```java
// 获取 private 的构造函数
Class cl = Class.forName(classPath);
Constructor constructor = cl.getDeclaredConstructor();
constructor.setAccessible(true);           
Object object = constructor.newInstance();
```
##### 调用有参构造函数

```java
private void createEmployeeByConstructor(String classPath, Class[] intArgsClass, Object[] intArgs){
    try {
        Class classType = Class.forName(classPath);
        Constructor constructor = classType.getConstructor(intArgsClass);
        constructor.setAccessible(true);  // 压制 java 检查，防止当构造函数时 private 时拿不到
        Object object = constructor.newInstance(intArgs);
    } catch (Exception e) {
        e.printStackTrace();
    }
}
```

### 2.通过反射调用方法 Method
#### 获取方法

```java
Class.getMethods() 返回自身和父类的方法
Class.getDeclaredMethods() 只返回自身的方法
```
####	获取指定的方法

```java
// methodName 是方法名，intArgsClass 是方法签名参数类型
Method method = clazz.getDeclaredMethod(methodName, intArgsClass);  
```
#### 调用方法

```java
 method.invoke(Object, intArgs) // intArgs 是具体的参数类型
```
#### 例子
##### 获取方法
```java
private void getMethods(Object object){
    // 获取所有方法，包括父类的
    Method[] methods = object.getClass().getMethods();
    for (Method method : methods){
        Log.i(TAG, "getMethods method " + method.toString());
    }

    // 至获取自身定义的方法
    Method[] methods1 = object.getClass().getDeclaredMethods();
    for (Method method : methods1){
        Log.i(TAG, "getMethods method " + method.toString());
    }
}
```

##### 调用方法
```java
// 反射调用方法
private void invokeMethod(Object object, String methodName, Class[] intArgsClass, Object[] intArgs){
    try {
        Class clazz =  object.getClass();
        Method method = clazz.getDeclaredMethod(methodName, intArgsClass);
        method.setAccessible(true);
        Object returnValue = method.invoke(object, intArgs);
    } catch (Exception e) {
        e.printStackTrace();
    }
}
```

### 3. 通过反射获取成员变量 Field
#### 获取 Field
```java
Fields[] fields = Class.getFields()  // 获取这个类和它父类的 public 成员变量
Fields[] fields = Class.getDeclaredFields()  //获取这个类和它父类的全部成员变量
```
#### 设置成员变量的值
```java
Class.getDeclaredField(fieldName) // 获取具体的成员变量
Field.setAccessible(true);
Field.set(object, value) // 设值
```

#### 例子
##### 获取成员变量
```java
// 获取成员变量
private void getFields(Object object){
    Field[] fields = object.getClass().getFields();
    for (Field field : fields){
        Class type = field.getType();
        String name = field.getName();
    }
}
```
##### 获取具体的成员变量
```java
private void getField(Object object, String fieldName){
    try {
        Field field = object.getClass().getDeclaredField(fieldName);
        field.setAccessible(true);
        Object fieldVale = field.get(object);
        String name = (String) fieldVale;
    } catch (Exception e) {
        e.printStackTrace();
    }
}
```

##### 设置成员变量的值
```java
private void setFieldValue(Object object, String fieldName, Object value){
    try {
        Field field = object.getClass().getDeclaredField(fieldName);
        field.setAccessible(true);
        field.set(object, value);
    } catch (Exception e){
        e.printStackTrace();
    }
}
```

### 4. 获取注解
Metho, Field, Constructor 都可以直接调用getAnnotationgetAnnotation(Class<> annotationType) 获取
> 使用注解的开源库都会用到 EeventBus, ARouter 等
	
```java
private void getAnnotationMethod(Object object){
    Method[] methods = object.getClass().getDeclaredMethods();
    for (Method method: methods){
        MyAnnotation annoType = method.getAnnotation(MyAnnotation.class);
        if (annoType != null){
            Log.i(TAG, "getAnnotationMethod name = " + method.getName());
        }
    }
}
```


### 5.获取修饰符
Metho, Field, Constructor 都可以直接调用 getModifier() 获取
Modifier 这个类可以分析这个返回值

```java
private void getModifiers(Object object){
    Field[] fields1 = object.getClass().getDeclaredFields();
    for (Field field : fields1){
        Class type = field.getType();
        String name = field.getName();
        printModifier(name, field.getModifiers());
    }
}

private void printModifier(String name, int modifier){
    String modifiers = Modifier.toString(modifier);
    boolean isFinal = Modifier.isFinal(modifier);
}
```

#### 6. 其他
#### setAccessible(true)
Metho, Field, Constructor 都可以设置 setAccessible(true)，  压制 java 检查，防止 private 时拿不到

#### getConstructor 与 getDeclaredConstructor 的区别
- getDeclaredConstructor 可以返回 private 的构造函数，但是需要 Constructor.setAccessible(true)， 配合一起使用

```java
Class cl = Class.forName(classPath);
Constructor constructor = cl.getDeclaredConstructor();
constructor.setAccessible(true);
Object object = constructor.newInstance();
```

- getConstructor 只是返回 public 的构造函数

```java
Class cl = Class.forName(classPath);
Constructor constructor = cl.getConstructor();
Object object = constructor.newInstance();
```


