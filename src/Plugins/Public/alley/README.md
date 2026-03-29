### 🌲 Alley 🌲

> ---
>
> [!IMPORTANT]
>
> - Plugin loaded: alley **(alley.dll)**
>
>   Описание:

```ini
Сброс всего груза, команды для полиции, присвоение себя к пиратам, телепорт админа к игроку,
отметитка цели для всей группыб звук старта гонки, запрет стыковки, подарок себе в виде денег
```

> ---
>
> ## config:
>
> 1.  [Пример конфигурации `alley_permissions.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_permissions.cfg)

> ---
>
> 3.  **misc_equip.ini** _(из ..\\data\\equipment\\)_
>
> ---
>
> 4.  [Пример конфигурации `alley_rep.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_rep.cfg)

> ---
>
> 5.  [Пример конфигурации `alley.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley.cfg)

> ---
>
> 6.  [Пример конфигурации `alley_restrictions.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_restrictions.cfg)

> ---
>
> 7.  **healingrates.cfg** _(из \\..\\DATA\\EQUIPMENT\\)_ - [Пример конфигурации `healingrates.cfg`](../../../Binaries/bin-conf/flhook_plugins/healingrates.cfg)

> ---
>
> 8.  [Пример конфигурации `alley_shipclassitems.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_shipclassitems.cfg)

> ---
>
> log:

1. ./flhook_logs/nodockcommand.log
2. ./flhook_logs/alley_gifts.log
3. ./flhook_logs/generatedids.log

> ---

> [!NOTE]
> Функция:

```bash
NoDockCommand
```

> ---
>
> [!WARNING]
> Команда:

```bash
/nodock
/nodock*
```

> [!CAUTION]
> Применение:

```bash
/nodock
/nodock*
```

> [!IMPORTANT]
> Описание:

```ini
Игрок с соответствующим удостоверением может удалить разрешения на стыковку у другого игрока выделив его как цель в космосе
```

> ---

> [!NOTE]
> Функция:

```sh
PoliceCmd
```

> ---
>
> [!WARNING]
> Команда:

```sh
/police
/police*
```

> [!CAUTION]
> Применение:

```bash
/police
/police*
```

> [!IMPORTANT]
> Описание:

```ini
Игрок с удостоверением полиции может командой включить/выключить полицейскую систему оповещения сиреной
```

> ---

> [!NOTE]
> Функция:

```sh
PirateCmd
```

> ---
>
> [!WARNING]
> Команда:

```sh
/pirate
/pirate*
```

> [!CAUTION]
> Применение:

```bash
/pirate
/pirate*
```

> [!IMPORTANT]
> Описание:

```ini
Присваивает вас к фракции пираты
```

> ---

> [!NOTE]
> Функция:

```sh
RacestartCmd
```

> ---
>
> [!WARNING]
> Команда:

```sh
/racestart
```

> [!CAUTION]
> Применение:

```bash
/racestart
```

> [!IMPORTANT]
> Описание:

```ini
Все кто имеет допустимую лицензию можно начинать отправлять звук старта гонки для игроков в пределах 5000 при вводе команды
```

> ---

> [!NOTE]
> Функция:

```sh
GiftCmd
```

> ---
>
> [!WARNING]
> Команда:

```sh
/gift
/gift*
```

> [!CAUTION]
> Применение:

```bash
/gift
/gift*
```

> [!IMPORTANT]
> Описание:

```ini
Дарит по идее вам деньги
```

> ---

> [!NOTE]
> Функция:

```sh
AlleyCmd_Help
```

> ---
>
> [!WARNING]
> Команда:

```sh
$help
$help
```

> [!CAUTION]
> Применение:

```bash
$help
$help*
```

> [!IMPORTANT]
> Описание:

```ini
Выводит справку по команде
```

> ---

> [!NOTE]
> Функция:

```sh
AlleyCmd_Chase
```

> ---
>
> [!WARNING]
> Команда:

```sh
$chase
$chase*
```

> [!CAUTION]
> Применение:

```bash
$chase <charname>
$chase* <charname>
```

> [!IMPORTANT]
> Описание:

```ini
Телепортирует вас через Боба к нужному игроку. Только для администратора имеющего платиновый уровень.
```

> ---

> [!NOTE]
> Функция:

```sh
UserCmd_MarkObjGroup
```

> ---
>
> [!WARNING]
> Команда:

```sh
/marktarget
/marktarget*
```

> [!CAUTION]
> Применение:

```bash
/marktarget
/marktarget*
```

> [!IMPORTANT]
> Описание:

```ini
Маркирует цель для всей группы
```

> ---

> [!NOTE]
> Функция:

```sh
UserCmd_JettisonAll
```

> ---
>
> [!WARNING]
> Команда:

```sh
/jettisonall
```

> [!CAUTION]
> Применение:

```bash
/jettisonall
```

> [!IMPORTANT]
> Описание:

```ini
Сбрасывает весь груз в космос
```

> ---
