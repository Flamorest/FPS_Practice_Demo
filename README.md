# UE5 C++ First Person Multiplayer Shooter Demo

A lightweight first-person shooter prototype built with **Unreal Engine 5.6**.
The project extends UE5's official First Person template into a playable demo featuring enemy AI, scoring, player death/replay flow, and minimal Listen Server multiplayer combat.

---

## Overview

This demo focuses on building a compact but complete FPS gameplay loop:

* First-person movement and shooting
* Server-authoritative hit detection
* Damageable targets, enemies, and players
* Enemy chase / attack behavior
* Per-player score, kill, and death tracking
* Victory / defeat state
* Death screen with replay
* Listen Server multiplayer test flow

The implementation is primarily written in **C++**, while Blueprint is used for asset assignment, animation setup, widget configuration, and level placement.

---

## Core Features

### Single Player Gameplay

* LineTrace-based first-person shooting
* Shooting targets with score feedback
* AI enemy that detects, chases, and attacks the player
* Enemy health, death, score reward, and animation feedback
* Player health, death state, and replay flow
* HUD display for health, score, targets hit, enemies killed, and victory state

### Enemy AI and Animation

* NavMesh-based enemy movement
* Distance-based chase and attack state switching
* Attack cooldown control
* Idle / Walk / Run animation driven by movement speed
* Attack Montage support through `DefaultSlot`
* Server-controlled attack logic with replicated visual feedback

### Multiplayer Gameplay

* Listen Server test mode with two players
* Client input routed through Server RPC
* Server-authoritative LineTrace and damage application
* Replicated player health and death state
* Independent per-player score using PlayerState
* Victory / defeat display based on the winning player
* Local death screen shown only to the dead player

---

## Technical Architecture

| System       | Responsibility                                                    |
| ------------ | ----------------------------------------------------------------- |
| Character    | Player input, shooting, health, death, replay request             |
| Enemy        | Chase logic, attack logic, enemy health, death, animation trigger |
| GameMode     | Server-side gameplay rules, score assignment, victory judgment    |
| GameState    | Replicated global match state, target score, winner state         |
| PlayerState  | Per-player score, kills, deaths, player identity                  |
| HUD / Widget | Health, score, hit feedback, victory/defeat, death screen         |

The multiplayer logic follows a server-authoritative model:

```text
Client Input
    ↓
ServerFire() RPC
    ↓
Server-side LineTrace
    ↓
Apply Damage / Score
    ↓
Replicated State
    ↓
Client HUD Update
```

Clients can request actions, but health, score, death, and victory are resolved on the server.

---

## Controls

| Input             | Action              |
| ----------------- | ------------------- |
| WASD              | Move                |
| Mouse             | Look                |
| Left Mouse Button | Fire                |
| Space             | Jump                |
| R                 | Restart level       |
| Replay Button     | Respawn after death |

---

## Test Setup

### Single Player

Use the default Play mode with one player.

Expected test flow:

1. Shoot targets to gain score.
2. Let the enemy chase and attack the player.
3. Kill the enemy to increase score and enemy kill count.
4. Reach the target score to trigger Victory.
5. Die, enter the You Died screen, and click Replay to respawn.

### Listen Server Multiplayer

Recommended PIE settings:

```text
Number of Players: 2
Net Mode: Play As Listen Server
Run Dedicated Server: Off
Play Mode: New Editor Window
```

Expected multiplayer test flow:

1. Both players can move and shoot independently.
2. Server player can damage client player.
3. Client player can damage server player through Server RPC.
4. Killing another player increases only the killer's score.
5. The dead player sees the local You Died screen.
6. Replay respawns only the dead player.
7. Reaching the target score shows Victory for the winner and Defeat for the other player.

---

## Demo Files

Suggested submission files:

```text
01_SinglePlayer_Gameplay_Demo.mp4
02_ListenServer_Multiplayer_Demo.mp4
03_UE5_FPS_Demo_Report.pdf
```

---

## Project Scope

This is a gameplay prototype focused on core FPS and multiplayer logic.
It does not include a full lobby system, matchmaking, dedicated server deployment, weapon inventory, ammo management, advanced enemy behavior trees, or production-level UI.

Potential extensions:

* Weapon switching and ammo system
* Dedicated server workflow
* Lag compensation
* Full UMG scoreboard
* Behavior Tree enemy AI
* More complete player animation and hit reaction system

---

## Environment

```text
Engine: Unreal Engine 5.6
Language: C++
Template: UE5 First Person Template
Network Mode: Listen Server
Platform: Windows
```

---

## Repository Notes

Generated folders such as `Binaries/`, `Intermediate/`, `Saved/`, `.vs/`, and `DerivedDataCache/` are excluded from version control.

Open the `.uproject` file with Unreal Engine 5.6 and rebuild the C++ project if required.
