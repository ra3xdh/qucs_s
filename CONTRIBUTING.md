# Qucs-S guidelines

## Communicating and contributing
### I have a question…
If you have a question, please use [discussions](https://github.com/ra3xdh/qucs_s/discussions) to ask it. Restrain from opening an issue: it will be transfered to discussions if it's a question only.

### I want to report a bug…
Check if there is no existing issue for the bug you've discovered. If there is no one, then feel free to open a new issue.

Write a concise title, shortly describing the bug. Don't go all along there, provide details in the description.

Write the description:
1. Mention the platform (Windows, Linux), QT and Qucs-S version
2. Show how to reproduce the bug step by step
3. Add anything that would ease reproducing and/or understanding the problem: screenshots, videos, error messages.
If the bug is related to schematic and its elements, then provide the schematic file. If you can't provide it for any reason (NDA, etc.), try to make a minimal substitute which is enough to reproduce the bug.

### I want to open a PR…
#### General rules:
1. Your branch must merge without any problem
2. Your changes must be about one thing (i.e. one logical unit) be it a bugfix or a new feature or refactoring, etc. Please restrain from making "packs" or other compound forms of fixes/features/…
3. Commits must be atomic, relatively small, easy to digest. Think in advance how easy it will be to review the changes you offer.
4. Commit messages must follow a general structure: 
    - brief description on the first line
    - blank line 
    - detailed description
   You can omit blank line and detailed description if have nothing to say. Commit messages must be no wider than 80 columns.
5. PR title must be concise, briefly describing what the PR does: adds a feature, removes smth, fixes a bug, etc.
6. In PR description write all about the changes you offer, like the intention behind them, what is fixed/added/removed, etc.

#### Rules for current release bugfixes
1. Use `release/YY.N` as a base branch for your bugfix branch
2. If you make a fix for a specific issue, then make its ID the first word in branch name. For example '310-fix-window-size'
3. Don't be afraid to use GitHub's facilities to [link](https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue) a PR to an issue.

#### Rules for other PRs
1. Use `current` as a base branch for your changes

## Tooling and code guidelines
Code base is old and at the moment it doesn't follow any rules uniformly. But for your changes please follow these guidelines:
- Prefer modern C++ features, everything up to and including C++17 is OK
- Use `camelCaseWithSmallFirstLetter` for variable names
- Do not write `if`, `for`, etc. without a code block even when it contains a single statement
Bad:
```c++
if (blah-blah) 
  doFooBar();
```
Good:
```c++
if (blah-blah) {
  doFooBar();
}
```
- Do not use tabs and use <TODO> spaces for intendation