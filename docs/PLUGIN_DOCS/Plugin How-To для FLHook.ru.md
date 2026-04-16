# Plugin How-To для FLHook

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Язык: </strong>
  
  <span style="color: #F5F752; margin: 0 10px;">
    ✅ 🇷🇺 Русский (текущий)
  </span>
  | 
  <a href="./Plugin%20How-To%20для%20FLHook.md" style="color: #0891b2; margin: 0 10px;">
    🇺🇸 English
  </a>
</div>

---

- [Plugin How-To для FLHook](#plugin-how-to-для-flhook)
	- [Основы](#основы)
	- [Пример: хук `HkIServerImpl::OnConnect`](#пример-хук-hkiserverimplonconnect)
	- [Пример реализации](#пример-реализации)
- [Управление поведением FLHook после вызова плагина](#управление-поведением-flhook-после-вызова-плагина)
- [Пример использования `Get_PluginReturnCode`](#пример-использования-get_pluginreturncode)
- [Что такое `PLUGIN_RETURNCODE`](#что-такое-plugin_returncode)
	- [Пример логики](#пример-логики)
- [Как FLHook это обрабатывает](#как-flhook-это-обрабатывает)
- [Почему важен приоритет плагина](#почему-важен-приоритет-плагина)
- [Важные замечания по `Get_PluginReturnCode`](#важные-замечания-по-get_pluginreturncode)
	- [Если ты используешь `Get_PluginReturnCode`](#если-ты-используешь-get_pluginreturncode)
- [`g_bPlugin_nofunctioncall`](#g_bplugin_nofunctioncall)
	- [Что он означает](#что-он-означает)
	- [Зачем это нужно](#зачем-это-нужно)
- [Практический смысл](#практический-смысл)
	- [1. Просто слушать событие](#1-просто-слушать-событие)
	- [2. Вмешиваться в выполнение](#2-вмешиваться-в-выполнение)
	- [3. Понимать, что сделали другие плагины](#3-понимать-что-сделали-другие-плагины)
- [Итог](#итог)
- [Минимальный шаблон](#минимальный-шаблон)

## Основы

Вот здесь уже начинается самое интересное — или самое болезненное, если что-то пойдёт не так.

Суть простая:

Если ты хочешь, чтобы FLHook вызывал твою функцию при определённом событии, тебе нужно:

1. Подписаться на нужный **hook**
2. Экспортировать функцию **с точно таким же прототипом**

---

## Пример: хук `HkIServerImpl::OnConnect`

Допустим, ты подключаешься к хуку:

```text
HkIServerImpl::OnConnect
```

В документации для него указан следующий прототип:

```cpp
void __stdcall HkIServerImpl::OnConnect(unsigned int)
```

Это значит, что твоя функция должна выглядеть **точно так же по сигнатуре**.

---

## Пример реализации

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

И всё.
После этого FLHook сможет вызвать твою функцию при подключении клиента.

---

# Управление поведением FLHook после вызова плагина

Если ты хочешь не просто "послушать" событие, а **повлиять на дальнейшее выполнение**, всё становится чуть интереснее.

Например, ты можешь захотеть:

- не вызывать оригинальную функцию FLHook / FLServer
- не давать другим плагинам обработать это событие
- полностью перехватить выполнение на себя

Для этого используется специальная экспортируемая функция:

```cpp
Get_PluginReturnCode
```

---

# Пример использования `Get_PluginReturnCode`

```cpp
PLUGIN_RETURNCODE returncode;

__declspec(dllexport) PLUGIN_RETURNCODE Get_PluginReturnCode()
{
	return returncode;
}

__declspec(dllexport) int __stdcall A_Hooked_Function(unsigned int iSomething)
{
	// do something here

	// теперь мы не хотим, чтобы FLHook вызывал оригинальную функцию
	// и не хотим, чтобы вызывались другие плагины
	returncode = SKIPPLUGINS_NOFUNCTIONCALL;

	return 0;
}
```

---

# Что такое `PLUGIN_RETURNCODE`

`PLUGIN_RETURNCODE` — это `enum`, который определён в файле:

```text
sdk/headers/plugin.h
```

Он определяет, **что должен сделать FLHook после выполнения твоего хука**.

---

## Пример логики

Если ты устанавливаешь:

```cpp
returncode = SKIPPLUGINS_NOFUNCTIONCALL;
```

то это означает:

- **не вызывать другие плагины**
- **не вызывать оригинальную функцию**
- вернуть текущее значение, которое вернул твой хук

Если функция не `void`, FLHook вернёт именно то, что ты вернул в своей функции.

В примере выше это:

```cpp
return 0;
```

---

# Как FLHook это обрабатывает

Внутри FLHook логика примерно такая:

1. Вызывается твоя hooked-функция
2. Сразу после этого FLHook вызывает:

```cpp
Get_PluginReturnCode()
```

3. И уже по этому значению решает:
   - вызывать ли другие плагины
   - вызывать ли оригинальную серверную функцию
   - или завершить обработку прямо сейчас

---

# Почему важен приоритет плагина

Теперь становится понятно, зачем нужен **priority** в `Get_PluginInfo`.

Если у тебя:

- античит с высоким приоритетом
- логгер с низким приоритетом

то античит может первым сказать:

```cpp
SKIPPLUGINS_NOFUNCTIONCALL
```

и тогда логгер уже **вообще не получит вызов**.

---

# Важные замечания по `Get_PluginReturnCode`

> [!IMPORTANT]
> Если твоя DLL **не экспортирует** `Get_PluginReturnCode`,
> FLHook всегда считает, что нужно работать по стандартной схеме:

- вызвать все плагины
- вызвать оригинальную функцию

---

## Если ты используешь `Get_PluginReturnCode`

Тогда обязательно:

> [!CAUTION]
> **Всегда выставляй `returncode` внутри каждого hooked-хука**

Иначе можно получить непредсказуемое поведение.

---

# `g_bPlugin_nofunctioncall`

В `plugin.h` также есть глобальный импорт:

```cpp
extern __declspec(dllimport) bool g_bPlugin_nofunctioncall;
```

---

## Что он означает

Если:

```cpp
g_bPlugin_nofunctioncall == true
```

это означает:

> оригинальная функция **не должна вызываться**

Причина может быть одна из двух:

1. её уже вызвал предыдущий плагин
2. предыдущий плагин специально запретил её вызов

---

## Зачем это нужно

Это сделано ради **совместимости между плагинами**.

Особенно полезно для хуков, которые должны выполняться **после определённых серверных методов**, например:

```text
IServerImpl::PlayerLaunch
```

Если один плагин уже изменил ход выполнения, другой плагин может это увидеть через `g_bPlugin_nofunctioncall` и корректно отреагировать.

---

# Практический смысл

То есть у тебя есть три уровня контроля:

---

## 1. Просто слушать событие

```cpp
void __stdcall OnConnect(...)
```

---

## 2. Вмешиваться в выполнение

Через:

```cpp
Get_PluginReturnCode()
```

---

## 3. Понимать, что сделали другие плагины

Через:

```cpp
g_bPlugin_nofunctioncall
```

---

# Итог

Если коротко:

- хочешь обработать событие → экспортируй правильную функцию
- хочешь влиять на цепочку вызова → используй `Get_PluginReturnCode`
- хочешь писать совместимые плагины → учитывай `g_bPlugin_nofunctioncall`

---

# Минимальный шаблон

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
