# 🚀 FLHook (v1.6.2+)

Плагин для сервера Freelancer, расширяющий его возможности: админка, команды, плагины, события, логирование и многое другое.

---

# 📦 Установка

⚠️ **Важно:** работает только с **FLServer 1.1**
С версией 1.0 сервер просто упадёт.

### Шаги:

1. Скопируй файлы из папки `bin` в:

   ```
   Freelancer/EXE/
   ```

2. Открой файл:

   ```
   Freelancer/EXE/dacomsrv.ini
   ```

3. Добавь в секцию `[Libraries]`:

   ```
   FLHook.dll
   ```

4. Настрой:

   ```
   FLHook.ini
   ```

5. Запусти сервер 🚀

---

# 🔌 Установка плагинов

Структура простая:

```
Freelancer/EXE/flhook_plugins/
```

- `.dll` плагина → сюда
- остальные файлы → как в архиве плагина

📌 Всегда читай README плагина — у них могут быть свои настройки

---

# ⚙️ Конфигурация

Основные настройки находятся в:

```
FLHook.ini
```

Там есть комментарии — просто пройди и настрой под себя.

---

# 🛠️ Админ-команды

## 📌 Где выполнять

- Консоль FLHook
- В игре: `.command`
- Через socket (например, PuTTY)

---

## 💰 Деньги

```
getcash <char>
setcash <char> <amount>
addcash <char> <amount>
```

---

## 🔨 Кик / Бан

```
kick <char> <reason>
ban <char>
unban <char>
kickban <char> <reason>
```

---

## 💬 Сообщения

```
msg <char> <text>
msgu <text>        # всем
msgs <system> <text>
```

---

## ☠️ Управление игроком

```
beam <char> <base>
kill <char>
```

---

## 🎭 Репутация

```
setrep <char> <group> <value>
resetrep <char>
```

---

## 📦 Груз

```
enumcargo <char>
addcargo <char> <item> <count>
removecargo <char> <id> <count>
```

---

## 👤 Персонажи

```
rename <old> <new>
deletechar <char>
```

---

## ⚙️ Настройки

```
setadmin <char> <rights>
getadmin <char>
rehash
```

---

## 🔌 Плагины

```
loadplugins
loadplugin <file>
unloadplugin <name>
listplugins
```

---

# 🔐 Права (Admin Rights)

Примеры:

```
superadmin
cash
kickban
msg
plugins
```

Можно комбинировать:

```
setadmin player cash,kickban,msg
```

---

# 🌐 Socket API

Подключение по TCP (порт в `FLHook.ini`):

```
PASS password
```

Дальше можно выполнять команды.

📡 Можно включить:

```
eventmode
```

И получать события:

- вход игрока
- выход
- убийства
- чат
- переходы между системами

---

# 📡 События (eventmode)

Примеры:

```
login char=Player id=1 ip=127.0.0.1
kill victim=Player type=player by=Enemy
chat from=Player text=hello
```

---

# 🎨 XML сообщения

Форматированные сообщения:

```xml
<TRA data="0xFF000003"/><TEXT>Hello</TEXT>
```

Поддержка:

- цвета 🎨
- жирный
- курсив
- размер

---

# 👤 Пользовательские команды

В игре:

```
/set diemsg all
/set chatfont big bold
/ignore player
/autobuy
```

---

# 📊 Логи

Папка:

```
Freelancer/EXE/flhook_logs/
```

Основные:

- `flhook.log` — debug
- `flhook_kicks.log`
- `flhook_cheaters.log`
- `flhook_connects.log`
- `flhook_admincmds.log`

---

# ⚠️ Известные проблемы

- Иногда баги с отображением чата при кастомном шрифте

---

# 💻 Разработка

- Требуется **MSVC VC10**
- Используется `FLCoreSDK`
- Для строк используется `FLHookWString.dll`

---

# 🔐 Шифрование (Blowfish)

- Блок: 8 байт
- Нужно дополнять `0x00`
- При дешифровке — удалять null-байты

---

# 🧩 Особенности

✔ Плагинная архитектура
✔ Socket API
✔ Античит
✔ Полный контроль сервера
✔ Логирование
✔ Event-система

---

# 📚 Ресурсы

- Форум: [http://www.the-startport.net](http://www.the-startport.net)

---

# ❤️ Авторы

- mc_horst
- w0dk4
- open-source сообщество

---
