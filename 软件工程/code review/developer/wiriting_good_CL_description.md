# Writing good CL descriptions
https://google.github.io/eng-practices/review/developer/cl-descriptions.html

A CL decription is a public record of what change is being made and why it was made. It will become a permanent part of our version control history, and will possibly be read by hundreds of people other than your reviewers over the years.


## First Line
- Short summary of what is being onde
- Complete sentance, written as though is was on order.
- Follow by empty line

The first line of a CL description should be a short summary of specifically what is being done by the CL, followed by a balank line.
Try to keep your first line short, focused, and to the point, The clarity and utility to the reader should be the top concern.

CL decription 的第一行应该简洁明了的说明这个 CL。

## Body is informative

The rest of the description should fill int the details and include any supplemental information a reader need to understand the changelist holistically.
It minght include a brief description of the problem that's being solved, and why this is the best approach.If there are any shortcomings to the approach, they should be mentioned. If relevant, include background information such as bug numbers, benchmark results, and links to design documents.

其他部分的描述应该是一些细节的描述，包括这个问题被解决的方法，讲明它为什么是最后的方案，如果是折中的方案，它的缺点也要说明。

## Bad CL Descriptions
Bad descriptions :
- “Fix build.”
- “Add patch.”
- “Moving code from A to B.”
- “Phase 1.”
- “Add convenience functions.”
- “kill weird URLs.”

## Good CL Descriptions
**Functionality change**
Example:
>rpc: remove size limit on RPC server message freelist.
>
> Servers like FizzBuzz have very large messages and would benefit from reuse. Make the freelist larger, and add a goroutine that frees the freelist entries slowly over time, so that idle servers eventually release all freelist entries.

The first few words describe what the CL actually does. The rest of the description talks about the problem being solved, why this is a good solution, and a bit more information about the specific implementation.

第一行说明这个 CL 是做什么的。
其他部分对这个方案进行说明，为什么这个好的方案，也补充了一些关于具体的实施方案。

**Refactoring**
Example:
> Construct a Task with a TimeKeeper to use its TimeStr and Now methods.
>
> Add a Now method to Task, so the borglet() getter method can be removed (which was only used by OOMCandidate to call borglet’s Now method). This replaces the methods on Borglet that delegate to a TimeKeeper.
>
> Allowing Tasks to supply Now is a step toward eliminating the dependency on Borglet. Eventually, collaborators that depend on getting Now from the Task should be changed to use a TimeKeeper directly, but this has been an accommodation to refactoring in small steps.
>
> Continuing the long-range goal of refactoring the Borglet Hierarchy.

The first line describes what the CL does and how this is a change from the past. The rest of the description talks about the specific implementation, the context of the CL, that the solution isn’t ideal, and possible future direction. It also explains why this change is being made.



**Small CL that needs some context**
Example:
> Create a Python3 build rule for status.py.
> 
> This allows consumers who are already using this as in Python3 to depend on a rule that is next to the original status build rule instead of somewhere in their own tree. It encourages new consumers to use Python3 if they can, instead of Python2, and significantly simplifies some automated build file refactoring tools being worked on currently.

The first sentence describes what’s actually being done. The rest of the description explains why the change is being made and gives the reviewer a lot of context.


## Review the description before submitting the CL
CLs can undergo significant change during review. It can be worthwhile to review a CL description before submitting the CL, to ensure that the description still reflects what the CL does.