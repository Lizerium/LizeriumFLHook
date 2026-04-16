# 💳 PLAYER CONTROLLER 💳

- [Назад](../../../../README.ru.md)

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Язык: </strong>
  
  <span style="color: #F5F752; margin: 0 10px;">
    ✅ 🇷🇺 Русский (текущий)
  </span>
  | 
  <a href="./README.md" style="color: #0891b2; margin: 0 10px;">
    🇺🇸 English
  </a>
</div>

---

## ⛄ Оглавление

- [💳 PLAYER CONTROLLER 💳](#-player-controller-)
  - [⛄ Оглавление](#-оглавление)
    - [🌁 Установка](#-установка)
    - [🌁 ADMIN COMMANDS](#-admin-commands)
      - [🌀 `smiteall`](#-smiteall)
      - [🌀 `bob`](#-bob)
      - [🌀 `playmusic`](#-playmusic)
      - [🌀 `playsound`](#-playsound)
      - [🌀 `playnnm`](#-playnnm)
      - [🌀 `beam`](#-beam)
      - [🌀 `pull`](#-pull)
      - [🌀 `move`](#-move)
      - [🌀 `chase`](#-chase)
      - [🌀 `lrs`](#-lrs)
      - [🌀 `makecoord`](#-makecoord)
      - [🌀 `authchar`](#-authchar)
      - [🌀 `reloadbans`](#-reloadbans)
      - [🌀 `setaccmovecode`](#-setaccmovecode)
      - [🌀 `rotatelogs`](#-rotatelogs)
      - [🌀 `privatemsg`](#-privatemsg)
      - [🌀 `showtags`](#-showtags)
      - [🌀 `addtag`](#-addtag)
      - [🌀 `droptag`](#-droptag)
      - [🌀 `reloadlockedships`](#-reloadlockedships)
    - [🌁 USER COMMANDS](#-user-commands)
      - [🌀 `help`](#-help)
      - [🌀 `commandlist`](#-commandlist)
      - [🌀 `pos`](#-pos)
      - [🌀 `stuck`](#-stuck)
      - [🌀 `droprep`](#-droprep)
      - [🌀 `dice`, `roll`, `coin`](#-dice-roll-coin)
      - [🌀 `pimpship`](#-pimpship)
      - [🌀 `showsetup`](#-showsetup)
      - [🌀 `showitems`](#-showitems)
      - [🌀 `setitem`](#-setitem)
      - [🌀 `renameme`](#-renameme)
      - [🌀 `movechar`](#-movechar)
      - [🌀 `set movecharcode`](#-set-movecharcode)
      - [🌀 `restart`](#-restart)
      - [🌀 `showrestarts`](#-showrestarts)
      - [🌀 `sendcash`](#-sendcash)
      - [🌀 `givecasht`](#-givecasht)
      - [🌀 `drawcash`](#-drawcash)
      - [🌀 `set cashcode`](#-set-cashcode)
      - [🌀 `showcash`](#-showcash)
      - [🌀 `group`](#-group)
      - [🌀 `local`](#-local)
      - [🌀 `system`](#-system)
      - [🌀 `invite`](#-invite)
      - [🌀 `factioninvite`](#-factioninvite)
      - [🌀 `setmsg`](#-setmsg)
      - [🌀 `showmsgs`](#-showmsgs)
      - [🌀 `ln`](#-ln)
      - [🌀 `sn`](#-sn)
      - [🌀 `gn`](#-gn)
      - [🌀 `tn`](#-tn)
      - [🌀 `target`](#-target)
      - [🌀 `reply`](#-reply)
      - [🌀 `privatemsg`](#-privatemsg-1)
      - [🌀 `factionmsg`](#-factionmsg)
      - [🌀 `set chattime`](#-set-chattime)
      - [🌀 `set dietime`](#-set-dietime)
      - [🌀 `mail`](#-mail)
      - [🌀 `mail`](#-mail-1)
      - [🌀 `showinfo`](#-showinfo)
      - [🌀 `setinfo`](#-setinfo)
      - [🌀 `time`](#-time)
      - [🌀 `lights`](#-lights)
      - [🌀 `selfdestruct`](#-selfdestruct)
      - [🌀 `shields`](#-shields)
      - [🌀 `survey`](#-survey)
      - [🌀 `showcoords`](#-showcoords)
      - [🌀 `savecoords`](#-savecoords)
      - [🌀 `cn`](#-cn)
      - [🌀 `setcoords`](#-setcoords)
      - [🌀 `jump`](#-jump)
      - [🌀 `beacon`](#-beacon)
      - [🌀 `jumpbeacon`](#-jumpbeacon)
      - [🌀 `charge`](#-charge)
      - [🌀 `jumpsys`](#-jumpsys)
      - [🌀 `showscan`](#-showscan)
      - [🌀 `maketag`](#-maketag)
      - [🌀 `droptag`](#-droptag-1)
      - [🌀 `settagpass`](#-settagpass)
      - [🌀 `changeship`](#-changeship)
      - [🌀 `delship`](#-delship)

> [!IMPORTANT]
>
> - Plugin loaded: playercntl **(playercntl.dll)**
>
>   Описание:

```ini
Устанавливает множество разных функций
```

1.  [Пример конфигурации `playercntl.ini`](../../../Binaries/bin-conf/flhook_plugins/playercntl.ini)
2.  [Пример конфигурации `jump_allowedsystems.cfg`](../../../Binaries/bin-conf/flhook_plugins/jump_allowedsystems.cfg)
3.  [Пример конфигурации `jump.cfg`](../../../Binaries/bin-conf/flhook_plugins/jump.cfg)
4.  [Пример конфигурации `alley_lockedships.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_lockedships.cfg)

### 🌁 Установка

1. Скопируйте файл:

```text
playercntl.dll
```

в папку:

```text
flhook_plugins\
```

2. Скопируйте файл:

```text
playercntl.ini
```

в папку:

```text
flhook_plugins\
```

3. В консоли FLHook выполните команду если изменили `playercntl.ini`:

```text
rehash
```

> чтобы перезагрузить конфигурацию.

---

### 🌁 ADMIN COMMANDS

#### 🌀 `smiteall`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
smiteall
```

> [!CAUTION]
> Применение:

```bash
smiteall
```

> [!IMPORTANT]
> Описание:

```ini
Поразите всех игроков в радиусе действия радаров 15k (взрывает всех)
```

#### 🌀 `bob`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
bob
```

> [!CAUTION]
> Применение:

```bash
bob <charname>
```

> [!IMPORTANT]
> Описание:

```ini
Игрок объявляется в розыск
```

#### 🌀 `playmusic`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
playmusic
```

> [!CAUTION]
> Применение:

```bash
playmusic <Musicname>
```

> [!IMPORTANT]
> Описание:

```ini
Проигрывает в радиусе 50к музыку игрокам
```

#### 🌀 `playsound`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
playsound
```

> [!CAUTION]
> Применение:

```bash
playsound <Soundname>
```

> [!IMPORTANT]
> Описание:

```ini
Проигрывает в радиусе 50к звук игрокам
```

#### 🌀 `playnnm`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
playnnm
```

> [!CAUTION]
> Применение:

```bash
playnnm <Soundname>
```

> [!IMPORTANT]
> Описание:

```ini
Проигрывает в радиусе 50к звук(?) игрокам
```

#### 🌀 `beam`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
beam
```

> [!CAUTION]
> Применение:

```bash
beam <charname> <basename>
```

> [!IMPORTANT]
> Описание:

```ini
Телепортирует admin на базу. Работает в разных системах, но требует доработки алгоритма выбора пути
```

#### 🌀 `pull`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
pull
```

> [!CAUTION]
> Применение:

```bash
pull <charname>
```

> [!IMPORTANT]
> Описание:

```ini
Притяните игрока к себе. Работает в разных системах, но требует доработки алгоритма выбора пути
```

#### 🌀 `move`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
move
```

> [!CAUTION]
> Применение:

```bash
move <x> <y> <z>
```

> [!IMPORTANT]
> Описание:

```ini
Переместиться на локацию
```

#### 🌀 `chase`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
chase
```

> [!CAUTION]
> Применение:

```bash
chase <charname>
```

> [!IMPORTANT]
> Описание:

```ini
Преследуйте игрока, телепорт к нему. Работает в разных системах, но требует доработки алгоритма выбора пути
```

#### 🌀 `lrs`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
lrs
```

> [!CAUTION]
> Применение:

```bash
lrs
```

> [!IMPORTANT]
> Описание:

```ini
Список кораблей, которым запрещено прыгать
```

#### 🌀 `makecoord`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
makecoord
```

> [!CAUTION]
> Применение:

```bash
makecoord
```

> [!IMPORTANT]
> Описание:

```ini
Отправить свои координаты
```

#### 🌀 `authchar`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
authchar
```

> [!CAUTION]
> Применение:

```bash
authchar <charname>
```

> [!IMPORTANT]
> Описание:

```ini
Проверяет есть ли персонаж в сети в статусе authenticated
```

#### 🌀 `reloadbans`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
reloadbans
```

> [!CAUTION]
> Применение:

```bash
reloadbans
```

> [!IMPORTANT]
> Описание:

```ini
Перезагружает списки забаненных по IP и по ID
```

#### 🌀 `setaccmovecode`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
setaccmovecode
```

> [!CAUTION]
> Применение:

```bash
setaccmovecode <charname> <code>
```

> [!IMPORTANT]
> Описание:

```ini
Установка кода перемещения для всех символов в учетной записи для персонажа
```

#### 🌀 `rotatelogs`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
rotatelogs
```

> [!CAUTION]
> Применение:

```bash
rotatelogs <charname> <code>
```

> [!IMPORTANT]
> Описание:

```ini
Бекапит логи сервера FLHook.log
```

#### 🌀 `privatemsg`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
pm
privatemsg
```

> [!CAUTION]
> Применение:

```bash
pm <charname> <message>
privatemsg <charname> <message>
```

> [!IMPORTANT]
> Описание:

```ini
Отправляет сообщение которое игрок получит после входа в систему
```

#### 🌀 `showtags`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
showtags
```

> [!CAUTION]
> Применение:

```bash
showtags
```

> [!IMPORTANT]
> Описание:

```ini
Показывает список тегов и их паролей с описанием и последним использованием
```

#### 🌀 `addtag`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
addtag
```

> [!CAUTION]
> Применение:

```bash
addtag <tag> <password> <description>
```

> [!IMPORTANT]
> Описание:

```ini
Добавляет тег, описание и его персональный пароль
```

#### 🌀 `droptag`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
droptag
```

> [!CAUTION]
> Применение:

```bash
droptag <tag>
```

> [!IMPORTANT]
> Описание:

```ini
Удаляет тег
```

#### 🌀 `reloadlockedships`

> [В оглавление](#оглавление)

> [!WARNING]
> Команда:

```bash
reloadlockedships
```

> [!CAUTION]
> Применение:

```bash
reloadlockedships
```

> [!IMPORTANT]
> Описание:

```ini
Показывает список заблокированных кораблей через конфиг alley_lockedships.cfg
```

> ---

### 🌁 USER COMMANDS

#### 🌀 `help`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_CustomHelp
```

> [!WARNING]
> Команда:

```bash
/help
/h
/?
```

> [!CAUTION]
> Применение:

```bash
/help
/h
/?
```

> [!IMPORTANT]
> Описание:

```ini
Печать пользовательской справки с переопределением встроенной справки
```

#### 🌀 `commandlist`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_CommandList
```

> [!WARNING]
> Команда:

```bash
/commandlist
```

> [!CAUTION]
> Применение:

```bash
/commandlist
```

> [!IMPORTANT]
> Описание:

```ini
Печать пользовательской справки о командах
```

#### 🌀 `pos`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Pos
```

> [!WARNING]
> Команда:

```bash
/pos
```

> [!CAUTION]
> Применение:

```bash
/pos
```

> [!IMPORTANT]
> Описание:

```ini
Печать текущей позиции
```

#### 🌀 `stuck`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Stuck
```

> [!WARNING]
> Команда:

```bash
/stuck
```

> [!CAUTION]
> Применение:

```bash
/stuck
```

> [!IMPORTANT]
> Описание:

```ini
Немного переместите корабль, если он застрял в базе
```

#### 🌀 `droprep`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_DropRep
```

> [!WARNING]
> Команда:

```bash
/droprep
```

> [!CAUTION]
> Применение:

```bash
/droprep
```

> [!IMPORTANT]
> Описание:

```ini
Команда, помогающая удалить любую принадлежность к фракции, которая может у вас есть
```

#### 🌀 `dice`, `roll`, `coin`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Dice
UserCmd_Coin
```

> [!WARNING]
> Команда:

```bash
/dice
/roll

/coin
```

> [!CAUTION]
> Применение:

```bash
/dice 1d20 | 1d20+3 | etc.
/roll 1d20 | 1d20+3 | etc.

/coin 1d20 | 1d20+3 | etc.
```

> [!IMPORTANT]
> Описание:

```ini
Бросание костей и разыгрывание рандомных комбинаций для жеребьёвок
```

#### 🌀 `pimpship`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_PimpShip
```

> [!WARNING]
> Команда:

```bash
/pimpship
```

> [!CAUTION]
> Применение:

```bash
/pimpship
```

> [!IMPORTANT]
> Описание:

```ini
Команда(ы) для настройки освещения корабля (на станции у диллера на эту тему)
```

#### 🌀 `showsetup`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ShowSetup
```

> [!WARNING]
> Команда:

```bash
/showsetup
```

> [!CAUTION]
> Применение:

```bash
/showsetup
```

> [!IMPORTANT]
> Описание:

```ini
Показать настройку корабля
```

#### 🌀 `showitems`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ShowSetup
```

> [!WARNING]
> Команда:

```bash
/showitems
```

> [!CAUTION]
> Применение:

```bash
/showitems
```

> [!IMPORTANT]
> Описание:

```ini
Отображение элементов, которые могут быть изменены.
```

#### 🌀 `setitem`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ChangeItem
```

> [!WARNING]
> Команда:

```bash
/setitem
```

> [!CAUTION]
> Применение:

```bash
/setitem
```

> [!IMPORTANT]
> Описание:

```ini
Измените элемент (маяк освещения) в идентификаторе слота на указанный элемент.
```

#### 🌀 `renameme`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_RenameMe
```

> [!WARNING]
> Команда:

```bash
/renameme
```

> [!CAUTION]
> Применение:

```bash
/renameme <charname> <password>
```

> [!IMPORTANT]
> Описание:

```ini
Изменяет имя на персонаже, на любой базе и за деньги, пароль нужен за смену имени на специальный тег в нём, чтобы стать частью фракции игроков
```

#### 🌀 `movechar`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_MoveChar
```

> [!WARNING]
> Команда:

```bash
/movechar
```

> [!CAUTION]
> Применение:

```bash
/movechar <charname> <code>
```

> [!IMPORTANT]
> Описание:

```ini
Переместите персонажа из удаленной учетной записи в эту.
```

#### 🌀 `set movecharcode`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SetMoveCharCode
```

> [!WARNING]
> Команда:

```bash
/set movecharcode
```

> [!CAUTION]
> Применение:

```bash
/set movecharcode <code>
```

> [!IMPORTANT]
> Описание:

```ini
Если <code> равен none то код удаляется и перемещение возможно без кода, в ином случае ставится код перемещения персонажа
```

#### 🌀 `restart`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Restart
```

> [!WARNING]
> Команда:

```bash
/restart
```

> [!CAUTION]
> Применение:

```bash
/restart <faction>
```

> [!IMPORTANT]
> Описание:

```ini
Перезапускает игру за другую фракцию
```

#### 🌀 `showrestarts`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ShowRestarts
```

> [!WARNING]
> Команда:

```bash
/showrestarts
```

> [!CAUTION]
> Применение:

```bash
/showrestarts
/showrestarts <private_code>
```

> [!IMPORTANT]
> Описание:

```ini
Показывает список доступных рестартов персонажа
```

#### 🌀 `sendcash`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_GiveCash
```

> [!WARNING]
> Команда:

```bash
/sendcash
/givecash
/gc
```

> [!CAUTION]
> Применение:

```bash
/sendcash <charname> <cash> <anon> <comment>
/givecash <charname> <cash> <anon> <comment>
/gc <charname> <cash> <anon> <comment>
```

> [!IMPORTANT]
> Описание:

```ini
Дарит персонажу ваши деньги в количестве <cash>, можно анонимно введя /givecash Admin 1000000000 anon Подарок
```

#### 🌀 `givecasht`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_GiveCashTarget
```

> [!WARNING]
> Команда:

```bash
/givecasht
/gct
```

> [!CAUTION]
> Применение:

```bash
/givecasht <cash> <anon> <comment>
/gct <cash> <anon> <comment>
```

> [!IMPORTANT]
> Описание:

```ini
Дарит выбранному мышкой персонажу ваши деньги в количестве <cash>, можно анонимно введя /givecash Admin 1000000000 anon Подарок
```

#### 🌀 `drawcash`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_DrawCash
```

> [!WARNING]
> Команда:

```bash
/drawcash
/drc
```

> [!CAUTION]
> Применение:

```bash
/drawcash <charname> <code> <cash>
/drc <charname> <code> <cash>
```

> [!IMPORTANT]
> Описание:

```ini
Дарит персонажу ваши деньги в количестве <cash>, можно анонимно введя /givecash Admin 123(если код есть) 1000000000
```

#### 🌀 `set cashcode`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SetCashCode
```

> [!WARNING]
> Команда:

```bash
/set cashcode
```

> [!CAUTION]
> Применение:

```bash
/set cashcode <code>
```

> [!IMPORTANT]
> Описание:

```ini
Устанавливает код для вашего кошелька, то есть игрок используя ваш код может смотреть ваш баланс командой /showcash <charname> <code>
```

#### 🌀 `showcash`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ShowCash
```

> [!WARNING]
> Команда:

```bash
/showcash
/shc
```

> [!CAUTION]
> Применение:

```bash
/showcash <charname> <code>
/shc <charname> <code>
```

> [!IMPORTANT]
> Описание:

```ini
Игрок смотрит ваш баланс командой /showcash используя код который вы ему предоставили и установили на кошелёк
```

#### 🌀 `group`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_GroupMsg
```

> [!WARNING]
> Команда:

```bash
/group
/g
```

> [!CAUTION]
> Применение:

```bash
/group <message>
/g <message>
```

> [!IMPORTANT]
> Описание:

```ini
Отправьте сообщение в групповой чат
```

#### 🌀 `local`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_LocalMsg
```

> [!WARNING]
> Команда:

```bash
/local
/l
```

> [!CAUTION]
> Применение:

```bash
/local <message>
/l <message>
```

> [!IMPORTANT]
> Описание:

```ini
Отправьте сообщение в локальный системный чат.
```

#### 🌀 `system`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SystemMsg
```

> [!WARNING]
> Команда:

```bash
/system
/s
```

> [!CAUTION]
> Применение:

```bash
/system <message>
/s <message>
```

> [!IMPORTANT]
> Описание:

```ini
Отправить сообщение в системный чат.
```

#### 🌀 `invite`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Invite
```

> [!WARNING]
> Команда:

```bash
/invite
/i
```

> [!CAUTION]
> Применение:

```bash
/invite <charname>
/i <charname>
```

> [!IMPORTANT]
> Описание:

```ini
Приглашает пользователя в группу
```

#### 🌀 `factioninvite`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_FactionMsg
```

> [!WARNING]
> Команда:

```bash
/factioninvite
/fi
```

> [!CAUTION]
> Применение:

```bash
/factioninvite <tag>
/fm <tag>
```

> [!IMPORTANT]
> Описание:

```ini
Отправьте сообщение с приглашением фракции всем игрокам с определенным префиксом.
```

#### 🌀 `setmsg`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Invite
```

> [!WARNING]
> Команда:

```bash
/setmsg
```

> [!CAUTION]
> Применение:

```bash
/setmsg <n> <msg_text>
```

> [!IMPORTANT]
> Описание:

```ini
Установка предустановленного сообщения, вместо n устанавливается его порядковый номер вместо text текст сообщения
/setmsg 1 Всем привет и приятного дня!
```

#### 🌀 `showmsgs`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ShowMsgs
```

> [!WARNING]
> Команда:

```bash
/showmsgs
```

> [!CAUTION]
> Применение:

```bash
/showmsgs
```

> [!IMPORTANT]
> Описание:

```ini
Показать предустановленные сообщения
```

#### 🌀 `ln`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_DRMsg
```

> [!WARNING]
> Команда:

```bash
/n
/ln
```

> [!CAUTION]
> Применение:

```bash
/1-9
/l1-9
```

> [!IMPORTANT]
> Описание:

```ini
Отправьте предустановленное сообщение в локальный системный чат от 1 до 9 включительно
```

#### 🌀 `sn`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SMsg
```

> [!WARNING]
> Команда:

```bash
/sn
```

> [!CAUTION]
> Применение:

```bash
/s1-9
```

> [!IMPORTANT]
> Описание:

```ini
Отправка предустановленного сообщения в системный чат от 1 до 9 включительно
```

#### 🌀 `gn`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_GMsg
```

> [!WARNING]
> Команда:

```bash
/gn
```

> [!CAUTION]
> Применение:

```bash
/g1-9
```

> [!IMPORTANT]
> Описание:

```ini
Отправка предустановленного сообщения в групповой чат от 1 до 9 включительно
```

#### 🌀 `tn`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SendToLastTarget
```

> [!WARNING]
> Команда:

```bash
/tn
```

> [!CAUTION]
> Применение:

```bash
/t1-9
```

> [!IMPORTANT]
> Описание:

```ini
Отправка сообщения последнему/текущему целевому объекту.
```

#### 🌀 `target`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SendToLastTarget
```

> [!WARNING]
> Команда:

```bash
/target
/t
```

> [!CAUTION]
> Применение:

```bash
/target <message>
/t <message>
```

> [!IMPORTANT]
> Описание:

```ini
Отправка сообщения последнему/текущему целевому объекту.
```

#### 🌀 `reply`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ReplyToLastPMSender
```

> [!WARNING]
> Команда:

```bash
/reply
/r
```

> [!CAUTION]
> Применение:

```bash
/reply <message>
/r <message>
```

> [!IMPORTANT]
> Описание:

```ini
Отправить сообщение последнему человеку, который отправил сообщение вам.
```

#### 🌀 `privatemsg`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_PrivateMsgID
```

> [!WARNING]
> Команда:

```bash
/privatemsg
/pm
/privatemsg$
/pm$
```

> [!CAUTION]
> Применение:

```bash
/privatemsg <charname> <messsage>
/pm <charname> <messsage>
/privatemsg$ <clientid> <messsage>
/pm$ <clientid> <messsage>
```

> [!IMPORTANT]
> Описание:

```ini
Отправить личное сообщение на указанный clientid и charname
```

#### 🌀 `factionmsg`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_FactionMsg
```

> [!WARNING]
> Команда:

```bash
/factionmsg
/fm
```

> [!CAUTION]
> Применение:

```bash
/factionmsg <tag> <message>
/fm <tag> <message>
```

> [!IMPORTANT]
> Описание:

```ini
Отправьте сообщение всем игрокам с определенным префиксом.
```

#### 🌀 `set chattime`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SetChatTime
```

> [!WARNING]
> Команда:

```bash
/set chattime
```

> [!CAUTION]
> Применение:

```bash
/set chattime <on|off>
```

> [!IMPORTANT]
> Описание:

```ini
Добавить вывод времени сообщения отправленного в чате и позволяет это выключить у себя каждому игроку
```

#### 🌀 `set dietime`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SetDeathTime
```

> [!WARNING]
> Команда:

```bash
/set dietime
```

> [!CAUTION]
> Применение:

```bash
/set dietime <on|off>
```

> [!IMPORTANT]
> Описание:

```ini
Добавить вывод времени сообщения о смерти и позволяет это выключить у себя каждому игроку
```

#### 🌀 `mail`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_MailShow
```

> [!WARNING]
> Команда:

```bash
/mail
```

> [!CAUTION]
> Применение:

```bash
/mail <msgnum>
```

> [!IMPORTANT]
> Описание:

```ini
Показать почту
```

#### 🌀 `mail`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_MailDel
```

> [!WARNING]
> Команда:

```bash
/maildel
```

> [!CAUTION]
> Применение:

```bash
/maildel <msgnum>
```

> [!IMPORTANT]
> Описание:

```ini
Удалить почту
```

#### 🌀 `showinfo`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ShowInfo
```

> [!WARNING]
> Команда:

```bash
/si
/showinfo
/showinfo*
```

> [!CAUTION]
> Применение:

```bash
/si me
/si <target>
/showinfo
/showinfo*
/showinfo <target>
/showinfo* <target>
```

> [!IMPORTANT]
> Описание:

```ini
Показать личную карточку игрока которую он сформировал командой /setinfo
```

#### 🌀 `setinfo`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SetInfo
```

> [!WARNING]
> Команда:

```bash
/setinfo
```

> [!CAUTION]
> Применение:

```bash
/setinfo <n> <command> <msg>
```

> [!IMPORTANT]
> Описание:

```ini
<n> от 1 до 5 включительно
<command> a или d - a:устанавливает информацию, d:удаляет информацию из указанного номера параграфа <n>
<msg> только при command=a - устанавливает текст параграфа вашей карточки
Установить данные личной карточки игрока (выключено)
```

#### 🌀 `time`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Time
```

> [!WARNING]
> Команда:

```bash
/time
/time*
```

> [!CAUTION]
> Применение:

```bash
/time
/time*
```

> [!IMPORTANT]
> Описание:

```ini
Напечатать время
```

#### 🌀 `lights`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Lights
```

> [!WARNING]
> Команда:

```bash
/lights
/lights*
```

> [!CAUTION]
> Применение:

```bash
/lights
/lights*
```

> [!IMPORTANT]
> Описание:

```ini
Включить маяки корабля
```

#### 🌀 `selfdestruct`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SelfDestruct
```

> [!WARNING]
> Команда:

```bash
/selfdestruct
/selfdestruct*
```

> [!CAUTION]
> Применение:

```bash
/selfdestruct
/selfdestruct*
```

> [!IMPORTANT]
> Описание:

```ini
Самоуничтожение корабля
```

#### 🌀 `shields`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Shields
```

> [!WARNING]
> Команда:

```bash
/shields
/shields*
```

> [!CAUTION]
> Применение:

```bash
/shields
/shields*
```

> [!IMPORTANT]
> Описание:

```ini
Включение выключение щита корабля
```

#### 🌀 `survey`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_Survey
```

> [!WARNING]
> Команда:

```bash
/survey
```

> [!CAUTION]
> Применение:

```bash
/survey
```

> [!IMPORTANT]
> Описание:

```ini
Что-то иссследует, похоже на солнце которое спаунит сервак по заданию (выключено)
```

#### 🌀 `showcoords`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ShowCoords
```

> [!WARNING]
> Команда:

```bash
/showcoords
```

> [!CAUTION]
> Применение:

```bash
/showcoords <n>
```

> [!IMPORTANT]
> Описание:

```ini
Показать предустановленные координаты (1-9)
```

#### 🌀 `savecoords`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SaveCoords
```

> [!WARNING]
> Команда:

```bash
/savecoords
```

> [!CAUTION]
> Применение:

```bash
/savecoords <n> <text>
```

> [!IMPORTANT]
> Описание:

```ini
Установка предустановленной координаты (1-9)
```

#### 🌀 `cn`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_LoadCoords
```

> [!WARNING]
> Команда:

```bash
/cn
```

> [!CAUTION]
> Применение:

```bash
/c1-9
```

> [!IMPORTANT]
> Описание:

```ini
Загрузка предустановленной координаты от 1 до 9 включительно
```

#### 🌀 `setcoords`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SetCoords
```

> [!WARNING]
> Команда:

```bash
/setcoords
```

> [!CAUTION]
> Применение:

```bash
/setcoords
```

> [!IMPORTANT]
> Описание:

```ini
Установить прыжковому двигателю координаты (выключено)
```

#### 🌀 `jump`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ActivateJumpDrive
```

> [!WARNING]
> Команда:

```bash
/jump
/jump*
```

> [!CAUTION]
> Применение:

```bash
/jump
/jump*
```

> [!IMPORTANT]
> Описание:

```ini
Прыгнуть прыжковым двигателем (выключено)
```

#### 🌀 `beacon`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_DeployBeacon
```

> [!WARNING]
> Команда:

```bash
/beacon
/beacon*
```

> [!CAUTION]
> Применение:

```bash
/beacon
/beacon*
```

> [!IMPORTANT]
> Описание:

```ini
Команды гиперпространственного маяка (выключено)
```

#### 🌀 `jumpbeacon`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_JumpBeacon
```

> [!WARNING]
> Команда:

```bash
/jumpbeacon
/jumpbeacon*
```

> [!CAUTION]
> Применение:

```bash
/jumpbeacon
/jumpbeacon*
```

> [!IMPORTANT]
> Описание:

```ini
Команды гиперпространственного маяка (выключено)
```

#### 🌀 `charge`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ChargeJumpDrive
```

> [!WARNING]
> Команда:

```bash
/charge
/charge*
```

> [!CAUTION]
> Применение:

```bash
/charge
/charge*
```

> [!IMPORTANT]
> Описание:

```ini
Команды прыжкового двигателя (выключено)
```

#### 🌀 `jumpsys`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ListJumpableSystems
```

> [!WARNING]
> Команда:

```bash
/jumpsys
```

> [!CAUTION]
> Применение:

```bash
/jumpsys
```

> [!IMPORTANT]
> Описание:

```ini
Анализ систем куда можно прыгнуть (выключено)
```

#### 🌀 `showscan`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ShowScan
```

> [!WARNING]
> Команда:

```bash
/showscan
/showscan$
/scan
/scanid
```

> [!CAUTION]
> Применение:

```bash
/showscan
/showscan$
/scan <charname>
/scanid <clientid>
```

> [!IMPORTANT]
> Описание:

```ini
Анализ систем куда можно прыгнуть (выключено)
```

#### 🌀 `maketag`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_MakeTag
```

> [!WARNING]
> Команда:

```bash
/maketag
```

> [!CAUTION]
> Применение:

```bash
/maketag <tag> <master password> <description>
```

> [!IMPORTANT]
> Описание:

```ini
Создание тега фракции со своим паролем и описанием
```

#### 🌀 `droptag`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_DropTag
```

> [!WARNING]
> Команда:

```bash
/droptag
```

> [!CAUTION]
> Применение:

```bash
/droptag <tag> <master_password>
```

> [!IMPORTANT]
> Описание:

```ini
Удаление тега фракции
```

#### 🌀 `settagpass`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_SetTagPass
```

> [!WARNING]
> Команда:

```bash
/settagpass
```

> [!CAUTION]
> Применение:

```bash
/settagpass <tag> <master_password> <rename_password>
```

> [!IMPORTANT]
> Описание:

```ini
Смена пароля тега фракции
```

#### 🌀 `changeship`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_ChangeShip
```

> [!WARNING]
> Команда:

```bash
/changeship
/cs
```

> [!CAUTION]
> Применение:

```bash
/changeship [id]
/cs [id]
```

> [!IMPORTANT]
> Описание:

```ini
Сохраняет корабль в импровизированный ангар игрока на котором вы находитесь в текущий момент.
Позволяет изменить корабль на другой, работает смена только в космосе. На сервере существует ограничение в количество кораблей которые вы можете сохранять в свой ангар, будьте внимательны! Вы можете удалить корабль из ангара введя команду /ds (или /delship)
```

#### 🌀 `delship`

> [В оглавление](#оглавление)

> ---
>
> [!NOTE]
> Функция:

```bash
UserCmd_DeleteShip
```

> [!WARNING]
> Команда:

```bash
/delship
/ds
```

> [!CAUTION]
> Применение:

```bash
/delship [id]
/ds [id]
```

> [!IMPORTANT]
> Описание:

```ini
Удаляет сохранённый корабль из списка тех, что появлялись по мере покупки (относится к импровизированному ангару игрока)
```

> ---
