# Plugin How-To for FLHook

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./Plugin%20How-To%20для%20FLHook.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

---

- [Plugin How-To for FLHook](#plugin-how-to-for-flhook)
	- [Basics](#basics)
	- [Example: hook `HkIServerImpl::OnConnect`](#example-hook-hkiserverimplonconnect)
	- [Implementation Example](#implementation-example)
- [Controlling FLHook Behavior After Plugin Execution](#controlling-flhook-behavior-after-plugin-execution)
- [Example using `Get_PluginReturnCode`](#example-using-get_pluginreturncode)
- [What is `PLUGIN_RETURNCODE`](#what-is-plugin_returncode)
	- [Logic Example](#logic-example)
- [How FLHook Handles This](#how-flhook-handles-this)
- [Why Plugin Priority Matters](#why-plugin-priority-matters)
- [Important Notes About `Get_PluginReturnCode`](#important-notes-about-get_pluginreturncode)
	- [If you use `Get_PluginReturnCode`](#if-you-use-get_pluginreturncode)
- [`g_bPlugin_nofunctioncall`](#g_bplugin_nofunctioncall)
	- [What it means](#what-it-means)
	- [Why it exists](#why-it-exists)
- [Practical Meaning](#practical-meaning)
	- [1. Just listen](#1-just-listen)
	- [2. Control execution](#2-control-execution)
	- [3. React to other plugins](#3-react-to-other-plugins)
- [Summary](#summary)
- [Minimal Template](#minimal-template)

---

## Basics

This is where things get interesting — or painful if something goes wrong.

The idea is simple:

If you want FLHook to call your function on a specific event, you must:

1. Subscribe to the required **hook**
2. Export a function with the **exact same signature**

---

## Example: hook `HkIServerImpl::OnConnect`

Suppose you hook into:

```text
HkIServerImpl::OnConnect
```

The documented prototype is:

```cpp
void __stdcall HkIServerImpl::OnConnect(unsigned int)
```

Your function must match this signature **exactly**.

---

## Implementation Example

```cpp
namespace HkIServerImpl
{
	__declspec(dllexport) void __stdcall OnConnect(unsigned int iClientID)
	{
		// do something here
		return;
	}
}
```

That’s it.
FLHook will now call your function when a client connects.

---

# Controlling FLHook Behavior After Plugin Execution

If you want more than just listening — if you want to **affect execution flow** — things get more interesting.

You might want to:

- prevent original FLHook/FLServer function from running
- stop other plugins from executing
- fully override behavior

For this, FLHook provides:

```cpp
Get_PluginReturnCode
```

---

# Example using `Get_PluginReturnCode`

```cpp
PLUGIN_RETURNCODE returncode;

__declspec(dllexport) PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

__declspec(dllexport) int __stdcall A_Hooked_Function(unsigned int iSomething)
{
	// do something here

	// block everything else
	returncode = SKIPPLUGINS_NOFUNCTIONCALL;

	return 0;
}
```

---

# What is `PLUGIN_RETURNCODE`

It is an enum defined in:

```text
sdk/headers/plugin.h
```

It tells FLHook **what to do after your hook finishes**.

---

## Logic Example

```cpp
returncode = SKIPPLUGINS_NOFUNCTIONCALL;
```

This means:

- do NOT call other plugins
- do NOT call original function
- return your function result

---

# How FLHook Handles This

Internally:

1. Your hook function is executed
2. FLHook immediately calls:

```cpp
Get_PluginReturnCode()
```

3. Based on the result it decides:
   - call next plugins or not
   - call original function or not
   - stop execution

---

# Why Plugin Priority Matters

Now you understand why **priority** in `Get_PluginInfo` is critical.

Example:

- anti-cheat plugin (high priority)
- logger plugin (low priority)

If anti-cheat returns:

```cpp
SKIPPLUGINS_NOFUNCTIONCALL
```

👉 logger will NEVER be called

---

# Important Notes About `Get_PluginReturnCode`

> [!IMPORTANT]
> If your DLL does NOT export `Get_PluginReturnCode`,
> FLHook uses default behavior:

- call all plugins
- call original function

---

## If you use `Get_PluginReturnCode`

> [!CAUTION]
> ALWAYS set `returncode` inside every hook

Otherwise you get unpredictable behavior.

---

# `g_bPlugin_nofunctioncall`

From `plugin.h`:

```cpp
extern __declspec(dllimport) bool g_bPlugin_nofunctioncall;
```

---

## What it means

If:

```cpp
g_bPlugin_nofunctioncall == true
```

Then:

> original function MUST NOT be called

Reasons:

1. previous plugin already handled it
2. previous plugin blocked it

---

## Why it exists

For **plugin compatibility**.

Example:

Hooks like:

```text
IServerImpl::PlayerLaunch
```

One plugin may override behavior.
Other plugins must detect this and adapt.

---

# Practical Meaning

You have 3 levels of control:

---

## 1. Just listen

```cpp
void __stdcall OnConnect(...)
```

---

## 2. Control execution

```cpp
Get_PluginReturnCode()
```

---

## 3. React to other plugins

```cpp
g_bPlugin_nofunctioncall
```

---

# Summary

- want to handle event → export correct function
- want to control flow → use `Get_PluginReturnCode`
- want compatibility → check `g_bPlugin_nofunctioncall`

---

# Minimal Template

```cpp
PLUGIN_RETURNCODE returncode;

__declspec(dllexport) PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

namespace HkIServerImpl
{
	__declspec(dllexport) void __stdcall OnConnect(unsigned int iClientID)
	{
		returncode = DEFAULT_RETURNCODE;

		// your code here
	}
}
```

---
