# Troubleshooting for FLHook Plugins

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./Troubleshooting%20для%20FLHook%20Plugins.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

---

- [Troubleshooting for FLHook Plugins](#troubleshooting-for-flhook-plugins)
  - [Error Tracking](#error-tracking)
  - [What to look for in `flhook.log`](#what-to-look-for-in-flhooklog)
  - [Exceptions in callback functions](#exceptions-in-callback-functions)
  - [Recommendation](#recommendation)
- [Important: Runtime Library \& STL](#important-runtime-library--stl)
  - [Problem](#problem)
  - [Why it happens](#why-it-happens)
- [What MUST be done](#what-must-be-done)
  - [Correct setting (Release)](#correct-setting-release)
  - [Otherwise you get](#otherwise-you-get)
- [Debug Build](#debug-build)
- [Golden Rule](#golden-rule)
  - [Release + Release](#release--release)
  - [Debug + Debug](#debug--debug)
  - [Mixed](#mixed)
- [Performance Timer](#performance-timer)
  - [What it does](#what-it-does)
  - [When useful](#when-useful)
- [How timer logging works](#how-timer-logging-works)
  - [Parameter](#parameter)
  - [Meaning](#meaning)
- [Practical usage](#practical-usage)
- [What to do if server lags](#what-to-do-if-server-lags)
  - [Checklist](#checklist)
    - [1. Check](#1-check)
    - [2. Check](#2-check)
    - [3. Check](#3-check)
- [Summary](#summary)
- [Practical takeaway](#practical-takeaway)

---

## Error Tracking

If you want to understand **why your plugin behaves like chaos**, start here:

```text
./EXE/flhook_logs/flhook.log
```

This is the main FLHook log, and it contains:

- callback exceptions
- plugin runtime errors
- hook failures
- DLL issues

---

## What to look for in `flhook.log`

Check it if:

- commands don’t trigger
- hooks behave incorrectly
- plugin doesn’t load
- server crashes or acts weird
- callback throws exception

---

## Exceptions in callback functions

> [!IMPORTANT]
> If your callback throws an exception, it is usually logged in:

```text
flhook.log
```

---

## Recommendation

Don’t rely only on that.

Implement your own exception handling:

- better diagnostics
- more context
- prevent silent crashes

---

# Important: Runtime Library & STL

Here begins classic C++ pain.

---

## Problem

If you share STL objects between FLHook and your plugin, crashes may happen.

Example:

- FLHook creates `std::string`
- your plugin frees it
- 💥 crash

---

## Why it happens

Different runtime libraries = different allocators = incompatible memory

---

# What MUST be done

> [!CAUTION]
> If you use STL across boundaries,
> you MUST match FLHook runtime.

---

## Correct setting (Release)

```text
Multi-threaded DLL (/MD)
```

Path:

```text
Project Settings → Code Generation → Runtime Library
```

---

## Otherwise you get

- debug assertion crashes
- heap corruption
- random crashes
- memory issues
- nightmare debugging

---

# Debug Build

If FLHook is debug:

```text
Multi-threaded Debug DLL (/MDd)
```

---

# Golden Rule

## Release + Release

```text
/MD
```

## Debug + Debug

```text
/MDd
```

## Mixed

```text
Bad. Very bad.
```

---

# Performance Timer

FLHook has a built-in **performance timer**.

---

## What it does

Tracks:

- execution time per plugin
- slow hooks
- performance bottlenecks

---

## When useful

If plugin:

- runs frequently
- uses timers
- handles many players
- heavy logic
- file/db operations

---

# How timer logging works

If execution exceeds threshold → logged.

Config in:

```ini
flhook.ini
```

---

## Parameter

```ini
TimerThreshold=
```

---

## Meaning

If plugin time > threshold → flagged as performance issue.

---

# Practical usage

Helps find:

- heavy hooks
- bad loops
- unnecessary checks
- laggy commands
- inefficient timers
- multi-plugin bottlenecks

---

# What to do if server lags

Symptoms:

- freezes
- delayed response
- laggy commands
- instability

---

## Checklist

### 1. Check

```text
flhook.log
```

### 2. Check

```text
performance timer logs
```

### 3. Check

- blocking main thread
- heavy STL operations
- unnecessary loops
- file IO inside hooks

---

# Summary

- errors → `flhook.log`
- memory compatibility → same runtime
- performance → Performance Timer

---

# Practical takeaway

Your survival kit:

- try/catch
- matching runtime
- logging
- performance monitoring

Without it →
welcome to **C++ server hell** 😈

---
