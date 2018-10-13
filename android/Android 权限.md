## Android 权限
### 1. 前述
Android 是一个权限分隔的操作系统，其中每个应用都有其独特的系统标识（Linux 用户 ID 和 组 ID）;

Android 安全架构的中心设计点是：在默认情况下任何应用都没有权限执行对其他应用、操作系统或用户有不利影响的任何操作；

### 2. 用户 ID 和文件操作
在 APK 安装时， Android 为每个软件包提供唯一的 Linux 用户 ID(UID)；
在不同的设备上，相同的软件包可能有不同的 UID,每个软件包在指定设备上的 UID 是唯一的；
只有在 manifest 中设置 sharedUserId 为一样，并且每个应用的签名一样，才会给分配同一个 UID

### 3. 权限
**系统只告诉用户需要的权限组，而不是具体权限**

#### 1. 分类
##### Activity 权限
```java
<activity> 标签， 限制由哪个Activity 启动

Activity.startActivity/startForResult
```
##### Service 权限
```java
<service> 标签，限制由谁启动或绑定 Service

Context.startService/stopService/bindService
```
##### BroadCast 权限
```java
<receiver> 标签，限制由谁发广播和接收广播

Context.registerReceiver/sendBroadCast
接收者和发送者都需要权限 
```
##### Content Provider 权限
```java
<provider> 标签

android:readPermission 限制读 Provider
android:writePermission 限制写 Provider
ContentProvider 增删改查都会检查权限
```
##### URI 权限
在用 URI 图片给其他应用时，可以通过 

```java
Intent.FLAG_GRANT_READ_URI_PERMISSION
 
Intent.FLAG_GRANT_WRITE_URI_PERMISSION 
```
确保接收的 Activity 有权限获取 URI 的数据

例子

```xml
<permission android:description="string resource"
            android:icon="drawable resource"
            android:label="string resource"
            android:name="string"
            android:permissionGroup="string"
            android:protectionLevel=["normal" | "dangerous" |
                                     "signature" | ...] />
```

#### 2. 运行权限
##### 权限等级
```java
Normal Permission
访问沙盒外部数据或资源，对用户隐私危险很少时，只需要在 manifest 文件中声明
 
Dangerous Permission
涉及用户隐私或用户存储的数据有影响时，需要运行时的权限
有 日历、相机、联系人、位置、麦克风、电话的状态、传感器、短信、存储
```
如果同一个 Permission Group 里面的一个权限被允许了，则这个 Group 里面的其他权限也当作是被允许的

##### 运行时请求

```java
 // Here, thisActivity is the current activity 检测权限
if (ContextCompat.checkSelfPermission(thisActivity,
                Manifest.permission.READ_CONTACTS)
        != PackageManager.PERMISSION_GRANTED) {

    // Should we show an explanation? 解析权限
    if (ActivityCompat.shouldShowRequestPermissionRationale(thisActivity,
            Manifest.permission.READ_CONTACTS)) {

        // Show an expanation to the user *asynchronously* -- don't block
        // this thread waiting for the user's response! After the user
        // sees the explanation, try again to request the permission.

    } else {   // 不解析，直接申请权限

        // No explanation needed, we can request the permission.

        ActivityCompat.requestPermissions(thisActivity,
                new String[]{Manifest.permission.READ_CONTACTS},
                MY_PERMISSIONS_REQUEST_READ_CONTACTS);

        // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
        // app-defined int constant. The callback method gets the
        // result of the request.
    }
}



// 权限返回时进行处理，Activity.java
public void onRequestPermissionsResult(int requestCode,
        String permissions[], int[] grantResults) {
    switch (requestCode)
        ....
}
```


