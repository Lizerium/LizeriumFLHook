# Available Hooks — FLHook Hooks

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./Available%20Hooks%20—%20Хуки%20FLHook.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

---

- [Available Hooks — FLHook Hooks](#available-hooks--flhook-hooks)
  - [Introduction](#introduction)
- [Core Plugin System Functions](#core-plugin-system-functions)
  - [`Get_PluginReturnCode`](#get_pluginreturncode)
    - [Purpose](#purpose)
    - [Important](#important)
  - [`Get_PluginInfo`](#get_plugininfo)
    - [Purpose](#purpose-1)
    - [Important](#important-1)
  - [`Plugin_Communication_CallBack`](#plugin_communication_callback)
    - [Purpose](#purpose-2)
- [FLServer Hooks](#flserver-hooks)
- [1. Server Lifecycle](#1-server-lifecycle)
    - [Shutdown](#shutdown)
    - [Startup](#startup)
    - [AFTER variant](#after-variant)
    - [Update](#update)
- [2. Player: connection, login, character selection](#2-player-connection-login-character-selection)
    - [OnConnect](#onconnect)
    - [Login](#login)
    - [CharacterSelect](#characterselect)
    - [CreateNewCharacter / DestroyCharacter](#createnewcharacter--destroycharacter)
    - [Disconnect](#disconnect)
- [3. Player: launch, docking, transitions](#3-player-launch-docking-transitions)
    - [PlayerLaunch](#playerlaunch)
    - [LaunchComplete](#launchcomplete)
    - [BaseEnter / BaseExit](#baseenter--baseexit)
    - [JumpInComplete](#jumpincomplete)
    - [SystemSwitchOutComplete](#systemswitchoutcomplete)
    - [LaunchPosHook](#launchposhook)
- [4. Chat and user commands](#4-chat-and-user-commands)
    - [SubmitChat](#submitchat)
    - [UserCmd\_Process](#usercmd_process)
    - [UserCmd\_Help](#usercmd_help)
- [5. Admin commands](#5-admin-commands)
    - [ExecuteCommandString\_Callback](#executecommandstring_callback)
    - [CmdHelp\_Callback](#cmdhelp_callback)
- [6. Combat and damage](#6-combat-and-damage)
    - [FireWeapon](#fireweapon)
    - [ShipDestroyed](#shipdestroyed)
    - [AllowPlayerDamage](#allowplayerdamage)
    - [Damage hooks](#damage-hooks)
- [7. Economy and items](#7-economy-and-items)
    - [Cash](#cash)
    - [Market](#market)
    - [Inventory](#inventory)
    - [Equipment](#equipment)
    - [Cargo](#cargo)
- [8. Player trading](#8-player-trading)
- [9. Movement and control](#9-movement-and-control)
    - [Ship systems](#ship-systems)
    - [Actions](#actions)
    - [SPRequestUseItem](#sprequestuseitem)
- [10. Navigation \& missions](#10-navigation--missions)
- [11. Mining \& objects](#11-mining--objects)
    - [MineAsteroid](#mineasteroid)
    - [Collision hooks](#collision-hooks)
- [12. Core systems](#12-core-systems)
    - [Tradelane](#tradelane)
    - [Docking (VERY IMPORTANT)](#docking-very-important)
    - [Time hooks](#time-hooks)
- [FLHook Callbacks](#flhook-callbacks)
    - [HkTimerCheckKick](#hktimercheckkick)
    - [SendDeathMsg](#senddeathmsg)
    - [ClearClientInfo](#clearclientinfo)
    - [LoadSettings](#loadsettings)
    - [BaseDestroyed](#basedestroyed)
- [Most Useful Hooks](#most-useful-hooks)
    - [Player logic](#player-logic)
    - [Commands](#commands)
    - [Combat](#combat)
    - [Economy](#economy)
    - [Advanced logic](#advanced-logic)

---

## Introduction

If a required hook is missing for a specific FLServer function, historically it was suggested to discuss it on the FLHook Plugin Version board at **the-starport.net**.

Below is a list of available hooks in the current FLHook version.

> [!NOTE]
> If no calling convention is specified, the default is:

```cpp
__cdecl
```

---

# Core Plugin System Functions

## `Get_PluginReturnCode`

```cpp
PLUGIN_RETURNCODE *Get_PluginReturnCode()
```

### Purpose

Controls what FLHook should do **after your plugin is executed**.

### Important

> [!IMPORTANT]
> This function **does NOT need to be registered as a hook**.

---

## `Get_PluginInfo`

```cpp
list<PLUGIN_INFO>* Get_PluginInfo()
```

### Purpose

Returns plugin metadata and replaces old `.ini` registration.

### Important

> [!IMPORTANT]
> This function **does NOT need to be registered as a hook**.

---

## `Plugin_Communication_CallBack`

```cpp
void Plugin_Communication_CallBack(PLUGIN_MESSAGE msg, void* data)
```

### Purpose

Callback for **inter-plugin communication**.

---

# FLServer Hooks

Hooks tied to FLServer internal events and functions.

---

# 1. Server Lifecycle

### Shutdown

Called when server shuts down.

### Startup

Called when server starts (before loading player data).

### AFTER variant

Called after full initialization.

### Update

Runs every server tick.

Used for:

- timers
- periodic checks
- continuous logic

---

# 2. Player: connection, login, character selection

### OnConnect

Triggered when client connects.

### Login

Triggered on login.

> Important: plugins are called **after Login**

Use `OnConnect` if you need logic **before login**.

### CharacterSelect

Character selection event.

### CreateNewCharacter / DestroyCharacter

Character creation and deletion.

### Disconnect

Triggered on player disconnect.

---

# 3. Player: launch, docking, transitions

### PlayerLaunch

Ship launch into space.

### LaunchComplete

Launch finished.

### BaseEnter / BaseExit

Dock / undock.

### JumpInComplete

System jump completed.

### SystemSwitchOutComplete

Leaving system.

### LaunchPosHook

Allows overriding spawn position.

Used for:

- teleport
- custom undock
- mobile bases

---

# 4. Chat and user commands

### SubmitChat

Intercepts chat messages.

Used for:

- custom commands
- filtering
- logging
- anti-spam

### UserCmd_Process

Main hook for adding custom commands.

Return `true` if handled.

### UserCmd_Help

Custom help output.

---

# 5. Admin commands

### ExecuteCommandString_Callback

Add custom admin commands.

Return `true` if handled.

### CmdHelp_Callback

Extend admin help.

---

# 6. Combat and damage

### FireWeapon

Triggered on weapon fire.

### ShipDestroyed

Triggered on ship destruction.

One of the most useful hooks.

### AllowPlayerDamage

Control PvP damage safely.

### Damage hooks

- `HkCb_AddDmgEntry`
- `MissileTorpHit`
- `GeneralDmg`

---

# 7. Economy and items

### Cash

- `ReqChangeCash`
- `ReqSetCash`

### Market

- `GFGoodBuy`
- `GFGoodSell`

### Inventory

- `ReqAddItem`
- `ReqModifyItem`
- `ReqRemoveItem`

### Equipment

- `ReqEquipment`

### Cargo

- `JettisonCargo`
- `TractorObjects`
- `SPScanCargo`

---

# 8. Player trading

Full control over trading:

- initiate
- accept
- modify
- cancel

---

# 9. Movement and control

### Ship systems

- equipment
- cruise
- thrusters

### Actions

- targeting
- maneuvers
- weapon groups

### SPRequestUseItem

Item usage.

---

# 10. Navigation & missions

Handles:

- pathfinding
- stats
- UI
- missions

---

# 11. Mining & objects

### MineAsteroid

Mining control hook.

### Collision hooks

- projectile collisions
- object updates

---

# 12. Core systems

### Tradelane

- `GoTradelane`
- `StopTradelane`

### Docking (VERY IMPORTANT)

`HkCb_Dock_Call` — one of the most powerful hooks.

Used for:

- custom docking
- restrictions
- distance checks
- mobile docking

### Time hooks

- global time
- simulation loop

---

# FLHook Callbacks

### HkTimerCheckKick

Runs every second.

Perfect for:

- timers
- delayed logic
- cleanup

### SendDeathMsg

Customize death messages.

### ClearClientInfo

Reset player state.

### LoadSettings

Reload configs (`rehash`).

### BaseDestroyed

Triggered when a base is destroyed.

---

# Most Useful Hooks

### Player logic

- PlayerLaunch
- BaseEnter / Exit
- Login
- Disconnect

### Commands

- UserCmd_Process
- ExecuteCommandString

### Combat

- ShipDestroyed
- AllowPlayerDamage

### Economy

- GFGoodBuy / Sell
- ReqEquipment

### Advanced logic

- Dock_Call
- LaunchPosHook
- MineAsteroid
- TimerCheckKick

---
