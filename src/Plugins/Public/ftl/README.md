### FTL

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
> - Plugin loaded: ftl **(ftl.dll)**
>
>   Description:

```ini
Adds teleportation devices of different levels and special docking restrictions based on cargo presence
```

> ---

> 1. **accpath + \flhookuser.ini**
> 2. [Example config `ftl.ini`](../../../Binaries/bin-conf/flhook_plugins/ftl.ini)
> 3. [Example config `ftlbadsystems.ini`](../../../Binaries/bin-conf/flhook_plugins/ftlbadsystems.ini)

> ---

> [!NOTE]
> Function:

```bash
UserCmd_FTL
```

> [!WARNING]
> Command:

```bash
/ftl
```

> [!CAUTION]
> Usage:

```bash
/ftl
/ftl [x] [y] [z]
```

> [!IMPORTANT]
> Description:

```ini
Teleports with an offset to the selected object using specified coordinates.
Example: /ftl 100 -100 -100 → teleports 100 right, 100 down, 100 backward relative to the selected object
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_SFTL
```

> [!WARNING]
> Command:

```bash
/sftl
```

> [!CAUTION]
> Usage:

```bash
/sftl [x] [y] [z]
```

> [!IMPORTANT]
> Description:

```ini
Teleports to the specified coordinates
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_JUMP
```

> [!WARNING]
> Command:

```bash
/j
```

> [!CAUTION]
> Usage:

```bash
/j
```

> [!IMPORTANT]
> Description:

```ini
Teleports to the selected object
```

> ---

> [!NOTE]
> Function:

```bash
UserCmd_COORDS
```

> [!WARNING]
> Command:

```bash
/coords
```

> [!CAUTION]
> Usage:

```bash
/coords
```

> [!IMPORTANT]
> Description:

```ini
Provides target coordinates for use with /ftl or /sftl commands
```

> ---
