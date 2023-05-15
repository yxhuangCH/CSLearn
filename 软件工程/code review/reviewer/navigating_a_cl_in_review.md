# Navigating a CL in Review
https://google.github.io/eng-practices/review/reviewer/navigate.html

## Summary
- Does the change make sense? Does it have a good description?
- Look at the most important part of the change first.Is it well-designed overall?
- Look at the rest of the CL in an appropriate sequence.

## Step One: Take a broad view of the change
第一步是先看 PR 的描述，对 PR 有个初步的了解，知道这个 PR 是干嘛的。
Look at the CL decription and what the  CL does in general.
Does this change even make sense? If this change shoulddn't have happened in the first place, respond immediately with an explanation of why the change should not be happening. and the same time, suggest the developer what should have done.

**The reviewer reject the current CL and provide an alternative suggestion, but they did it courteously.**

## Step two: Examine the main parts of the CL
从“主要的部分”开始看起，这样可以有助于 Reviewer 对 CL 的了解
- First find the file or files that are the "main" part of this CL. This helps give context to all the smaller parts of the CL, and generally accelerates doing the code review.
- If some major design problems with this part of CL, reviewer should send those comments immediately, even if you don't have time to review the rest of the CL right now. 如果看到“主要部分”的代码有问题，应该理解加 comment,让开发者了解到这些问题这样做的好处是让开发者可以快速调整，而不是全部做完再全部返工，这样避免了开发大量的调整的工作
## Step Three: Look through the rest of the CL in an appropriate sequence
There are no major design problems with the CL as a whole, try to figure out a logical sequence to look through the files.
Sometimes it's also helpful to read the ests first before you read the main code, because then you have an idea of what is supposed to be doing.
如果主要的设计部分没有问题，就可以仔细去看每个文件里面的逻辑等问题。有时候在review 主要部分代码之前，可以先看一下这部分的测试代码，通过这些测试代码，可以让 reviewer 掌握这些代码改动的意图。

