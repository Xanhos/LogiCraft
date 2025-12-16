# ⚠️ Disclaimer: Educational Project Context

LogiCraft was developed during our first year of C++ studies. The project was built by three main contributors over a span of just three months.

At the time, we were unfamiliar with game engine architecture and learned the GameObject/GameComponent system "on the fly" while developing. As a result of these strict time constraints and our simultaneous learning process, the codebase is not optimized and would require a complete refactor to meet professional/usable standards. We have chosen to keep the repository in its original state to provide context on our development journey and ability to deliver under pressure.

With this in mind, enjoy the **Logicraft** !

---
# Logicraft 🎮

**Logicraft** is a 2D game engine and editor developed in **C++**. It allows users to create video game scenes through a **no-code interface**, enabling the placement of static assets, dynamic elements, and game logic without writing code.

![C++](https://img.shields.io/badge/Language-C++-00599C?style=flat-square)
![ImGui](https://img.shields.io/badge/GUI-ImGui-darkred?style=flat-square)
![Status](https://img.shields.io/badge/Status-Educational_Legacy-yellow?style=flat-square)

## 📖 About The Project

Logicraft was designed to emulate a simplified version of professional engines like Unity. The core goal was to build a tool that separates the **engine code** from the **game design**, allowing a user to assemble a scene visually.

The editor creates a hierarchy of objects where users can attach components to entities to give them behavior, appearance, and logic.

---

## ✨ Key Features

### 🎨 Rendering & Visuals
* **Static Assets:** Drag-and-drop support for PNG textures.
* **Animations:** Support for 2D sprite sheet animations.
* **Custom Shaders:** Implementation of specific shaders, including:
    * **Water Shader:** Distortion and reflection effects.
    * **Heat Shader:** Heat haze/distortion effects.
* **Parallax Scrolling:** Native support for multi-layered backgrounds to create depth.
* **Particle System:** Built-in emitter for dynamic visual effects.

### 🧠 Gameplay & Logic
* **Behavior Tree AI:** A simplified AI system allowing for the creation of basic NPC behaviors and decision-making.
* **Player Management:** dedicated systems for Player Spawning and state management.

### 🛠 Architecture & Editor
* **GameObject / Component System:** Inspired by Unity, this allows for a flexible composition of game entities.
* **Scene Hierarchy:** Objects are organized in a parent-child relationship tree.
* **ImGui Interface:** The entire editor GUI (inspector, hierarchy, asset browser) is built using **Dear ImGui**.

---

## ⚙️ Technical Stack

* **Language:** C++
* **UI Library:** Dear ImGui
* **Architecture:** Object-Oriented Composite pattern.

---

## 👥 Contributors

This engine was the result of a collaborative effort by:

* [Yann Grallan/Xanhos] https://github.com/Xanhos
* [Charles Lesage/sharllesse] https://github.com/sharllesse
* [Samy MENA-BOUR/SamyMENA-BOUR] https://github.com/SamyMENA-BOUR

---

*Thanks for checking out Logicraft!*
