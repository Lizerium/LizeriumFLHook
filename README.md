> **26.11.2020 23:26 - по настоящее время**

# ⏬Русское описание⏬ [English desc.](README.en..md)

FLHook(**_8.0.0_**)

> ## Версия: **Dvurechensky**
>
> ## Игра: **Freelancer Lizerium**

Это изначально частный репозиторий **FLHook** от **Freelancer Lizerium**.
Гибрид репозиториев большого количества мертвых серверов Freelancer.
В него включены клиентский хук и много плагинов под него.

> [!WARNING]
> Данный проект - был протестирован успешно на [Lizerium](https://lizup.ru/) сервере от меня [Dvurechensky](https://www.dvurechensky.pro/)

> [!CAUTION]
> Если вы увидели этот проект в общем доступе значит я решил поделиться с вами не только этим проектом но и всем [Lizerium](https://lizup.ru/)

### Документация

- [Разработка плагинов для FLHook](<docs/PLUGIN_DOCS/Разработка плагинов для FLHook.md>)
- [Available Hooks — Хуки FLHook](<docs/PLUGIN_DOCS/Available Hooks — Хуки FLHook.md>)
- [Plugin How-To для FLHook](<docs/PLUGIN_DOCS/Plugin How-To для FLHook.md>)
- [SDK Files & Inter-Plugin Communication для FLHook](<docs/PLUGIN_DOCS/SDK Files & Inter-Plugin Communication для FLHook.md>)
- [Troubleshooting для FLHook Plugins](<docs/PLUGIN_DOCS/Troubleshooting для FLHook Plugins.md>)

### 🈴ВАЖНО: МНЕ НИКТО НЕ ПОМОГАЛ, ВСЕ ОТКАЗАЛИСЬ ИЛИ ПРОИГНОРИРОВАЛИ🈴

- Этот проект настроен для работы в **Visual Studio 2022 Professional**.
- Версия **Windows SDK - 8.1**.
- Компилятор - **VC141**.
- ![MFC](src/Media/MFC.png)

## 📳Как пользоваться📳

- Не используйте решение в конфигурации **DEBUG**. **Всегда** используйте **RELEASE**🌱 **WIN32**🌱
- Ваши скомпилированные плагины находятся в **Binaries/bin-vc14/flhook_plugins** - `ПОЧТИ`, часть в папках Release соответствующих плагинов.

## 📳Спецификация📳

### 📳Оглавление📳

1. [AFK](#AFK) | [README](src/Plugins/Public/afk/README.md)
2. [ALLEY](#ALLEY) | [README](src/Plugins/Public/alley/README.md)
3. [AUTOBUY](#AUTOBUY) | [README](src/Plugins/Public/autobuy/README.md)
4. [BALANCE MAGIC](#BALANCE-MAGIC) | [README](src/Plugins/Public/balancemagic/README.md)
5. [BANKER](#BANKER) | [README](src/Plugins/Public/banker/README.md)
6. [BASE](#BASE) | [README](src/Plugins/Public/base_plugin/README.md)
7. [BEAM ME UP](#BEAM-ME-UP) | [README](src/Plugins/Public/beammeup/README.md)
8. [BOUNTY HUNT](#BOUNTY-HUNT) | [README](src/Plugins/Public/bountyhunt/README.md)
9. [BUILD](#BUILD) | [README](src/Plugins/Public/builds/README.md)
10. [CLOAK VERSION 2](#CLOAK-VERSION-2) | [README](src/Plugins/Public/cloak_plugin/README.md)
11. [CLOAK VERSION 1](#CLOAK-VERSION-1) | [README](src/Plugins/Public/cloak_kosa/README.md)
12. [COMMODITY LIMIT](#COMMODITY-LIMIT) | [README](src/Plugins/Public/commoditylimit/README.md)
13. [CONNECTION](#CONN) | [README](src/Plugins/Public/conn_plugin/README.md)
14. [DOCK RESTRICT](#DOCK-RESTRICT) | [README](src/Plugins/Public/dockrestrict/README.md)
15. [DOCK TAG](#DOCK-TAG) | [README](src/Plugins/Public/docktag/README.md)
16. [DRONE TYPES](#DRON-BAYS) | [README](src/Plugins/Public/dronebays/README.md)
17. [DYNAMIC MISSION 2](#DYNAMIC-MISSION-2) | [README](src/Plugins/Public/dynamic_mission_2/README.md)
18. [EVENT](#EVENT) | [README](src/Plugins/Public/event/README.md)
19. [FAST START](#FAST-START) | [README](src/Plugins/Public/fast_start/README.md)
20. [FTL](#FTL) | [README](src/Plugins/Public/ftl/README.md)
21. [HELP](#HELP) | [README](src/Plugins/Public/help_expanded/README.md)
22. [GETREP](#GET-REP) | [README](src/Plugins/Public/getrep/README.md)
23. [ITEM RESTRICTIONS](#ITEM-RESTRICTIONS) | [README](src/Plugins/Public/item_restrict/README.md)
24. [JSON BUDDY](#JSON-BUDDY) | [README](src/Plugins/Public/JSONBuddy/README.md)
25. [KILL COUNTER](#KILL-COUNTER) | [README](src/Plugins/Public/killcounter/README.md)
26. [LZ COMPAT](#LZ-COMPAT) | [README](src/Plugins/Public/LZCompat/README.md)
27. [MARK](#MARK) | [README](src/Plugins/Public/mark/README.md)
28. [MARKET FUCKER](#MARKET-FUCKER) | [README](src/Plugins/Public/MarketFucker/README.md)
29. [MINE CONTROLE VERSION 1](#MINE-CONTROLE-VERSION-1) | [README](src/Plugins/Public/minecontrol_plugin/README.md)
30. [MISCELLANEOUS COMMANDS](#MISCELLANEOUS-COMMANDS) | [README](src/Plugins/Public/MiscellaneousCommands/README.md)
31. [NPC CONTROL](#NPC-CONTROL) | [README](src/Plugins/Public/npc_control/README.md)
32. [REG ARMOUR](#REG-ARMOUR) | [README](src/Plugins/Public/reg_armour/README.md)
33. [TEMP BAN](#TEMP-BAN) | [README](src/Plugins/Public/tempban/README.md)
34. [TEMPLATE](#TEMPLATE) | [README](src/Plugins/Public/__PLUGIN_TEMPLATE/README.md)
35. [PVE CONTROLLER](#PVE-CONTROLLER) | [README](src/Plugins/Public/pvecontroller/README.md)
36. [STORAGE](#STORAGE) | [README](src/Plugins/Public/storage/README.md)
37. [PLAYER CONTROLLER](#PLAYER-CONTROLLER) | [README](src/Plugins/Public/playercntl_plugin/README.md)

---

### AFK

⤴️[Оглавление](#Оглавление)

```bash
/afk
```

```ini
Устанавливает пользователю метку AFK. Если сообщения других игроков ему
отправлены, им будет сказано, что вы временно недоступны.
```

> ---

```bash
/back
```

```ini
Отключает метку AFK для пользователя.
```

> ---

---

### ALLEY

⤴️[Оглавление](#Оглавление)

```bash
/nodock
/nodock*
```

```ini
Игрок с соответствующим удостоверением может удалить разрешения на стыковку у другого игрока выделив его как цель в космосе
```

> ---

```bash
/police
/police*
```

```ini
Игрок с удостоверением полиции может командой включить/выключить полицейскую систему оповещения сиреной
```

> ---
>
> /pirate
> /pirate\*

```ini
Присваивает вас к фракции пираты
```

> ---

```bash
/racestart
```

```ini
Все кто имеет допустимую лицензию можно начинать отправлять звук старта гонки для игроков в пределах 5000 при вводе команды
```

> ---

```bash
/gift
/gift*
```

```ini
Дарит по идее вам деньги
```

> ---

```bash
$help
$help*
```

```ini
Выводит справку по команде /alley
```

> ---

```bash
$chase <charname>
$chase* <charname>
```

```ini
Телепортирует вас через Боба к нужному игроку. Только для администратора имеющего платиновый уровень.
```

> ---

```bash
/marktarget
/marktarget*
```

> [!IMPORTANT]
> Описание:

```ini
Установка маркера на цель для группы
```

> ---

```bash
/jettisonall
```

```ini
Выбрасывает все ваши предметы в космос
```

> ---

---

### AUTOBUY

⤴️[Оглавление](#Оглавление)

```bash
/autobuy <param> [<on/off>]

/autobuy info - показать информацию об автокупке
/autobuy missiles on/off - включить/выключить автокупку ракет
/autobuy torps on/off - включить/выключить автокупку торпед
/autobuy mines on/off - включить/выключить автокупку мин
/autobuy cd on/off - включить/выключить автокупку блокираторов
/autobuy cm on/off - включить/выключить автокупку обманок
/autobuy reload on/off - включить/выключить автокупку нанороботов и батарей щита
/autobuy all on/off - включить/выключить автокупку всех расходников

Примеры:
/autobuy info - показать информацию об автокупке
/autobuy all on - включить автокупку всех расходников
/autobuy torps off - отключить автокупку торпед
```

```ini
Управляет автокупкой расходников
```

> ---

---

### BALANCE MAGIC

⤴️[Оглавление](#Оглавление)

```bash
/snacclassic
```

```ini
Создаёт классические амуницию dsy_snova_classic вместо dsy_snova_civ
```

> ---

---

### BANKER

⤴️[Оглавление](#Оглавление)

```bash
/bank <amount>
```

```ini
Переводит деньги в банк из баланса пользователя
```

> ---

```bash
/bankw <amount>
```

```ini
Переводит деньги из банка в баланс пользователя
```

> ---

---

### BASE

⤴️[Оглавление](#Оглавление)

```bash
/base help [page]
```

```ini
Показать эту страницу помощи. Укажите номер страницы, чтобы увидеть следующую страницу.
```

> ---

```bash
/base login [password]
```

```ini
Войдите как администратор базы.
Следующие команды доступны только в том случае, если вы вошли в систему как базовый администратор.
```

> ---

```bash
/base addpwd [password]
```

```bash
/base addpwd [password] [viewshop]
```

```ini
Добавляйте пароли администратора для базы
Добавьте viewhop в addpwd, чтобы разрешить просмотр магазина только с помощью пароля.
```

> ---

```bash
/base rmpwd [password]
```

```ini
Удаляйте пароли администратора для базы
```

> ---

```bash
/base lstpwd
```

```ini
Перечисляйте пароли администратора для базы
```

> ---

```bash
/base setmasterpwd [old password] [new password]
```

```ini
Установите мастер-пароль для базы.
```

> ---

```bash
/base addtag [tag]
```

```ini
Добавляйте теги союзников для базы.
```

> ---

```bash
/base rmtag [tag]
```

```ini
Удаляйте теги союзников для базы.
```

> ---

```bash
/base lsttag
```

```ini
Перечисляйте теги союзников для базы.
```

> ---

```bash
/base addhostile [tag]
```

```ini
Добавляйте в черный список теги базы.
Они будут расстреляны на месте, поэтому используйте полные теги, такие как like =LSF= или IMG| или shipname like Crunchy_Salad
```

> ---

```bash
/base rmhostile [tag]
```

```ini
Удаляйте из черного списка теги базы.
Они будут расстреляны на месте, поэтому используйте полные теги, такие как like =LSF= или IMG| или shipname like Crunchy_Salad
```

> ---

```bash
/base lsthostile
```

```ini
Перечисляйте черный список тегов базы.
Они будут расстреляны на месте, поэтому используйте полные теги, такие как like =LSF= или IMG| или shipname like Crunchy_Salad
```

> ---

```bash
/base rep
```

```bash
/base rep [clear]
```

```ini
Установите или удалите фракцию, к которой принадлежит эта база.
При настройке принадлежности к фракции выбор падёт на ту к которой принадлежит игрок, выполняющий команду.
```

> ---

```bash
/base defensemode
```

```ini
Управляйте режимом защиты базы.
Defense Mode 1 - Logic: Blacklist > Whitelist > IFF Standing.
Права на стыковку: только корабли, внесенные в белый список.
Defense Mode 2 - Logic: Blacklist > Whitelist > IFF Standing.
Права на стыковку: любой человек с хорошей репутацией.
Defense Mode 3 - Logic: Blacklist > Whitelist > Hostile
Права на стыковку: только корабли, внесенные в белый список.
Defense Mode 4 - Logic: Blacklist > Whitelist > Neutral
Права на стыковку: любой человек с хорошей репутацией.
Defense Mode 5 - Logic: Blacklist > Whitelist > Neutral
Права на стыковку: только корабли, внесенные в белый список.
```

> ---

```bash
/base deploy <password> <basename>
```

```ini
Создаёт базу при наличии ремонтного корабля, предварительной полной остановке перед созданием, груза для строительства
```

> ---

```bash
/shop price [item] [price] [min stock] [max stock]
```

```ini
Установите [price] для [item]. Если текущий запас меньше [min stock]
тогда предмет нельзя будет купить на пришвартованных кораблях. Если текущий запас больше или равен
[max stock] тогда предмет нельзя будет продать на базу пришвартованными кораблями.
Запретить продажу на базу предмета пришвартованными кораблями при всех условиях, [max stock] установить в 0.
Запретить покупку предмета с базы пристыкованными кораблями при любых условиях, [min stock] установить в 0.
```

> ---

```bash
/shop remove [item]
```

```ini
Удалить товар из списка акций.
Его нельзя продать на базу пришвартованными кораблями, если они не являются администраторами базы.
```

> ---

```bash
/shop [page]
```

```ini
Показать список товаров магазина [page]. На странице отображается максимум 40 элементов.
```

> ---

```bash
/basebank withdraw [credits],
```

```ini
Снимайте кредиты имеющиеся в базовом банке.
```

> ---

```bash
/basebank deposit [credits],
```

```ini
Вносите в базовый банк кредиты.
```

> ---

```bash
/basebank status
```

```ini
Проверяйте состояние кредитов, имеющихся в базовом банке.
```

> ---

```bash
/base info
```

```ini
Задайте описание инфокарты базы.
```

> ---

```bash
/base facmod
```

```ini
Управление заводскими модулями.
```

> ---

```bash
/base defmod
```

```ini
Управляйте модулями защиты.
```

> ---

```bash
/base shieldmod
```

```ini
Модули щита управления.
```

> ---

```bash
/base buildmod
```

```ini
Контролируйте строительство и разрушение базовых модулей и улучшений.
```

> ---

---

### BEAM ME UP

⤴️[Оглавление](#Оглавление)

```bash
/beammeup
```

```ini
Телепорт в другую точку галактики, при условии нахождения пользователя на станции(базе) телепортации
```

> ---

```bash
/return
```

```ini
Возвращает игрока на станцию телепортации
```

```bash
/beammeup aldrin
```

```ini
Телепорт в Альдрин, при наличии в трюме пропуска в Альдрин
```

```bash
/beammeup secret
```

```ini
Телепорт в Секретную Локацию, при наличии в трюме пропуска в Секретную Локацию
```

> ---

---

### BOUNTY HUNT

⤴️[Оглавление](#Оглавление)

```bash
/bountyhunt <charname> <credits> [<minutes>]
```

```ini
Предлагает награду в <credits> кредитов за убийство <charname> в течение следующих <minutes> минут
```

> ---

```bash
/bountyhuntid <id> <credits> [<minutes>]
```

```ini
Предлагает награду в <credits> кредитов за убийство <id> в течение следующих <minutes> минут
```

> ---

---

### BUILD

⤴️[Оглавление](#Оглавление)

```bash
/bequip-<id>
```

```ini
Cоздаст элемент выбранного номера в списке №1, №2 (/bequip - выведет справку)
```

> ---

```bash
/bwep-<id>
```

```ini
Cоздаст элемент выбранного номера в списке №1, №2(/bwep - выведет справку)
```

> ---

```bash
/bammo-<id>
```

```ini
Cоздаст элемент выбранного номера в списке №1, №2(/bammo - выведет справку)
```

> ---

---

### CLOAK VERSION 2

⤴️[Оглавление](#Оглавление)

```bash
/cloak
/cloak*
```

```ini
Погружает корабль в невидимость
```

> ---

```bash
/disruptor
/disruptor*
```

```ini
Разрушает невидимость вокруг игрока, в зависимости от установленного разрушителя
```

> ---

---

### CLOAK VERSION 1

⤴️[Оглавление](#Оглавление)

```bash
/cloak
/c
```

```ini
Погружает корабль в невидимость
```

> ---

```bash
/uncloak
/uc
```

```ini
Выключает маскировку
```

> ---

---

### COMMODITY LIMIT

## ⤴️[Оглавление](#Оглавление)

---

### CONN

## ⤴️[Оглавление](#Оглавление)

```bash
/conn
```

```ini
Телепортирует игрока в другую систему
```

> ---

```bash
/return
```

```ini
Возвращает игрока в систему, из которой он был телепортирован
```

> ---

---

### DOCK RESTRICT

## ⤴️[Оглавление](#Оглавление)

### DOCK TAG

## ⤴️[Оглавление](#Оглавление)

### DRON BAYS

⤴️[Оглавление](#Оглавление)

```bash
/deploydrone [DroneType]
/dd [DroneType]
```

```ini
Запускает дрон, совместимый с вашим дронбаем
```

> ---

```bash
/dronetarget
/dta
```

```ini
Направляет ваш дрон атаковать выбранную цель
```

> ---

```bash
/dronedebug
```

```ini
Перечисляет содержимое карты дронов пользователя.
```

> ---

```bash
/dronestop
/ds
```

```ini
Это заставляет дрон останавливаться, что бы он ни делал, и сидеть на месте.
```

> ---

```bash
/dronestop
/ds
```

```ini
Это заставляет дрон останавливаться, что бы он ни делал.
```

> ---

```bash
/dronerecall
/dr
```

```ini
Отзывает ваш дрон и стыкует его с носителем.
```

> ---

```bash
/dronehelp
/dh
```

```ini
Выводит справку по управлению дронами.
```

> ---

```bash
/dronetypes
/dt
```

```ini
Список всех доступных типов дронов для вашего типа отсека
```

> ---

```bash
/dronecome
/dc
```

```ini
Отключает дрон от его текущей цели и направляет его к вашей позиции
```

> ---

---

### DYNAMIC MISSION 2

## ⤴️[Оглавление](#Оглавление)

### EVENT

## ⤴️[Оглавление](#Оглавление)

### FAST START

## ⤴️[Оглавление](#Оглавление)

### FTL

⤴️[Оглавление](#Оглавление)

```bash
/ftl
/ftl [x] [y] [z]
```

```ini
Телепорт со смещением на указанные координаты к выделенному объекту
Например: /ftl 100 -100 -100 -> телепорт на 100 вправо, 100 вниз, 100 назад от выделенного объекта
```

> ---

```bash
/sftl [x] [y] [z]
```

```ini
Телепорт на указанные координаты
```

> ---

```bash
/j
```

```ini
Телепорт к выделенному объекту
```

> ---

```bash
/coords
```

```ini
Предоставляет координаты вашей цели для использования командами /ftl или /sftl
```

> ---

---

### HELP

⤴️[Оглавление](#Оглавление)

```bash
/helpmenu
/helpmenu*
/helpmenu <page>
/helpmenu* <page>
```

```ini
Выводит список меню помощи или открывает страницу помощи с номером <page>
```

> ---

```bash
/start
/start*
```

```ini
Выводит стартовую ознакомительную страницу
```

> ---

```bash
/rules
/rules*
```

```ini
Выводит страницу правил
```

> ---

---

### GET REP

⤴️[Оглавление](#Оглавление)

```bash
/rep <page>
/rep* <page>
```

```ini
Выводит список отношений к игроку всех фракций. Используйте страницу 1 и 2.
```

> ---

---

### ITEM RESTRICTIONS

⤴️[Оглавление](#Оглавление)

> ---

---

### JSON BUDDY

⤴️[Оглавление](#Оглавление)

> ---

```bash
/ping
```

```ini
Показывает ваш пинг
```

> ---

```bash
/pingtarget
```

```ini
Показывает пинг цели
```

> ---

---

### KILL COUNTER

⤴️[Оглавление](#Оглавление)

> ---

```bash
/kills <charname>
```

```ini
Список убийств игрока
```

> ---

---

### LZ COMPAT

⤴️[Оглавление](#Оглавление)

> ---

---

### MARK

⤴️[Оглавление](#Оглавление)

> ---

```bash
/mark
/m
```

```ini
Заставляет выбранный объект отображаться в важном разделе левого нижнего меню и быть полноценно выделенным.
```

> ---

```bash
/unmark
/um
```

```ini
Снимает выделение с выбранного объекта
```

> ---

```bash
/unmarkall
/uma
```

```ini
Снимает выделение со всех выделенных объектов
```

> ---

```bash
/groupmark
/gm
```

```ini
Выделяет объект для всей группы.
```

> ---

```bash
/groupunmark
/gum
```

```ini
Снимает выбранный объект из выделения для всей группы.
```

> ---

```bash
/ignoregroupmarks
```

```ini
Игнорирует отметки других участников вашей группы.
```

> ---

```bash
/automark <on|off> [radius_in_KM]
```

```ini
Автоматически маркирует игроков вокруг в определённом радиусе (например: /automark on 10000)
```

> ---

```bash
/sonar <on|off>
```

```ini
Сонар автоматически подсвечивает все контейнеры в радиусе заданном на сервере каждые 45 секунд.
```

> ---

---

### MARKET FUCKER

⤴️[Оглавление](#Оглавление)

> ---

---

### MINE CONTROLE VERSION 1

⤴️[Оглавление](#Оглавление)

> ---

---

### MISCELLANEOUS COMMANDS

⤴️[Оглавление](#Оглавление)

> ---

```bash
/refresh
/refresh*
```

```ini
Обновляет временные метки файла персонажа для всех кораблей на учетной записи.
```

> ---

```bash
/freelancer
/freelancer*
```

```ini
Дает пользователю IFF фрилансера (нейтральная ко всем фракция)
```

> ---

---

### NPC CONTROL

⤴️[Оглавление](#Оглавление)

> ---

---

### REG ARMOUR

⤴️[Оглавление](#Оглавление)

> ---

---

### TEMP BAN

⤴️[Оглавление](#Оглавление)

> ---

---

### TEMPLATE

⤴️[Оглавление](#Оглавление)

> ---

```bash
/template
/template*
```

```ini
Выводит наличие конфига autobuy.cfg
```

> ---

---

### PVE CONTROLLER

⤴️[Оглавление](#Оглавление)

> ---

```bash
/pool
```

```ini
Баунти-пул статус выплат
```

> ---

```bash
/value
```

```ini
Стоимость корабля
```

> ---

---

### STORAGE

⤴️[Оглавление](#Оглавление)

> ---

```bash
/shelp
```

```ini
Показывает полный список команд связанных со складами
```

> ---

```bash
/store [n] [ammount]
/store all
```

```ini
Отправляет груз на склад <n> номер груза корабля (узнать - /enumcargo)
```

> ---

```bash
/unstore [n] [ammount]
/unstore all
```

```ini
Отправит весь груз со склада в корабль
```

> ---

```bash
/sinfo
```

```ini
Показывает количество предметов лежащих на складе текущей системы, поддерживает постраничный вывод информации, например /sinfo 1 и тд.
```

> ---

```bash
/enumcargo
```

```ini
Показывает количество предметов лежащих в трюме вашего корабля
```

> ---

```bash
/cargosystems
/carsys
```

```ini
Выводит список систем в которых есть груз в складах с указанием количества этого груза
```

> ---

```bash
/searchgood [searchstring]
/sh [searchstring]
```

```ini
Ищет товар в ваших складах по всей галактике (можно писать часть названия - он выведет похожие)
```

> ---

---

### PLAYER CONTROLLER

⤴️[Оглавление](#Оглавление)

```bash
/help
/h
/?
```

```ini
Печать пользовательской справки с переопределением встроенной справки
```

> ---

```bash
/commandlist
```

```ini
Печать пользовательской справки о командах
```

> ---

```bash
/changeship [id]
/cs [id]
```

```ini
Сохраняет корабль в импровизированный ангар игрока на котором вы находитесь в текущий момент.
Позволяет изменить корабль на другой, работает смена только в космосе. На сервере существует ограничение в количество кораблей которые вы можете сохранять в свой ангар, будьте внимательны! Вы можете удалить корабль из ангара введя команду /ds (или /delship)
```

> ---

```bash
/delship [id]
/ds [id]
```

```ini
Удаляет сохранённый корабль из списка тех, что появлялись по мере покупки (относится к импровизированному ангару игрока)
```

> ---

```bash
/pos
```

```ini
Печать текущей позиции
```

> ---

```bash
/stuck
```

```ini
Немного переместите корабль, если он застрял в базе
```

> ---

```bash
/droprep
```

```ini
Команда, помогающая удалить любую принадлежность к фракции, которая может у вас есть
```

> ---

```bash
/droprep
```

```ini
Команда, помогающая удалить любую принадлежность к фракции, которая может у вас есть
```

> ---

```bash
/dice 1d20 | 1d20+3 | etc.
/roll 1d20 | 1d20+3 | etc.

/coin 1d20 | 1d20+3 | etc.
```

```ini
Бросание костей и разыгрывание рандомных комбинаций для жеребьёвок
```

> ---

```bash
/pimpship
```

```ini
Команда(ы) для настройки освещения корабля (на станции у диллера на эту тему)
```

> ---

```bash
/showsetup
```

```ini
Показать настройку корабля
```

> ---

```bash
/showitems
```

```ini
Отображение элементов, которые могут быть изменены.
```

> ---

```bash
/setitem
```

```ini
Измените элемент (маяк освещения) в идентификаторе слота на указанный элемент.
```

> ---

```bash
/renameme <charname> <password>
```

```ini
Изменяет имя на персонаже, на любой базе и за деньги, пароль нужен за смену имени на специальный тег в нём, чтобы стать частью фракции игроков
```

> ---

```bash
/movechar <charname> <code>
```

```ini
Переместите персонажа из удаленной учетной записи в эту.
```

> ---

```bash
/set movecharcode <code>
```

```ini
Если <code> равен none то код удаляется и перемещение возможно без кода, в ином случае ставится код перемещения персонажа
```

> ---

```bash
/set movecharcode <code>
```

```ini
Если <code> равен none то код удаляется и перемещение возможно без кода, в ином случае ставится код перемещения персонажа
```

> ---

```bash
/restart <faction>
```

```ini
Перезапускает игру за другую фракцию
```

> ---

```bash
/showrestarts
/showrestarts <private_code>
```

```ini
Показывает список доступных рестартов персонажа
```

> ---

```bash
/sendcash <charname> <cash> <anon> <comment>
/givecash <charname> <cash> <anon> <comment>
/gc <charname> <cash> <anon> <comment>
```

```ini
Дарит персонажу ваши деньги в количестве <cash>, можно анонимно введя /givecash Admin 1000000000 anon Подарок
```

> ---

```bash
/givecasht <cash> <anon> <comment>
/gct <cash> <anon> <comment>
```

```ini
Дарит выбранному мышкой персонажу ваши деньги в количестве <cash>, можно анонимно введя /givecash Admin 1000000000 anon Подарок
```

> ---

```bash
/drawcash <charname> <code> <cash>
/drc <charname> <code> <cash>
```

```ini
Дарит персонажу ваши деньги в количестве <cash>, можно анонимно введя /givecash Admin 123(если код есть) 1000000000
```

> ---

```bash
/set cashcode <code>
```

```ini
Устанавливает код для вашего кошелька, то есть игрок используя ваш код может смотреть ваш баланс командой /showcash <charname> <code>
```

> ---

```bash
/showcash <charname> <code>
/shc <charname> <code>
```

```ini
Игрок смотрит ваш баланс командой /showcash используя код который вы ему предоставили и установили на кошелёк
```

> ---

```bash
/group <message>
/g <message>
```

```ini
Отправьте сообщение в групповой чат
```

> ---

```bash
/local <message>
/l <message>
```

```ini
Отправьте сообщение в локальный системный чат.
```

> ---

```bash
/system <message>
/s <message>
```

```ini
Отправить сообщение в системный чат.
```

> ---

```bash
/invite <charname>
/i <charname>
```

```ini
Приглашает пользователя в группу
```

> ---

```bash
/factioninvite <tag>
/fm <tag>
```

```ini
Отправьте сообщение с приглашением фракции всем игрокам с определенным префиксом.
```

> ---

```bash
/setmsg <n> <msg_text>
```

```ini
Установка предустановленного сообщения, вместо n устанавливается его порядковый номер вместо text текст сообщения
/setmsg 1 Всем привет и приятного дня!
```

> ---

```bash
/showmsgs
```

```ini
Показать предустановленные сообщения
```

> ---

```bash
/showmsgs
```

```ini
Показать предустановленные сообщения
```

> ---

```bash
/1-9
/l1-9
```

```ini
Отправьте предустановленное сообщение в локальный системный чат от 1 до 9 включительно
```

> ---

```bash
/s1-9
```

```ini
Отправка предустановленного сообщения в системный чат от 1 до 9 включительно
```

> ---

```bash
/g1-9
```

```ini
Отправка предустановленного сообщения в групповой чат от 1 до 9 включительно
```

> ---

```bash
/t1-9
```

```ini
Отправка сообщения последнему/текущему целевому объекту.
```

> ---

```bash
/target <message>
/t <message>
```

```ini
Отправка сообщения последнему/текущему целевому объекту.
```

> ---

```bash
/reply <message>
/r <message>
```

```ini
Отправить сообщение последнему человеку, который отправил сообщение вам.
```

> ---

```bash
/privatemsg <charname> <messsage>
/pm <charname> <messsage>
/privatemsg$ <clientid> <messsage>
/pm$ <clientid> <messsage>
```

```ini
Отправить личное сообщение на указанный clientid и charname
```

> ---

```bash
/factionmsg <tag> <message>
/fm <tag> <message>
```

```ini
Отправьте сообщение всем игрокам с определенным префиксом.
```

> ---

```bash
/set chattime <on|off>
```

```ini
Добавить вывод времени сообщения отправленного в чате и позволяет это выключить у себя каждому игроку
```

> ---

```bash
/set dietime <on|off>
```

```ini
Добавить вывод времени сообщения о смерти и позволяет это выключить у себя каждому игроку
```

> ---

```bash
/mail <msgnum>
```

```ini
Показать почту
```

> ---

```bash
/maildel <msgnum>
```

```ini
Удалить почту
```

> ---

```bash
/si me
/si <target>
/showinfo
/showinfo*
/showinfo <target>
/showinfo* <target>
```

```ini
Показать личную карточку игрока которую он сформировал командой /setinfo
```

> ---

```bash
/setinfo <n> <command> <msg>
```

```ini
<n> от 1 до 5 включительно
<command> a или d - a:устанавливает информацию, d:удаляет информацию из указанного номера параграфа <n>
<msg> только при command=a - устанавливает текст параграфа вашей карточки
Установить данные личной карточки игрока (выключено)
```

> ---

```bash
/time
/time*
```

```ini
Напечатать время
```

> ---

```bash
/lights
/lights*
```

```ini
Включить маяки корабля
```

> ---

```bash
/selfdestruct
/selfdestruct*
```

```ini
Самоуничтожение корабля
```

> ---

```bash
/shields
/shields*
```

```ini
Включение выключение щита корабля
```

> ---

```bash
/survey
```

```ini
Что-то иссследует, похоже на солнце которое спаунит сервак по заданию (выключено)
```

> ---

```bash
/showcoords <n>
```

```ini
Показать предустановленные координаты (1-9)
```

> ---

```bash
/savecoords <n> <text>
```

```ini
Установка предустановленной координаты (1-9)
```

> ---

```bash
/c1-9
```

```ini
Загрузка предустановленной координаты от 1 до 9 включительно
```

> ---

```bash
/setcoords
```

```ini
Установить прыжковому двигателю координаты (выключено)
```

> ---

```bash
/jump
/jump*
```

```ini
Прыгнуть прыжковым двигателем (выключено)
```

> ---

```bash
/jump
/jump*
```

```ini
Прыгнуть прыжковым двигателем (выключено)
```

> ---

```bash
/beacon
/beacon*
```

```ini
Команды гиперпространственного маяка (выключено)
```

> ---

```bash
/jumpbeacon
/jumpbeacon*
```

```ini
Команды гиперпространственного маяка (выключено)
```

> ---

```bash
/charge
/charge*
```

```ini
Команды прыжкового двигателя (выключено)
```

> ---

```bash
/jumpsys
```

```ini
Анализ систем куда можно прыгнуть (выключено)
```

> ---

```bash
/showscan
/showscan$
/scan <charname>
/scanid <clientid>
```

```ini
Анализ систем куда можно прыгнуть (выключено)
```

> ---

```bash
/maketag <tag> <master password> <description>
```

```ini
Создание тега фракции со своим паролем и описанием
```

> ---

```bash
/droptag <tag> <master_password>
```

```ini
Удаление тега фракции
```

> ---

```bash
/settagpass <tag> <master_password> <rename_password>
```

```ini
Смена пароля тега фракции
```

> ---

TODO: anticheat, base, dynamic_mission, dsacesrv, dock

> **26.11.2020 23:26 - по настоящее время**
