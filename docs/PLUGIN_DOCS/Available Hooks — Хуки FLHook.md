# Available Hooks — Хуки FLHook

- [Available Hooks — Хуки FLHook](#available-hooks--хуки-flhook)
  - [Введение](#введение)
- [Базовые системные функции плагина](#базовые-системные-функции-плагина)
  - [`Get_PluginReturnCode`](#get_pluginreturncode)
    - [Назначение](#назначение)
    - [Важно](#важно)
  - [`Get_PluginInfo`](#get_plugininfo)
    - [Назначение](#назначение-1)
    - [Важно](#важно-1)
  - [`Plugin_Communication_CallBack`](#plugin_communication_callback)
    - [Назначение](#назначение-2)
- [FLServer Hooks](#flserver-hooks)
- [1. Жизненный цикл сервера](#1-жизненный-цикл-сервера)
  - [Shutdown](#shutdown)
  - [Startup](#startup)
    - [AFTER-вариант](#after-вариант)
  - [Update](#update)
    - [Использование](#использование)
- [2. Игрок: подключение, логин, выбор персонажа](#2-игрок-подключение-логин-выбор-персонажа)
  - [OnConnect](#onconnect)
  - [Login](#login)
    - [Особенность](#особенность)
    - [Важно](#важно-2)
    - [AFTER-вариант](#after-вариант-1)
  - [CharacterSelect](#characterselect)
  - [CreateNewCharacter](#createnewcharacter)
  - [DestroyCharacter](#destroycharacter)
  - [DisConnect](#disconnect)
- [3. Игрок: запуск, док, переходы, системы](#3-игрок-запуск-док-переходы-системы)
  - [PlayerLaunch](#playerlaunch)
  - [LaunchComplete](#launchcomplete)
  - [BaseEnter / BaseExit](#baseenter--baseexit)
  - [JumpInComplete](#jumpincomplete)
  - [SystemSwitchOutComplete](#systemswitchoutcomplete)
  - [LaunchPosHook](#launchposhook)
    - [Назначение](#назначение-3)
    - [Использование](#использование-1)
- [4. Чат и пользовательские команды](#4-чат-и-пользовательские-команды)
  - [SubmitChat](#submitchat)
    - [Использование](#использование-2)
  - [HkCb\_SendChat](#hkcb_sendchat)
  - [UserCmd\_Process](#usercmd_process)
    - [Назначение](#назначение-4)
    - [Важно](#важно-3)
  - [UserCmd\_Help](#usercmd_help)
- [5. Админ-команды и консоль](#5-админ-команды-и-консоль)
  - [ExecuteCommandString\_Callback](#executecommandstring_callback)
    - [Назначение](#назначение-5)
    - [Важно](#важно-4)
  - [CmdHelp\_Callback](#cmdhelp_callback)
- [6. Бой, урон, оружие, уничтожение](#6-бой-урон-оружие-уничтожение)
  - [FireWeapon](#fireweapon)
  - [ShipDestroyed](#shipdestroyed)
    - [Использование](#использование-3)
  - [AllowPlayerDamage](#allowplayerdamage)
    - [Назначение](#назначение-6)
  - [HkCb\_AddDmgEntry](#hkcb_adddmgentry)
  - [HkCB\_MissileTorpHit](#hkcb_missiletorphit)
  - [HkCb\_GeneralDmg](#hkcb_generaldmg)
- [7. Торговля, деньги, предметы, рынок](#7-торговля-деньги-предметы-рынок)
  - [ReqChangeCash / ReqSetCash](#reqchangecash--reqsetcash)
  - [GFGoodBuy / GFGoodSell](#gfgoodbuy--gfgoodsell)
  - [ReqAddItem / ReqModifyItem / ReqRemoveItem](#reqadditem--reqmodifyitem--reqremoveitem)
  - [ReqEquipment](#reqequipment)
  - [JettisonCargo / TractorObjects / SPScanCargo](#jettisoncargo--tractorobjects--spscancargo)
- [8. Торговля между игроками](#8-торговля-между-игроками)
  - [InitiateTrade / AcceptTrade / TerminateTrade](#initiatetrade--accepttrade--terminatetrade)
  - [AddTradeEquip / DelTradeEquip / SetTradeMoney / TradeResponse / StopTradeRequest](#addtradeequip--deltradeequip--settrademoney--traderesponse--stoptraderequest)
- [9. Движение, управление кораблём, действия игрока](#9-движение-управление-кораблём-действия-игрока)
  - [ActivateEquip / ActivateCruise / ActivateThrusters](#activateequip--activatecruise--activatethrusters)
  - [SetManeuver / SetTarget / SetWeaponGroup](#setmaneuver--settarget--setweapongroup)
  - [SPRequestUseItem](#sprequestuseitem)
- [10. Навигация, миссии, интерфейс, система](#10-навигация-миссии-интерфейс-система)
  - [RequestBestPath / RequestGroupPositions / RequestPlayerStats / RequestRankLevel](#requestbestpath--requestgrouppositions--requestplayerstats--requestranklevel)
  - [InterfaceItemUsed / SetInterfaceState / SetVisitedState](#interfaceitemused--setinterfacestate--setvisitedstate)
  - [MissionResponse / AbortMission / Hail / BaseInfoRequest / LocationInfoRequest](#missionresponse--abortmission--hail--baseinforequest--locationinforequest)
- [11. Майнинг, астероиды, объекты](#11-майнинг-астероиды-объекты)
  - [MineAsteroid](#mineasteroid)
  - [SPMunitionCollision / SPObjCollision / SPObjUpdate](#spmunitioncollision--spobjcollision--spobjupdate)
- [12. Трейдлейны, докинг, физика и ядро сервера](#12-трейдлейны-докинг-физика-и-ядро-сервера)
  - [GoTradelane / StopTradelane](#gotradelane--stoptradelane)
  - [HkCb\_Dock\_Call](#hkcb_dock_call)
    - [Назначение](#назначение-7)
    - [Использование](#использование-4)
  - [HkCb\_Update\_Time](#hkcb_update_time)
  - [HkCb\_Elapse\_Time](#hkcb_elapse_time)
- [FLHook Callbacks](#flhook-callbacks)
  - [HkTimerCheckKick](#hktimercheckkick)
    - [Использование](#использование-5)
  - [SendDeathMsg](#senddeathmsg)
  - [ClearClientInfo](#clearclientinfo)
    - [Использование](#использование-6)
  - [LoadSettings](#loadsettings)
    - [Использование](#использование-7)
  - [BaseDestroyed](#basedestroyed)
- [Практически самые полезные хуки](#практически-самые-полезные-хуки)
  - [Для механик игроков](#для-механик-игроков)
  - [Для команд](#для-команд)
  - [Для боёв / урона](#для-боёв--урона)
  - [Для экономики / предметов](#для-экономики--предметов)
  - [Для движения / особой логики](#для-движения--особой-логики)

## Введение

Если тебе не хватает какого-то хука для конкретной функции FLServer, исторически это предлагалось обсуждать на форуме FLHook Plugin Version board на **the-starport.net**.

Ниже приведён список доступных хуков текущей версии FLHook.

> [!NOTE]
> Если для функции **не указан calling convention**, то по умолчанию используется:

```cpp id="1zq4dq"
__cdecl
```

---

# Базовые системные функции плагина

## `Get_PluginReturnCode`

```cpp id="29l6ch"
PLUGIN_RETURNCODE *Get_PluginReturnCode()
```

### Назначение

Управляет тем, что FLHook должен делать **после вызова твоего плагина**.

### Важно

> [!IMPORTANT]
> Эту функцию **не нужно регистрировать как hook**.

---

## `Get_PluginInfo`

```cpp id="t81c7g"
list<PLUGIN_INFO>* Get_PluginInfo()
```

### Назначение

Возвращает информацию о плагине и заменяет старую регистрацию через `.ini`.

### Важно

> [!IMPORTANT]
> Эту функцию **не нужно регистрировать как hook**.

---

## `Plugin_Communication_CallBack`

```cpp id="nztk06"
void Plugin_Communication_CallBack(PLUGIN_MESSAGE msg, void* data)
```

### Назначение

Callback для **межплагинного взаимодействия**.

---

# FLServer Hooks

Это хуки, привязанные к событиям и внутренним функциям самого FLServer.

---

# 1. Жизненный цикл сервера

## Shutdown

```cpp id="4k3vwd"
void __stdcall HkIServerImpl::Shutdown(void)
```

Вызывается при **завершении работы FLServer**.

---

## Startup

```cpp id="x5a6m4"
bool __stdcall HkIServerImpl::Startup(struct SStartupInfo const &p1)
```

Вызывается при **запуске FLServer**, **до** загрузки персонажей и игровых данных.

### AFTER-вариант

```cpp id="pddxg7"
bool __stdcall HkIServerImpl::Startup_AFTER(struct SStartupInfo const &p1)
```

Вызывается уже **после полной загрузки данных**.

---

## Update

```cpp id="s4zdy4"
int __stdcall HkIServerImpl::Update()
```

Вызывается на **каждом тике FLServer**.

### Использование

Идеальное место для:

- таймеров
- периодических проверок
- логики, работающей постоянно

---

# 2. Игрок: подключение, логин, выбор персонажа

## OnConnect

```cpp id="yy1o0o"
void __stdcall HkIServerImpl::OnConnect(unsigned int)
void __stdcall HkIServerImpl::OnConnect_AFTER(unsigned int)
```

Вызывается при **подключении клиента** к серверу.

---

## Login

```cpp id="mz5dr7"
void __stdcall HkIServerImpl::Login(struct SLoginInfo const &li, unsigned int iClientID)
```

### Особенность

> [!IMPORTANT]
> Плагины вызываются **после этой функции**.

Если тебе нужно отрабатывать **до логина**, используй:

```cpp id="2h9j8p"
OnConnect
```

### Важно

До `Login` многие player-related команды вроде `HkKick` ещё не работают.

### AFTER-вариант

```cpp id="yw0v85"
void __stdcall HkIServerImpl::Login_AFTER(struct SLoginInfo const &li, unsigned int iClientID)
```

Вызывается после `LoadUserSettings`, то есть настройки персонажа уже загружены.

---

## CharacterSelect

```cpp id="g4f0ie"
void __stdcall HkIServerImpl::CharacterSelect(struct CHARACTER_ID const & cId, unsigned int iClientID)
void __stdcall HkIServerImpl::CharacterSelect_AFTER(struct CHARACTER_ID const & cId, unsigned int iClientID)
```

Вызывается при **выборе персонажа**.

---

## CreateNewCharacter

```cpp id="c8ssri"
void __stdcall HkIServerImpl::CreateNewCharacter(struct SCreateCharacterInfo const & scci, unsigned int iClientID)
void __stdcall HkIServerImpl::CreateNewCharacter_AFTER(struct SCreateCharacterInfo const & scci, unsigned int iClientID)
```

Создание нового персонажа.

---

## DestroyCharacter

```cpp id="jsj7mk"
void __stdcall HkIServerImpl::DestroyCharacter(struct CHARACTER_ID const &cId, unsigned int iClientID)
void __stdcall HkIServerImpl::DestroyCharacter_AFTER(struct CHARACTER_ID const &cId, unsigned int iClientID)
```

Удаление персонажа.

---

## DisConnect

```cpp id="s10h80"
void __stdcall HkIServerImpl::DisConnect(unsigned int iClientID, enum EFLConnection p2)
void __stdcall HkIServerImpl::DisConnect_AFTER(unsigned int iClientID, enum EFLConnection p2)
```

Вызывается при **отключении игрока**.

---

# 3. Игрок: запуск, док, переходы, системы

## PlayerLaunch

```cpp id="4u3p77"
void __stdcall HkIServerImpl::PlayerLaunch(unsigned int iShip, unsigned int iClientID)
void __stdcall HkIServerImpl::PlayerLaunch_AFTER(unsigned int iShip, unsigned int iClientID)
```

Запуск игрока в космос / вылет с базы.

---

## LaunchComplete

```cpp id="d9qd7l"
void __stdcall HkIServerImpl::LaunchComplete(unsigned int iBaseID, unsigned int iShip)
void __stdcall HkIServerImpl::LaunchComplete_AFTER(unsigned int iBaseID, unsigned int iShip)
```

Вызывается после завершения запуска.

---

## BaseEnter / BaseExit

```cpp id="9i4qg7"
void __stdcall HkIServerImpl::BaseEnter(unsigned int iBaseID, unsigned int iClientID)
void __stdcall HkIServerImpl::BaseEnter_AFTER(unsigned int iBaseID, unsigned int iClientID)

void __stdcall HkIServerImpl::BaseExit(unsigned int iBaseID, unsigned int iClientID)
void __stdcall HkIServerImpl::BaseExit_AFTER(unsigned int iBaseID, unsigned int iClientID)
```

Вход / выход с базы.

---

## JumpInComplete

```cpp id="njlwmw"
void __stdcall HkIServerImpl::JumpInComplete(unsigned int iSystemID, unsigned int iShip)
void __stdcall HkIServerImpl::JumpInComplete_AFTER(unsigned int iSystemID, unsigned int iShip)
```

Завершение прыжка в систему.

---

## SystemSwitchOutComplete

```cpp id="mt3pcv"
void __stdcall HkIServerImpl::SystemSwitchOutComplete(unsigned int iShip, unsigned int iClientID)
void __stdcall HkIServerImpl::SystemSwitchOutComplete_AFTER(unsigned int iShip, unsigned int iClientID)
```

Выход из системы.

---

## LaunchPosHook

```cpp id="u3p9nm"
bool __stdcall LaunchPosHook(uint iSpaceID, struct CEqObj &p1, Vector &p2, Matrix &p3, int iDock)
```

### Назначение

Позволяет **изменить позицию появления корабля при запуске**.

### Использование

Очень полезен для:

- mobile docking
- кастомного undock
- телепортации
- proxy base логики

---

# 4. Чат и пользовательские команды

## SubmitChat

```cpp id="nv2s51"
void __stdcall HkIServerImpl::SubmitChat(CHAT_ID, unsigned long, void const *, CHAT_ID, int)
void __stdcall HkIServerImpl::SubmitChat_AFTER(CHAT_ID, unsigned long, void const *, CHAT_ID, int)
```

Отправка сообщений в чат.

### Использование

Подходит для:

- кастомных чат-команд
- логирования чата
- фильтрации сообщений
- антиспама

---

## HkCb_SendChat

```cpp id="pfk7m4"
void __stdcall HkCb_SendChat(uint iClientID, uint iTo, uint iSize, void *pRDL)
```

Хук на отправку чат-пакета.

---

## UserCmd_Process

```cpp id="p2d1o8"
bool UserCmd_Process(uint iClientID, const wstring &wscCmd)
```

### Назначение

Позволяет вставлять свои пользовательские команды **без ковыряния `SubmitChat`**.

### Важно

Если команда обработана, функция должна вернуть:

```cpp id="b9ifpq"
true
```

---

## UserCmd_Help

```cpp id="e1k34h"
void UserCmd_Help(uint iClientID, const wstring &wscParam)
```

Вызывается при использовании команды помощи.

Используется для вывода собственных пользовательских команд через:

```cpp id="ql14l8"
PrintUserCmdText
```

---

# 5. Админ-команды и консоль

## ExecuteCommandString_Callback

```cpp id="mqzg7z"
bool ExecuteCommandString_Callback(CCmds* classptr, const wstring &wscCmdStr)
```

### Назначение

Добавление собственных **админ-команд**.

### Важно

Если команда обработана, рекомендуется:

- вернуть `true`
- выставить appropriate returncode (`NOFUNCTIONCALL`, если нужно)

---

## CmdHelp_Callback

```cpp id="fh8byq"
void CmdHelp_Callback(CCmds* classptr)
```

Вызывается при админской команде help.

Используется для отображения добавленных админ-команд.

---

# 6. Бой, урон, оружие, уничтожение

## FireWeapon

```cpp id="2kj8rb"
void __stdcall HkIServerImpl::FireWeapon(unsigned int iClientID, struct XFireWeaponInfo const &wpn)
void __stdcall HkIServerImpl::FireWeapon_AFTER(unsigned int iClientID, struct XFireWeaponInfo const &wpn)
```

Выстрел оружием.

---

## ShipDestroyed

```cpp id="4qg55s"
void __stdcall ShipDestroyed(DamageList *_dmg, DWORD *exs, uint iKill)
```

Уничтожение корабля.

### Использование

Один из самых полезных боевых хуков.

---

## AllowPlayerDamage

```cpp id="qqf8v1"
bool AllowPlayerDamage(uint iClientID, uint iClientIDTarget)
```

### Назначение

Позволяет контролировать урон между игроками **без грязного вмешательства в damage hooks**.

---

## HkCb_AddDmgEntry

```cpp id="uxd0kp"
void __stdcall HkCb_AddDmgEntry(DamageList *dmgList, unsigned short p1, float p2, enum DamageEntry::SubObjFate p3)
void __stdcall HkCb_AddDmgEntry_AFTER(DamageList *dmgList, unsigned short p1, float p2, enum DamageEntry::SubObjFate p3)
```

Работа с записью урона.

---

## HkCB_MissileTorpHit

```cpp id="r8hjxe"
int __stdcall HkCB_MissileTorpHit(char *ECX, char *p1, DamageList *dmg)
```

Хук на попадание ракет / торпед.

---

## HkCb_GeneralDmg

```cpp id="y0h7xw"
void __stdcall HkCb_GeneralDmg(char *szECX)
```

Общая обработка урона.

---

# 7. Торговля, деньги, предметы, рынок

## ReqChangeCash / ReqSetCash

```cpp id="01f0q5"
void __stdcall HkIServerImpl::ReqChangeCash(int p1, unsigned int iClientID)
void __stdcall HkIServerImpl::ReqChangeCash_AFTER(int p1, unsigned int iClientID)

void __stdcall HkIServerImpl::ReqSetCash(int p1, unsigned int iClientID)
void __stdcall HkIServerImpl::ReqSetCash_AFTER(int p1, unsigned int iClientID)
```

Работа с изменением денег.

---

## GFGoodBuy / GFGoodSell

```cpp id="ewntdj"
void __stdcall HkIServerImpl::GFGoodBuy(struct SGFGoodBuyInfo const &gbi, unsigned int iClientID)
void __stdcall HkIServerImpl::GFGoodBuy_AFTER(struct SGFGoodBuyInfo const &gbi, unsigned int iClientID)

void __stdcall HkIServerImpl::GFGoodSell(struct SGFGoodSellInfo const &gsi, unsigned int iClientID)
void __stdcall HkIServerImpl::GFGoodSell_AFTER(struct SGFGoodSellInfo const &gsi, unsigned int iClientID)
```

Покупка и продажа товаров.

---

## ReqAddItem / ReqModifyItem / ReqRemoveItem

```cpp id="k9t8dn"
void __stdcall HkIServerImpl::ReqAddItem(...)
void __stdcall HkIServerImpl::ReqModifyItem(...)
void __stdcall HkIServerImpl::ReqRemoveItem(...)
```

Работа с инвентарём и предметами.

---

## ReqEquipment

```cpp id="9lqqy6"
void __stdcall HkIServerImpl::ReqEquipment(class EquipDescList const &edl, unsigned int iClientID)
void __stdcall HkIServerImpl::ReqEquipment_AFTER(class EquipDescList const &edl, unsigned int iClientID)
```

Изменение экипировки.

---

## JettisonCargo / TractorObjects / SPScanCargo

```cpp id="z74n0r"
void __stdcall HkIServerImpl::JettisonCargo(...)
void __stdcall HkIServerImpl::TractorObjects(...)
void __stdcall HkIServerImpl::SPScanCargo(...)
```

Работа с грузом и объектами в космосе.

---

# 8. Торговля между игроками

## InitiateTrade / AcceptTrade / TerminateTrade

```cpp id="yz4x5r"
void __stdcall HkIServerImpl::InitiateTrade(...)
void __stdcall HkIServerImpl::AcceptTrade(...)
void __stdcall HkIServerImpl::TerminateTrade(...)
```

Все основные хуки торговли между игроками.

---

## AddTradeEquip / DelTradeEquip / SetTradeMoney / TradeResponse / StopTradeRequest

Полный контроль над логикой трейда.

---

# 9. Движение, управление кораблём, действия игрока

## ActivateEquip / ActivateCruise / ActivateThrusters

```cpp id="bx2ztl"
void __stdcall HkIServerImpl::ActivateEquip(...)
void __stdcall HkIServerImpl::ActivateCruise(...)
void __stdcall HkIServerImpl::ActivateThrusters(...)
```

Активация оборудования, круиза и форсажа.

---

## SetManeuver / SetTarget / SetWeaponGroup

```cpp id="hktu2f"
void __stdcall HkIServerImpl::SetManeuver(...)
void __stdcall HkIServerImpl::SetTarget(...)
void __stdcall HkIServerImpl::SetWeaponGroup(...)
```

Манёвры, цели, группы оружия.

---

## SPRequestUseItem

```cpp id="vlz6fm"
void __stdcall HkIServerImpl::SPRequestUseItem(struct SSPUseItem const &p1, unsigned int iClientID)
```

Использование предметов / оборудования.

---

# 10. Навигация, миссии, интерфейс, система

## RequestBestPath / RequestGroupPositions / RequestPlayerStats / RequestRankLevel

Запросы клиента на навигацию, позиции группы, статистику и ранг.

---

## InterfaceItemUsed / SetInterfaceState / SetVisitedState

Работа с UI и состоянием интерфейса.

---

## MissionResponse / AbortMission / Hail / BaseInfoRequest / LocationInfoRequest

Работа с миссиями, взаимодействием и данными по локациям.

---

# 11. Майнинг, астероиды, объекты

## MineAsteroid

```cpp id="1x1llw"
void __stdcall HkIServerImpl::MineAsteroid(...)
void __stdcall HkIServerImpl::MineAsteroid_AFTER(...)
```

Хук для контроля майнинга.

---

## SPMunitionCollision / SPObjCollision / SPObjUpdate

```cpp id="1q0r1l"
void __stdcall HkIServerImpl::SPMunitionCollision(...)
void __stdcall HkIServerImpl::SPObjCollision(...)
void __stdcall HkIServerImpl::SPObjUpdate(...)
```

Коллизии, обновления объектов, столкновения.

---

# 12. Трейдлейны, докинг, физика и ядро сервера

## GoTradelane / StopTradelane

```cpp id="h7d5ow"
void __stdcall HkIServerImpl::GoTradelane(...)
void __stdcall HkIServerImpl::StopTradelane(...)
```

Контроль использования trade lane.

---

## HkCb_Dock_Call

```cpp id="az5qj6"
int __cdecl HkCb_Dock_Call(unsigned int const &uShipID, unsigned int const &uSpaceID, int p3, enum DOCK_HOST_RESPONSE p4)
```

### Назначение

Один из самых мощных хуков.

Вызывается при **любом docking-поведении**:

- игроки
- NPC
- вход в очередь
- попытка стыковки

### Использование

Идеален для:

- mobile docking
- custom docking logic
- ограничений стыковки
- проверки расстояния / условий

---

## HkCb_Update_Time

```cpp id="e1nlta"
void __cdecl HkCb_Update_Time(double dInterval)
void __cdecl HkCb_Update_Time_AFTER(double dInterval)
```

Хук на обновление глобального времени.

Оригинальная функция:

```cpp id="xv4v2w"
Timing::UpdateGlobalTime(dInterval)
```

---

## HkCb_Elapse_Time

```cpp id="jq0x1d"
void __stdcall HkCb_Elapse_Time(float p1)
void __stdcall HkCb_Elapse_Time_AFTER(float p1)
```

Главный рабочий цикл:

- NPC
- физика
- игровые процессы

Оригинальная функция:

```cpp id="7g8n2p"
Server.ElapseTime(p1)
```

---

# FLHook Callbacks

Это уже не прямые хуки FLServer, а **callback-и самого FLHook**.

---

## HkTimerCheckKick

```cpp id="ifn8l8"
void HkTimerCheckKick()
```

Вызывается **каждую секунду**.

### Использование

Идеален для:

- таймеров
- delayed actions
- проверок состояний
- cleanup

---

## SendDeathMsg

```cpp id="xas7cg"
void SendDeathMsg(const wstring &wscMsg, uint iSystemID, uint iClientIDVictim, uint iClientIDKiller)
```

Позволяет изменить механику отправки death messages.

---

## ClearClientInfo

```cpp id="nd0hr2"
void ClearClientInfo(uint iClientID)
```

Вызывается при подключении нового игрока.

### Использование

Сбрасывай здесь:

- player-specific state
- кэш
- временные структуры
- свои plugin-data по клиенту

---

## LoadSettings

```cpp id="f4u0sy"
void LoadSettings()
```

Вызывается:

- при старте FLHook / FLServer
- при `rehash`

### Использование

Здесь нужно:

- загружать `.ini`
- обновлять конфиги
- пересчитывать ID / кэш / настройки

---

## BaseDestroyed

```cpp id="l5jv9s"
void BaseDestroyed(uint iObject, uint iClientIDBy)
```

Вызывается при уничтожении **докабельной базы**.

- `iObject` — SpaceID базы
- `iClientIDBy` — кто уничтожил

---

# Практически самые полезные хуки

Если говорить честно, то **90% серверной логики** обычно делается вот на этих хуках:

---

## Для механик игроков

- `PlayerLaunch`
- `BaseEnter`
- `BaseExit`
- `CharacterSelect`
- `Login`
- `DisConnect`

## Для команд

- `UserCmd_Process`
- `ExecuteCommandString_Callback`
- `CmdHelp_Callback`

## Для боёв / урона

- `ShipDestroyed`
- `AllowPlayerDamage`
- `FireWeapon`
- `HkCb_AddDmgEntry`

## Для экономики / предметов

- `GFGoodBuy`
- `GFGoodSell`
- `ReqEquipment`
- `ReqChangeCash`
- `ReqModifyItem`

## Для движения / особой логики

- `Dock_Call`
- `LaunchPosHook`
- `MineAsteroid`
- `HkTimerCheckKick`

---
