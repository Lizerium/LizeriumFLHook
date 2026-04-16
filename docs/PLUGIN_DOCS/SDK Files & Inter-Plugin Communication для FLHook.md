# SDK Files & Inter-Plugin Communication for FLHook

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./SDK%20Files%20%26%20Inter-Plugin%20Communication%20для%20FLHook.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

---

- [SDK Files \& Inter-Plugin Communication for FLHook](#sdk-files--inter-plugin-communication-for-flhook)
	- [SDK Files](#sdk-files)
	- [Build Notes](#build-notes)
- [`plugin.h`](#pluginh)
- [Inter-Plugin Communication](#inter-plugin-communication)
	- [How it works](#how-it-works)
- [Message Handler Example](#message-handler-example)
- [What’s happening here](#whats-happening-here)
		- [`msg`](#msg)
		- [`data`](#data)
- [Working with `data`](#working-with-data)
- [What can be passed](#what-can-be-passed)
- [Important requirement](#important-requirement)
	- [Old system](#old-system)
	- [New system (`Get_PluginInfo`)](#new-system-get_plugininfo)
- [How to send a message](#how-to-send-a-message)
- [What happens](#what-happens)
- [Practical usage](#practical-usage)
- [Example scenario](#example-scenario)
	- [Plugin A](#plugin-a)
	- [Plugin B](#plugin-b)
- [Advantages](#advantages)
- [Summary](#summary)
- [Minimal template](#minimal-template)
	- [Receive](#receive)
	- [Send](#send)

---

## SDK Files

In the folder:

```text
src/sdk
```

you will find header (`.h`) and library (`.lib`) files required for developing FLHook plugins.

They provide the interface to:

- call **original FLServer functions**
- use **FLHook exported functions**
- build **inter-plugin communication**

---

## Build Notes

> [!IMPORTANT]
> Don’t forget to add required `.lib` files to your **Linker Dependencies**
> otherwise your project may fail to build or resolve symbols.

---

# `plugin.h`

One of the most important SDK files:

```text
plugin.h
```

It contains:

- `PLUGIN_RETURNCODE`
- plugin base structures
- inter-plugin communication interface
- message exchange definitions between DLLs

---

# Inter-Plugin Communication

FLHook allows plugins to **communicate with each other** via a messaging system.

This means one plugin can:

- send commands to another
- transfer data
- trigger actions in another module

---

## How it works

FLHook provides:

```cpp
Plugin_Communication(...)
```

And the receiving plugin must implement:

```cpp
Plugin_Communication_CallBack
```

---

# Message Handler Example

```cpp
__declspec(dllexport) void Plugin_Communication_CallBack(PLUGIN_MESSAGE msg, void* data)
{
	if(msg == DO_SOMETHING) {
		CUSTOM_STRUCT* incoming_data = reinterpret_cast<CUSTOM_STRUCT*>(data);

		// process data
	}
}
```

---

# What’s happening here

### `msg`

Message type / command identifier

### `data`

Pointer to arbitrary data passed with the message

---

# Working with `data`

Since `data` is:

```cpp
void*
```

You must know what was sent and cast it manually.

```cpp
CUSTOM_STRUCT* incoming_data = reinterpret_cast<CUSTOM_STRUCT*>(data);
```

---

# What can be passed

> [!NOTE]
> `CUSTOM_STRUCT` can be **any structure of any size**

You can pass:

- player IDs
- character names
- coordinates
- flags
- custom state
- any internal data

The only requirement: **both plugins must agree on the structure**

---

# Important requirement

> [!CAUTION]
> `Plugin_Communication_CallBack` MUST be registered as a hook
> otherwise FLHook will never call it.

---

## Old system

Was defined in `[Hooks]`

---

## New system (`Get_PluginInfo`)

Register it like:

```cpp
p_PI->mapHooks.insert(pair<string, int>("Plugin_Communication_CallBack", 0));
```

---

# How to send a message

```cpp
CUSTOM_STRUCT outgoing_data;
Plugin_Communication(DO_SOMETHING, &outgoing_data);
```

---

# What happens

1. Data structure is created
2. `Plugin_Communication` is called
3. All subscribed plugins receive it
4. Each plugin decides:
   - ignore
   - or handle

---

# Practical usage

Useful for modular architecture:

- Plugin A → event detection
- Plugin B → state storage
- Plugin C → UI / logic

They communicate without direct DLL calls.

---

# Example scenario

## Plugin A

Detects event:

- player docked
- cloak enabled
- base state changed

Sends:

```cpp
Plugin_Communication(DO_SOMETHING, &outgoing_data);
```

---

## Plugin B

Receives in:

```cpp
Plugin_Communication_CallBack(...)
```

Executes logic.

---

# Advantages

- low coupling between DLLs
- modular design
- easy extensibility
- clean architecture

---

# Summary

- `src/sdk` → development base
- `plugin.h` → core file
- `Plugin_Communication` → messaging system
- `Plugin_Communication_CallBack` → receiver

---

# Minimal template

## Receive

```cpp
__declspec(dllexport) void Plugin_Communication_CallBack(PLUGIN_MESSAGE msg, void* data)
{
	if (msg == DO_SOMETHING)
	{
		CUSTOM_STRUCT* incoming_data = reinterpret_cast<CUSTOM_STRUCT*>(data);

		// handle
	}
}
```

---

## Send

```cpp
CUSTOM_STRUCT outgoing_data;
Plugin_Communication(DO_SOMETHING, &outgoing_data);
```

---
