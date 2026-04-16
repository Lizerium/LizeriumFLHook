# FLHook Plugin Development

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./Разработка%20плагинов%20для%20FLHook.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

---

- [FLHook Plugin Development](#flhook-plugin-development)
	- [Introduction](#introduction)
	- [Other Documentation Parts](#other-documentation-parts)
	- [How it works](#how-it-works)
	- [Source Code Examples](#source-code-examples)
		- [Available examples:](#available-examples)
- [Plugin Information](#plugin-information)
	- [Changes since FLHook 1.6.1](#changes-since-flhook-161)
	- [Required exported function](#required-exported-function)
- [Example `Get_PluginInfo`](#example-get_plugininfo)
- [What `Get_PluginInfo` does](#what-get_plugininfo-does)
- [PLUGIN\_INFO fields](#plugin_info-fields)
		- [`sName`](#sname)
		- [`sShortName`](#sshortname)
		- [`bMayPause`](#bmaypause)
		- [`bMayUnload`](#bmayunload)
		- [`mapHooks`](#maphooks)
- [How `mapHooks` works](#how-maphooks-works)
- [Hook priority](#hook-priority)
	- [Meaning](#meaning)
	- [Example](#example)
	- [Practical meaning](#practical-meaning)
- [Recommendations](#recommendations)
- [Summary](#summary)

---

## Introduction

Let’s start with a quick overview of **how the FLHook plugin system works**.

Each plugin is a **DLL library** that exports specific functions.  
These functions are then called by **FLHook**.

---

## Other Documentation Parts

- Plugin How-To for FLHook
- SDK Files & Inter-Plugin Communication
- Troubleshooting for FLHook Plugins
- Available Hooks — FLHook Hooks

---

## How it works

Example flow:

- player sends chat message
- FLHook calls `SubmitChat`
- FLHook checks loaded plugins
- if plugin exports this hook → it is called

So the pipeline is:

1. **FLHook hooks FLServer**
2. FLHook forwards events to plugins

---

## Source Code Examples

> [!IMPORTANT]
> Example plugin sources are included in:

```text
./src/
```

### Available examples:

- `advanced connection`
- `tempban`

These are well-commented and useful for learning.

---

# Plugin Information

## Changes since FLHook 1.6.1

Starting from:

```text
FLHook 1.6.1
```

Plugins no longer require `.ini` registration.

Now you only need to:

```text
Place DLL into flhook_plugins/
```

---

## Required exported function

Every plugin MUST export:

```cpp
Get_PluginInfo
```

This replaces old `.ini` registration.

---

# Example `Get_PluginInfo`

```cpp
EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "TempBan Plugin by w0dk4";
	p_PI->sShortName = "tempban";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->mapHooks.insert(pair<string, int>("HkTimerCheckKick", 0));
	p_PI->mapHooks.insert(pair<string, int>("HkIServerImpl::Login", 0));
	p_PI->mapHooks.insert(pair<string, int>("Plugin_Communication_CallBack", 0));
	p_PI->mapHooks.insert(pair<string, int>("ExecuteCommandString_Callback", 0));
	p_PI->mapHooks.insert(pair<string, int>("CmdHelp_Callback", 0));

	return p_PI;
}
```

---

# What `Get_PluginInfo` does

It tells FLHook:

- plugin name
- short name
- pause capability
- unload capability
- which hooks to subscribe to

---

# PLUGIN_INFO fields

### `sName`

Full plugin name

### `sShortName`

Technical identifier

### `bMayPause`

Can plugin be paused

### `bMayUnload`

Can plugin be unloaded at runtime

### `mapHooks`

List of hook functions

---

# How `mapHooks` works

```cpp
p_PI->mapHooks.insert(pair<string, int>("HkTimerCheckKick", 0));
```

Means:

- plugin subscribes to `HkTimerCheckKick`
- FLHook will call corresponding function in DLL

> [!CAUTION]
> If you list a hook → you MUST export it
> otherwise plugin will break or fail to load

---

# Hook priority

Second parameter = priority:

```cpp
p_PI->mapHooks.insert(pair<string, int>("SubmitChat", 3));
```

---

## Meaning

If multiple plugins use same hook:

👉 higher priority executes first

---

## Example

- anti-cheat → priority 3
- logger → priority 0

Execution order:

1. anti-cheat
2. logger

---

## Practical meaning

Important when:

- one plugin blocks event
- logging must happen after logic
- multiple plugins modify same behavior

---

# Recommendations

Before writing plugin:

- study `tempban`
- study `advanced connection`
- explore available hooks
- understand FLHook lifecycle

---

# Summary

FLHook plugin system:

- FLHook hooks FLServer
- distributes events to plugins
- plugins subscribe via `Get_PluginInfo`

This makes FLHook:

- flexible
- modular
- extendable without core modification

---
