# The standard of code review
## Standard
- First developers must be able to make progress on their tasks.
-  It is  the duty of the reviewer to make sure that each CL is of such a quality that the overall code health of their codebase is not decreasing as time goes on.
-  A reviewer has ownership and responsibility over the code they are reviewing

Note: there is no such thing as "perfect" code, there is only better code.

## Principles
- **Technical facts and data overrule opinions and personal prefences.**
这个原则的含义是：在评估代码时，应该以技术事实和数据为基础，而不是个人偏好或主观判断。
在代码审查中，代码的质量和可维护性应该是最重要的考虑因素。因此，当评估代码时，应该基于技术事实和数据，而不是仅仅基于个人的主观看法或偏好

- **On matters of style, the style is the absolute authority.**
是一个在代码审查（code review）中常用的原则。这个原则的含义是：在风格问题上，代码应该符合已经确定的规范，而不是个人喜好或主观判断。
在编写代码时，不同的人可能会有不同的编码风格偏好。但是，为了确保代码的可读性和可维护性，应该遵循一个已经确定的规范，如编码规范（Coding Style Guidelines）或风格指南（Style Guides）
- **Aspect of software design are almost never a pure style issuse or just a peronal preference**
这个原则的含义是：软件设计的方方面面往往不是纯粹的风格问题，也不仅仅是个人喜好。
在软件开发中，设计是非常重要的。好的设计可以确保代码的可读性、可维护性和可扩展性，而不良的设计则可能导致代码质量下降、维护成本增加等问题。因此，在进行代码审查时，应该关注代码的设计方面。

- **If no other rule applies, then the reviewer may ask the author to be consistent with what is in the current codebase, as long as that doesn't worsen the overall code health of the system**
这个原则的含义是：如果没有其他规则适用，那么审查人员可以要求作者保持与当前代码库中一致，只要这不会损害整个系统的代码质量。
在软件开发中，代码一致性对于代码的可读性和可维护性是非常重要的。如果代码库中的代码格式、命名约定、注释等方面不一致，那么会给代码的阅读和理解带来很大的困难。因此，在进行代码审查时，代码一致性也是需要考虑的因素之一。


## Mentoring
Code review can have an important function of teaching developers something new about a language, a framework, or general software design principles. It's always fine to leave comments that help a developer learn something new. Sharing knowledge is part of improving the code health of a system over time.Just keep in mind that if your comment is purely educational, but not critical to meeting the standards described in this document, indicate that it's not mandatory for the author to resolve it in this CL.

## Resolving conflicts
In any conflict on a code review, the first step is be for the developer and reviewer to try to come to consensus.

when coming to consensus becomes difficult, it can help to have a face-to-face meeting or a video conference between the reviewer and the author, instead of just trying to resolve the conflict through code review comments.