# What to look For In a Code Review

https://google.github.io/eng-practices/review/reviewer/looking-for.html

- The code is well-designed.
- The functionality is good for the users of the code.
- Any UI changes are sensible and look good.
- Any parallel programming is done safely.
- The code isn't more complex than it needs to be.
- The developer isn't implementing things they might need in the future but don't know they need now.
- Code has appropriate unit tests.
- Tests are well-designed.
- The developer used clear names for everything.
- Comments are clear and useful, and mostly explain why instead of what.
- Code is appropriately documented.
- The code conforms to our style guides.

## Design 设计
The most important thing to cover in a  review it the overall design of the CL.
- Do the interactions of various pieces of code in the CL make sense?
- Does this change belong in your codebase, or in a library?
- Does it integrate well with the rest of your system?
- Is now a good time to adds this functionality?

## Functionality 功能
- The CL is developer intended,and it good for the users of this code.
- Developer to test CLs well-engough ,and Reviewer should be thinking about the edge, looking for concurrency problem.
- Validate the CL.
- Think about parallel programming

## Compexity 复杂度
**Too complex usually means can't be understood quickly by the code readers**. it can alse mean "**developers are likely to instroduce bugs when they try to call or modify this code**"

Is the CL **over-engineering**,Encourage developers to solve the problem they know needs to solved now, not the problem that the developer speculates might need to be solved int the future.

- Is the CL more complex than it should be?
- Check this at every level of the CL -- are individual lines too complex?
- Are functions too complex?
- Are class too complex?

## Test 测试
Ask for unit, integration, or end-to-end tests as appropriate for the change.
Tests are also code that has be maintained.Don't accept complexity in tests just because they aren't part of the main binary.

## Naming 命名
A good name is long enough to fully communicate what the item is or does, without being so long that is becomes hard to read.

## Comments 注释
- The developer write clear comments in understandable English
- All of the comments are actually necessary
Usually comments are useful when they explain why some code exits, and should not be eplaining what some code is doing.

## Style 代码风格
The author of the CL should not include major style changes combined with other changes.

## Consistency 一致性
If no other rule applies, the author should maintain consistency with the existing code.


## Documentation 文档
Update associated documentation.


## Context 上下文
It's useful to think about the CL in the context of the system as a whole.
**Don't accept CLs that CLs that degrade the code health of the system.**


## Good Things
If you see something nice in the CL, tell the developer.
Code reviews often just focus on mistakes, but they should offer encouragement and appreiciation for good practies.