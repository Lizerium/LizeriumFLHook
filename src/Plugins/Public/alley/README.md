### 🌲 Alley 🌲

- [Back](../../../../README.md)

<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./README.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

---

> [!IMPORTANT]
>
> - Plugin loaded: alley **(alley.dll)**
>
> Description:

```ini
Cargo jettison, police commands, pirate faction assignment, admin teleport to player,
group target marking, race start sound, docking restrictions, self-money gift system
```

> ---
>
> ## config:
>
> 1. [Example config `alley_permissions.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_permissions.cfg)

> ---
>
> 2. **misc_equip.ini** _(from ..\data\equipment\)_

> ---
>
> 3. [Example config `alley_rep.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_rep.cfg)

> ---
>
> 4. [Example config `alley.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley.cfg)

> ---
>
> 5. [Example config `alley_restrictions.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_restrictions.cfg)

> ---
>
> 6. **healingrates.cfg** _(from \..\DATA\EQUIPMENT\)_
>    [Example config `healingrates.cfg`](../../../Binaries/bin-conf/flhook_plugins/healingrates.cfg)

> ---
>
> 7. [Example config `alley_shipclassitems.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_shipclassitems.cfg)

> ---
>
> log:

1. ./flhook_logs/nodockcommand.log
2. ./flhook_logs/alley_gifts.log
3. ./flhook_logs/generatedids.log

---

> [!NOTE]
> Function:

```bash
NoDockCommand
```

> [!WARNING]
> Command:

```bash
/nodock
/nodock*
```

> [!CAUTION]
> Usage:

```bash
/nodock
/nodock*
```

> [!IMPORTANT]
> Description:

```ini
A player with the appropriate license can revoke docking permissions from another player by targeting them in space.
```

---

> [!NOTE]
> Function:

```sh
PoliceCmd
```

> [!WARNING]
> Command:

```sh
/police
/police*
```

> [!CAUTION]
> Usage:

```bash
/police
/police*
```

> [!IMPORTANT]
> Description:

```ini
A player with a police license can enable/disable the police alert system with siren.
```

---

> [!NOTE]
> Function:

```sh
PirateCmd
```

> [!WARNING]
> Command:

```sh
/pirate
/pirate*
```

> [!CAUTION]
> Usage:

```bash
/pirate
/pirate*
```

> [!IMPORTANT]
> Description:

```ini
Assigns you to the pirate faction.
```

---

> [!NOTE]
> Function:

```sh
RacestartCmd
```

> [!WARNING]
> Command:

```sh
/racestart
```

> [!CAUTION]
> Usage:

```bash
/racestart
```

> [!IMPORTANT]
> Description:

```ini
Players with the required license can trigger a race start sound for all players within a 5000 range.
```

---

> [!NOTE]
> Function:

```sh
GiftCmd
```

> [!WARNING]
> Command:

```sh
/gift
/gift*
```

> [!CAUTION]
> Usage:

```bash
/gift
/gift*
```

> [!IMPORTANT]
> Description:

```ini
Grants money to the player (self-reward system).
```

---

> [!NOTE]
> Function:

```sh
AlleyCmd_Help
```

> [!WARNING]
> Command:

```sh
$help
$help*
```

> [!CAUTION]
> Usage:

```bash
$help
$help*
```

> [!IMPORTANT]
> Description:

```ini
Displays help information for commands.
```

---

> [!NOTE]
> Function:

```sh
AlleyCmd_Chase
```

> [!WARNING]
> Command:

```sh
$chase
$chase*
```

> [!CAUTION]
> Usage:

```bash
$chase <charname>
$chase* <charname>
```

> [!IMPORTANT]
> Description:

```ini
Teleports you to the specified player via Bob. Available only for administrators with platinum level.
```

---

> [!NOTE]
> Function:

```sh
UserCmd_MarkObjGroup
```

> [!WARNING]
> Command:

```sh
/marktarget
/marktarget*
```

> [!CAUTION]
> Usage:

```bash
/marktarget
/marktarget*
```

> [!IMPORTANT]
> Description:

```ini
Marks the selected target for the entire group.
```

---

> [!NOTE]
> Function:

```sh
UserCmd_JettisonAll
```

> [!WARNING]
> Command:

```sh
/jettisonall
```

> [!CAUTION]
> Usage:

```bash
/jettisonall
```

> [!IMPORTANT]
> Description:

```ini
Jettisons all cargo into space.
```

---
