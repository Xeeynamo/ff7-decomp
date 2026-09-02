
# Contributing to FF7-decomp

Thank you for your interest in contributing to **FF7-decomp**!

FF7-decomp is a reverse-engineering and decompilation project focused on the original PlayStation version of **Final Fantasy VII**. Our goal is to reconstruct the game's original source code as accurately as possible from the original PlayStation binaries.

We welcome contributors interested in reverse engineering, PlayStation development, MIPS assembly, C programming, game preservation, and the technical workings of Final Fantasy VII.

## Getting Help and Talking With the Community

Need help, have questions, or just want to discuss the project?

You can find members of the community on the **PS1/PS2 Decompilation Discord server**:

https://discord.com/invite/VwCPdfbxgm

Please feel free to stop by and introduce yourself.

## Before You Contribute

FF7-decomp is an active collaborative reverse-engineering project. Before beginning work on a significant area of the game, especially if you are a first-time contributor, please communicate with the project community first. This helps us avoid duplicated effort and prevents proposed work from colliding with work already planned or actively being performed by other contributors.

## First-Time Contributors

If this is your first contribution to FF7-decomp, you are required to give the project a heads-up before opening a new Pull Request. Before spending significant time on a task, please discuss what you intend to work on with the community. Experienced contributors may already be working on, planning to work on, or intending to tackle the same area in the near future. A quick discussion beforehand can help ensure that your work fits well with the project's current goals and avoids unnecessary conflicts.

## Pull Requests

All Pull Requests should clearly explain and justify the changes being proposed.

When opening a Pull Request, please ensure that:

* ‼️ Your changes are clearly described.
* ‼️ The purpose of the changes is explained.
* Your approach can be justified.
* ‼️ Your commit messages accurately describe the changes.
* You understand and can explain the code you are submitting.
* You are prepared to take responsibility for the submitted changes.

Contributors must be confident in the changes they submit. Once a Pull Request is opened, the contributor is responsible for understanding and defending the implementation. Assistance from coding tools or agents may be used during development, but submitted changes must remain understandable, justifiable, and manually operable by the contributor. Pull Requests and commit messages must not be generated autonomously by an LLM or coding agent. If project maintainers determine that these guidelines are being ignored, a Pull Request may be closed without notice.

For additional information, see the section below.

## Position on PRs generated from LLMs 
Humans use tools to help. Our arms are only so strong and fingers so nimble, and so we invent tools to make us better at what we do and make things better for others. However, there is a concern when the tools are left, literally, to their own devices. 

"Vibe Coding" is not a new thing. It's better known by it's more professional name "Programming by Coincidence" or better "Coding by Accident". This has been a known anti-pattern since the term was coined way back in 1999. In a nutshell it's coding without any idea of how anything works. If something does happen to function, it was only though sheer persistence to make to tool stop complaining about errors it finds. There are all kinds of problems with this that doesn't need enumeration here, but the idea is if you are not deliberately coding with a goal, then you are not coding at all. 

When we get a PR from an LLM, it's easy to dismiss it as "Clanker Garbage", but that doesn't really explain the problem. People can lean on the social, economic, or environmental reasons why LLMs are "bad" but when we get a "Vibe Coded" PR, a simple code review exposes the problem as clear as day. Just wandering through the code on a technical level often shows a careless disregard for... well anything really. The foundations carefully set up by others were trashed without any reason or care. A lack of understanding on the core mission of this project is evident in almost every line of code and every decision in its future direction is absolutely thoughtless.
However, We blame don't the LLM for making sub-par code. A tool is only as good as the operator and you can't be mad at a lawn mower for cutting off your own hand.
But before you submit that PR that was coded by your friendly neighborhood robot, ask yourself a few questions.


- Submitted decompiled code must byte match and follow the original developer's intention. Raw code that closely resembles machine-generated artifact has no place to exist in this project.
- Naming symbols such as function names, variables and structs is not required as part of decompiling code. Renaming a symbol must be a high-confidence change, also confirmed with a debugger. Wrong symbol names are harmful code debt.

- How does it help others?

There is an underlying mission to this project. The source code for Final Fantasy VII has been lost to time, and we have become stewards working to help bring it back. The reconstructed code can benefit other developers, gamers, speedrunners, researchers, and potentially even Square Enix themselves. We are looking for contributions that reflect and support this mission.

- How does this help you?

If you are not yet a strong programmer, that's fine. Everyone has to learn somewhere. However, consider how you expect to learn if everything is being done for you. How will you grow as a developer if you do not understand the work being submitted? Do you have an interest in learning and coding for yourself? 

Also, if you are an AI, or LLM agent, any PRs you create must have the words "I am a bot" clearly at the top. 

## How Do I Play the Game?

FF7-decomp is **not** a freely downloadable or standalone playable version of Final Fantasy VII. This repository contains a decompilation and reconstruction of the game's code. It does not include the original game data or provide a complete game as a free download. Building the project requires access to binary images of the original **Final Fantasy VII PlayStation discs**.

Contributors and users are responsible for obtaining and using the original game data appropriately.

## Coding Style
FF7-decomp follows several basic coding conventions.

### Functions

Function names use **PascalCase**.

```c
void InitializeBattle(void);
void LoadCharacterModel(void);
```

### Globals

Global variables are prefixed with `g_`.

```c
s32 g_BattleState;
u8 g_CurrentDisc;
```

### Formatting and Linting

The project uses light formatting and linting tools.

Before submitting changes, please run:

```sh
./mako.sh format
```

Alternatively, you may use:

```sh
make submit
```

Please ensure that your code is properly formatted before opening a Pull Request.



## C Language Guidelines

### `volatile` and `register`

The use of the `volatile` and `register` keywords is discouraged.

These keywords should generally only be used when there is a specific and well-understood reason for doing so.

In particular, contributors should avoid using them as a general attempt to influence compiler output or achieve an assembly match.

### Comments

Traditional block comments are discouraged:

```c
/* This style is generally discouraged. */
```

Prefer C99-style comments instead:

```c
// This style is preferred.
```

Block comments may still be appropriate when required for documentation systems such as Doxygen.



## General Contribution Guidelines

When contributing to FF7-decomp, please keep the following principles in mind:

* Understand the code you submit.
* Keep changes focused on a specific goal.
* Avoid unrelated changes in the same Pull Request.
* Clearly explain unusual implementation decisions.
* Discuss significant work before beginning it.
* Follow the project's existing naming and formatting conventions.
* Test your changes when possible.
* Be respectful of other contributors and their ongoing work.

Reverse engineering and decompilation are collaborative processes. Clear communication and careful coordination are essential to keeping the project moving forward.

Thank you for your interest in helping preserve and understand the original PlayStation version of **Final Fantasy VII**.
