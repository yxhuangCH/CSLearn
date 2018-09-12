## Java 注解
注解是那些插入到源码中使用其他工具可以对其进行处理的便签。这些工具可以在源码层次上进线操作，或者可以出来编译在其中放置了注解的类文件。

This allows annotations to be retained by Java VM at run-time and read via reflection。

Annotation are tags or meta-data, which can be inserted in to source code. So that it could be process at runtime and can take decision according to that annotations。

### 1. 注解的语法
注解是由注解接口定义的

```java
    modifiers @interface AnnotationName{
        type elementName();
        //或者
        type elementName() default value;
        ...
    }
```
实例：

```java
    public @interface BugReport{
        int severity() = 0;
        String assignedTo() default "[none]";
    }
```

### 2. 注解的类型

| 用于编译的注解 |作用 |
| --- | --- | 
| @Deprecated     | 用于不再鼓励使用的项
|@SuppressWarning | 阻止某个给定类型的警告信息
| @Override |检查改方法是否覆盖了某一个超类方法
| @Generated | 作为代码生成工具使用


<br/>

|用于管理资源的注解 | 作用
| --- | --- |
| @PostConstruct | 被标记的方法应该在构造之后立即被调用
| @PreDestroy | 被标记的方法应该在被移除之前立即被调用
| @Resource | 用于资源的注入
 
<br/>


| 元注解| 作用 |
| --- | --- |
| @Target | 指明可以应用这个注解的那些项|
| @Retention | 指明这个注解可以保留多久 |
| @Documented | 指明这个注解应该包含在注解项的文档中 |
| @Inherited | 指明这个注解应用与一个类的时候，能够自动被它的子类继承|

<br/>

##### @Target 注解的元素类型

| 元素类型 | 注解适用场合 |  
| --- | --- | 
|  TYPE | 类（包括 enum）及接口（包括注解类型） |
| ANNOTATION_TYPE | 注解类型声明|
| PACKAGE | 包 |
| METHOD | 方法 |
| CONSTRUTOR | 构造器 |
| FIELD | 成员域（包括 enum 常量）|
| PARAMETER | 方法或构造参数 |
| LOCAL_VARIABLE | 局部变量 |

<br/>

##### @Retention
用于指定一条注解应该保留多长时间

| 保留规则 | 描述 |  
| --- | --- | 
|  SOURCE | 不包括在类文件中的注解 |
| CLASS | 包括在类文件中的注解，但是虚拟机不需要将它们载入 | 
| RUNTIME | 包括在类文件中的注解，并由虚拟机载入。通过反射API 可获得它们 |


例子:

```java
    @Target(ElementType.METHOD)
    @Retention(RetentionPolicy.RUNTIME)
    public @interface ActionListener {
        
    }
```

<br/>

### 3. 注解处理器类库 

##### 1. java.lang.reflectAnnotatedElement

```java
boolean isAnnotationPresent(Class<? extends Annotation> annotationType)
// 如果该项具有给定类型的注解，则返回 true
    
<T extends Annotation> T getAnnotation(Class<T> annotationType)
// 获取给定类型的注解, 如果垓项目不具有这样的注解，则返回 Null
    
Annotation[] getAnnotation()
// 获取该项的所有注解，包括继承而来的注解.如果没有出现任何注解，则返回一个长度为 0 的数组
    
Annotation[] getDeclaredAnnotation()
// 获取该项的所有注解，不包括继承而来的注解.如果没有出现任何注解，则返回一个长度为 0 的数组  
     
```
例子：

```java
try {
    Class<?> testClass = Class.forName(className);
    Object object = testClass.newInstance();
    Method[] methods = testClass.getMethods();
    for (Method method : methods){
        if (method.isAnnotationPresent(Test.class)){
            Test annotation = method.getAnnotation(Test.class);
            Class<? extends Exception> expectedClass = annotation.Expected();
            if (expectedClass != null){
                try {
                    method.invoke(object);
                } catch (InvocationTargetException e) {
                    if (e.getTargetException().getClass() == expectedClass){
                        System.out.println("Test Case Pass With Custom Exception");
                    }else {
                        e.printStackTrace();

                    }
                }
            }
        }
    }
} catch (ClassNotFoundException e) {
    e.printStackTrace();
} catch (IllegalAccessException e) {
    e.printStackTrace();
} catch (InstantiationException e) {
    e.printStackTrace();
}
```
##### 2. AbstractProcessor

```java
@AutoService(Processor.class)
public class MyProcessor extends AbstractProcessor {

    private Filer mFiler;
    private Elements mElementUtils;
    private Messager mMessager;
    
    @Override
    public synchronized void init(ProcessingEnvironment processingEnvironment) {
        super.init(processingEnvironment);

        mFiler = processingEnv.getFiler();
        mElementUtils = processingEnv.getElementUtils();
        mMessager = processingEnv.getMessager();

        mGenerationClass = new GenerationClass();

    }

    @Override
    public SourceVersion getSupportedSourceVersion() {
        return SourceVersion.latestSupported();
    }

    @Override
    public Set<String> getSupportedAnnotationTypes() {
        return Collections.singleton(MyAnnotation.class.getCanonicalName());
    }

    @Override
    public boolean process(Set<? extends TypeElement> set, RoundEnvironment roundEnvironment) {
        ...
        return true;
    }
}
```




  
 




