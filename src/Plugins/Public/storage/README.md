### 🔑 STORAGE 🔑

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
> - Plugin loaded: store **(store.dll)**
>
>   Description

```ini
Manages item storage across system warehouses
```

> ---

> 1. [Example configuration `ErrorStore.ini`](../../../Binaries/bin-conf/flhook_plugins/ErrorStore.ini)

> ---

> [!NOTE]
> Function:

```bash
UserCmd_Help
```

> [!WARNING]
> Command:

```bash
/shelp
```

> [!CAUTION]
> Usage:

```bash
/shelp
```

> [!IMPORTANT]
> Description:

```ini
Displays the full list of warehouse-related commands
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_Store
```

> [!WARNING]
> Command:

```bash
/store
```

> [!CAUTION]
> Usage:

```bash
/store [n] [amount]
/store all
```

> [!IMPORTANT]
> Description:

```ini
Sends cargo to storage. <n> is the cargo slot index in the ship (use /enumcargo to view)
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_Ustore
```

> [!WARNING]
> Command:

```bash
/unstore
```

> [!CAUTION]
> Usage:

```bash
/unstore [n] [amount]
/unstore all
```

> [!IMPORTANT]
> Description:

```ini
Transfers stored cargo back to the ship
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_Istore
```

> [!WARNING]
> Command:

```bash
/sinfo
```

> [!CAUTION]
> Usage:

```bash
/sinfo
```

> [!IMPORTANT]
> Description:

```ini
Displays the number of items stored in the current system warehouse
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_EnumCargo
```

> [!WARNING]
> Command:

```bash
/enumcargo
```

> [!CAUTION]
> Usage:

```bash
/enumcargo
```

> [!IMPORTANT]
> Description:

```ini
Displays the contents of your ship cargo hold
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_CargoSystems
```

> [!WARNING]
> Command:

```bash
/cargosystems
/carsys
```

> [!CAUTION]
> Usage:

```bash
/cargosystems
/carsys
```

> [!IMPORTANT]
> Description:

```ini
Displays a list of systems where you have stored cargo, along with quantities
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_CargoSystems
```

> [!WARNING]
> Command:

```bash
/searchgood
/sh
```

> [!CAUTION]
> Usage:

```bash
/searchgood [searchstring]
/sh [searchstring]
```

> [!IMPORTANT]
> Description:

```ini
Searches for items in your warehouses across the galaxy (supports partial matches)
```

> ---
