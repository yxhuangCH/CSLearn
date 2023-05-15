

 What to look For In a Code Review

# 1. Design 设计
The most important thing to cover in a  review it the overall design of the CL.
- Do the interactions of various pieces of code in the CL make sense?
- Does this change belong in your codebase, or in a library?
- Does it integrate well with the rest of your system?
- Is now a good time to adds this functionality?

在代码审查（Code Review）中，设计（Design）是一个关键的部分。设计不仅要考虑代码的可读性、可维护性和可扩展性，还需要考虑代码与整个系统的交互是否合理，新增功能是否符合项目的发展方向等方面。

以下是一段简单的 Python 代码，我们可以从设计的角度审查这段代码：

```python
class Animal:
    def __init__(self, name, age):
        self.name = name
        self.age = age

    def speak(self):
        pass

class Dog(Animal):
    def speak(self):
        return "Woof!"

class Cat(Animal):
    def speak(self):
        return "Meow!"
```

针对这段代码，我们可以从以下几个方面进行审查：

**1. 代码之间的交互是否合理？**
这段代码定义了一个基类 Animal 和两个子类 Dog 和 Cat。子类继承了基类的属性和方法，并覆盖了 speak 方法。这种设计将共享的属性和方法抽象到基类中，使得代码结构清晰，易于理解和维护。

**2. 这个改动是否应该在你的代码库中，还是应该在一个库中？**
在这个例子中，我们只有一个简单的类结构，可以作为项目的一部分。但如果我们要处理更多的动物类型，可能需要考虑将这个功能抽象成一个独立的库，方便其他项目引用和复用。

**3. 这个改动是否与你的系统集成得很好？**
在这个简单的例子中，我们可以看到类之间的继承关系清晰，子类覆盖了基类的方法，符合面向对象设计原则。如果这个代码段是系统的一部分，那么我们需要确保它与其他系统组件协同工作，譬如与数据库、用户界面等。

**4. 现在是添加这个功能的好时机吗？**
在项目开发过程中，我们需要根据项目的优先级和需求来决定添加功能的时机。例如，如果当前项目的核心功能还未完善，或者有紧急的 bug 需要修复，那么添加这个功能可能不是一个好时机。在决定添加这个功能时，应该充分考虑项目需求、团队协作和项目进度等因素。


# 2. Functionlity 功能


# 3. Complexity 复杂度


# 4. Tests 测试


# 5. Naming 命名

# 6. Comments 注释


# 7. Style 代码风格

# 8. Consistency 一致性


# 9. Documentation 文档




