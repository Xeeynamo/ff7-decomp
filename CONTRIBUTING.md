
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

* Your approach can be justified.
* You understand and can explain the code you are submitting.
* You are prepared to take responsibility for the submitted changes.

Contributors must be confident in the changes they submit. Once a Pull Request is opened, the contributor is responsible for understanding and defending the implementation. Assistance from coding tools or agents may be used during development, but submitted changes must remain understandable, justifiable, and manually operable by the contributor. Pull Requests and commit messages must not be generated autonomously by an LLM or coding agent. If project maintainers determine that these guidelines are being ignored, a Pull Request may be closed without notice.

- Submitted decompiled code must byte match and follow the original developer's intention. Raw code that closely resembles machine-generated artifact has no place to exist in this project.
- Naming symbols such as function names, variables and structs is not required as part of decompiling code. Renaming a symbol must be a high-confidence change, also confirmed with a debugger. Wrong symbol names are harmful code debt.
- The reconstructed source code you will submit aims to save what has been lost to time. What you will bring back will be helpful to developers, modders, speedrunners, researchers and to the Square Enix team themselves. We are looking for contributions that reflect this mission.

- You do not need to know assembly or MIPS to start with the project. We strongly recommend to get in touch before proposing your first changes. If you are not a programmer, supporting the project with documentation through debugging and code analysis is also accepted.

## How Do I Play the Game?

FF7-decomp is **not** a freely downloadable or standalone playable version of Final Fantasy VII. This repository contains a decompilation and reconstruction of the game's code. It does not include the original game data or provide a complete game as a free download. Building the project requires access to binary images of the original **Final Fantasy VII PlayStation discs**.

Contributors and users are responsible for obtaining and using the original game data appropriately.


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
