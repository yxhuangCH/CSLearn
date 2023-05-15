# Speed of Code Reviews
https://google.github.io/eng-practices/review/reviewer/speed.html

## Code Reviews should be fast
We optimize for the speed at which a team of developers can produce a product together, as apposed to optimizing for the speed at which an individual developer can write code.

When code review are slow, those things happen:
- The velocity of the team as whole is decreased.
- Developers start to protest the code review process.
- Code health can be impacted.

作为一个团体，review 的速度应该快一些，不然会影响到整个团体进度。
如果 review 速度慢了，会有一些负面的影响，例如：
- 整个团队的进度会收到拖累，developer 只能在那里等着 review;
- Developer 会怀疑 code  review 的这个流程，如果经常等太久没有 review, developer 就会有怨言；
- 如果 review 太慢，后面 developer 为了赶时间，容易造成代码质量的下降

总得来说，code review 的速度越快越好。
## How Fast should Code Reviews be

If reviewer is not in the middle of a focused task, should do a code review shortly after it comes in.

A typical CL should be get multiple rounds of review within a single day.

## Speed vs. Interruption
If you are in the middle of a focused task, such as writing code, don't interrupt yourself to do a code review.
如果你正在集中精力在敲代码，这个时候，就不要打断自己去做 code review ，可以在完成了自己的这个 task 之后再去做.

## Fast Responses
快速的反应是很重要的，如果 review 全部的代码需要的时间比较久，可以快速告诉 developer,你就要开始 review 了。
When it comes to code reviews, the speed of individual response is more import than the speed of the entire process.
It'w import that reviewers spend enough time on the review to ensure the code meets standards, but fast individual responses are still ideal.

## Cross-Time-Zone Reviews
When dealing with time zone differences, try to get back to the author while they have time to respond before the end of their working hours.
If they have already finished work for the day, then try to make sure your review is done before they start work the next day.

## LGTM With Comments
LGTM comments
- The reviewer is confident that the developer will appropriately address all the reviewers's remaining comments.
- The remaining changes are minor and don't have to be done by the developer.
- When the developer and reivewer are in different time zones.

## Large CLs
If CL is huge and reviewer don't have time to review it, should be to ask the developer to split the CL into serveral smaller CLs.
If a CL can't be broken up into smaller CLs, and reviewer don't have time to review the entire thing quickly, then at least write some comments on the overall design of the CL and send it back to the developer for improvement.
如果一个 CL 太大了，与此同时 reviewer 也没有足够的时间去 review 的时候，可以建议 developer 将一个大的 CL 分成几个小的 CL. 
如果不能拆分的时候，reviewer 可以先过整体的设计，先提供意见给 developer.

# How to write Code Review Comments
https://google.github.io/eng-practices/review/reviewer/comments.html
## Summary
- Be kind
- Explain your reasoning.
- Balance giving explicit directions with just pointing out problems and letting the developer decide.
- Encourage developers to simplify code or add code comments instead of just explainning the complexity to you.

## Courtesy 礼貌
Reviewer makes comments about the code and never makes comments about the developer.

Bad:"Why did **you** use threads here when there's obviously no benefit to be gained from concurrency?"
Good:"The concurrency model here is adding complexity to the system without any actual performance benefit that I can see. Because there's no performance benefit, it's best for this code to be single-threaded instead of using multiple threads"


## Explain Why
Good comment help the developer understand why you are making your comment.Sometimes it's appropritate to give a bit more explanation around your intent, the best practice you're following, or how your suggestion improves code health.

## Giving Guidance
Reviewer is not required to do detailed design of a solution or write code for the developer.
In general, reviewer should strike an appropriate balance between pointing out problems and providing direct guidance.Pointing out problems and letting the developer make a decision often helps the developer learn, and makes it easier to do code reviews.
作为一个 Reviewer 不需要给 developer 做一个详细的设计的解决方案，但是需要平衡指出问题和提供之间指引。可以指出问题，让 develpor 自己去做决定，这样对于 developer 的成长是有帮助的。

## Label comment severity
Consider labeling the severity of reviewer's comments, differentiating required changes from guidelines or suggestions.

> Nit: This is a minor thing. Technically you should do it, but it won’t hugely impact things.
> Optional (or Consider): I think this may be a good idea, but it’s not strictly required.
> FYI: I don’t expect you to do this in this CL, but you may find this interesting to think about for the future.

This makes review intent explicit and helps authors prioritize the importance of various comments.It also helps avoid misunderstandings;without comment labels, authors may interpret all comments as mandatory, even if some comments are merely intended to be informational or optional.

可以加一下 comment 的标签，这些标签可以让 reviewer 的意图更加清晰地传达给 develepor,同时让 developer 知道这些 comments 的优先级，不会把所有的 comments 都当成强制性的。

## Accepting Explanations
Adding a comment in the code is an appropriate way, as long as it's not just explaining overly complex code.