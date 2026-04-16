### 🌲 AUTOBUY 🌲

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

> ---
>
> ## Plugin loaded: autobuy **(autobuy.dll)**
>
> Description:

```ini
Adds automatic restocking of consumables
```

> ---

> 1. **..\data\equipment\misc_equip.ini**
> 2. **..\data\equipment\weapon_equip.ini**
> 3. [Example config `autobuy.cfg`](../../../Binaries/bin-conf/flhook_plugins/autobuy.cfg)

> ---

> [!NOTE]
> Function:

```bash
UserCmd_AutoBuy
```

> [!WARNING]
> Command:

```bash
/autobuy
/autobuy*
```

> [!CAUTION]
> Usage:

```bash
/autobuy <param> [<on/off>]

/autobuy info           - show autobuy information
/autobuy missiles on/off - enable/disable missile autobuy
/autobuy torps on/off    - enable/disable torpedo autobuy
/autobuy mines on/off    - enable/disable mine autobuy
/autobuy cd on/off       - enable/disable cruise disruptor autobuy
/autobuy cm on/off       - enable/disable countermeasure autobuy
/autobuy reload on/off   - enable/disable nanobot & shield battery autobuy
/autobuy all on/off      - enable/disable all consumables autobuy

Examples:
/autobuy info        - show autobuy status
/autobuy all on      - enable autobuy for all consumables
/autobuy torps off   - disable torpedo autobuy
```

> [!IMPORTANT]
> Description:

```ini
Controls automatic purchasing of consumables.
```

> ---
