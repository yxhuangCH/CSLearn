# Handing pushback in code reviews
https://google.github.io/eng-practices/review/reviewer/pushback.html
在 Code reivew 的时候，难免会有 review 的意见被推回的时候，这个时候可以参考下面的一些意见
## Who is right?
- When a developer disagrees with your suggestion, first take a moment to consider if they are correct.They are closer to the code than you are, and so they might really have a better insight about certain aspects of it.
当 develper 不同意 reviewer 的意见时，首先需要考虑他们是正确的，developer 比 reviewer 更接近代码，他们对代码有着更好的见解。
- Developers are not always right.In this case the reviewer should further explain why they belive that their suggestion is correct.
当然，developer 也不是经常是对的，这种情况的时候， review 需要进一步解析为为什么 reviewer 的建议是正确的。
- When the reviewer believes their suggesion will improve code health, they should continue to advocate for the change, if they believe the resulting code quality improvement justifies the additional work requested.**Improving code health tends to happen in small steps.**
提高代码质量在日常的小进步。当 reviewer 的建议有利于提高代码质量的时候，reviewer 应该继续向 developer 进行建议。

## Cleaning It Up later
To get their work done faster, developers may resist additional reviews for a change list (CL) and promise to clean up issues in a later CL. However, experience shows that these clean-up tasks are often forgotten. Insisting on cleaning up issues before a CL is approved and merged into the codebase is therefore best to prevent the codebase from deteriorating

If a CL introduces new complexity, it must be cleaned up before submission unless it is an emergency. They can optionally also write a TODO comment in the code that references the filed bug.

为了更快地完成工作，开发人员可能会抵制对变更列表（CL）进行额外的审查，并承诺在以后的 CL 中解决问题。然而，经验表明这些清理任务经常会被遗忘。因此，在批准和合并 CL 到代码库之前，坚持清理问题是防止代码库恶化的最佳方法。

如果一个 CL 引入了新的复杂性，除非出现紧急情况，否则必须在提交之前进行清理。他们还可以在代码中编写 TODO 注释，引用已提交的 bug。