### 🌲 BASE 🌲

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
> - Plugin loaded: base **(base.dll)**
>
>   Description:

```ini
Adds player-built bases with their own banking system and permission management
```

> ---

> 1. [Example config `base.cfg`](../../../Binaries/bin-conf/flhook_plugins/base.cfg)
> 2. [Example config `base_recipe_items.cfg`](../../../Binaries/bin-conf/flhook_plugins/base_recipe_items.cfg)
> 3. [Example config `base_recipe_modules.cfg`](../../../Binaries/bin-conf/flhook_plugins/base_recipe_modules.cfg)
> 4. [Example config `base_archtypes.cfg`](../../../Binaries/bin-conf/flhook_plugins/base_archtypes.cfg)

> ---

> [!NOTE]
> Function:

```bash
BaseHelp
```

> [!WARNING]
> Command:

```bash
/base
```

> [!CAUTION]
> Usage:

```bash
/base help [page]
```

> [!IMPORTANT]
> Description:

```ini
Displays the help page. Specify a page number to navigate further.
```

> ---

> [!NOTE]
> Function:

```bash
BaseLogin
```

> [!WARNING]
> Command:

```bash
/base login
```

> [!CAUTION]
> Usage:

```bash
/base login [password]
```

> [!IMPORTANT]
> Description:

```ini
Logs in as a base administrator.
Additional commands become available after authentication.
```

> ---

> [!NOTE]
> Function:

```bash
BaseAddPwd
```

> [!WARNING]
> Command:

```bash
/base addpwd
```

> [!CAUTION]
> Usage:

```bash
/base addpwd [password]
/base addpwd [password] [viewshop]
```

> [!IMPORTANT]
> Description:

```ini
Adds administrator passwords for the base.
Use "viewshop" to allow shop viewing access only.
```

> ---

> [!NOTE]
> Function:

```bash
BaseRmPwd
```

> [!WARNING]
> Command:

```bash
/base rmpwd
```

> [!CAUTION]
> Usage:

```bash
/base rmpwd [password]
```

> [!IMPORTANT]
> Description:

```ini
Removes administrator passwords for the base.
```

> ---

> [!NOTE]
> Function:

```bash
BaseLstPwd
```

> [!WARNING]
> Command:

```bash
/base lstpwd
```

> [!CAUTION]
> Usage:

```bash
/base lstpwd
```

> [!IMPORTANT]
> Description:

```ini
Lists all administrator passwords for the base.
```

> ---

> [!NOTE]
> Function:

```bash
BaseSetMasterPwd
```

> [!WARNING]
> Command:

```bash
/base setmasterpwd
```

> [!CAUTION]
> Usage:

```bash
/base setmasterpwd [old password] [new password]
```

> [!IMPORTANT]
> Description:

```ini
Sets the master password for the base.
```

> ---

> [!NOTE]
> Function:

```bash
BaseAddAllyTag / BaseRmHostileTag
```

> [!WARNING]
> Command:

```bash
/base addtag
```

> [!CAUTION]
> Usage:

```bash
/base addtag [tag]
```

> [!IMPORTANT]
> Description:

```ini
Adds an allied tag to the base.
```

> ---

> [!NOTE]
> Function:

```bash
BaseRmAllyTag
```

> [!WARNING]
> Command:

```bash
/base rmtag
```

> [!CAUTION]
> Usage:

```bash
/base rmtag [tag]
```

> [!IMPORTANT]
> Description:

```ini
Removes an allied tag from the base.
```

> ---

> [!NOTE]
> Function:

```bash
BaseLstAllyTag
```

> [!WARNING]
> Command:

```bash
/base lsttag
```

> [!CAUTION]
> Usage:

```bash
/base lsttag
```

> [!IMPORTANT]
> Description:

```ini
Lists all allied tags of the base.
```

> ---

> [!NOTE]
> Function:

```bash
BaseAddHostileTag / BaseRmAllyTag
```

> [!WARNING]
> Command:

```bash
/base addhostile
```

> [!CAUTION]
> Usage:

```bash
/base addhostile [tag]
```

> [!IMPORTANT]
> Description:

```ini
Adds a tag to the base blacklist.
Blacklisted targets will be attacked automatically.
Use full tags like "=LSF=", "IMG|" or shipname patterns.
```

> ---

> [!NOTE]
> Function:

```bash
BaseRmHostileTag
```

> [!WARNING]
> Command:

```bash
/base rmhostile
```

> [!CAUTION]
> Usage:

```bash
/base rmhostile [tag]
```

> [!IMPORTANT]
> Description:

```ini
Removes a tag from the base blacklist.
```

> ---

> [!NOTE]
> Function:

```bash
BaseLstHostileTag
```

> [!WARNING]
> Command:

```bash
/base lsthostile
```

> [!CAUTION]
> Usage:

```bash
/base lsthostile
```

> [!IMPORTANT]
> Description:

```ini
Lists all blacklisted tags for the base.
```

> ---

> [!NOTE]
> Function:

```bash
BaseRep
```

> [!WARNING]
> Command:

```bash
/base rep
```

> [!CAUTION]
> Usage:

```bash
/base rep
/base rep clear
```

> [!IMPORTANT]
> Description:

```ini
Sets or clears the faction affiliation of the base.
Faction is determined by the player executing the command.
```

> ---

> [!NOTE]
> Function:

```bash
BaseDefenseMode
```

> [!WARNING]
> Command:

```bash
/base defensemode
```

> [!CAUTION]
> Usage:

```bash
/base defensemode
```

> [!IMPORTANT]
> Description:

```ini
Controls base defense modes.

Mode 1: Blacklist > Whitelist > IFF
Docking: whitelist only

Mode 2: Blacklist > Whitelist > IFF
Docking: good reputation allowed

Mode 3: Blacklist > Whitelist > Hostile
Docking: whitelist only

Mode 4: Blacklist > Whitelist > Neutral
Docking: good reputation allowed

Mode 5: Blacklist > Whitelist > Neutral
Docking: whitelist only
```

> ---

> [!NOTE]
> Function:

```bash
BaseDeploy
```

> [!WARNING]
> Command:

```bash
/base deploy
```

> [!CAUTION]
> Usage:

```bash
/base deploy <password> <basename>
```

> [!IMPORTANT]
> Description:

```ini
Deploys a base if requirements are met:
repair ship, full stop, and required cargo.
```

> ---

> [!NOTE]
> Function:

```bash
Shop
```

> [!WARNING]
> Command:

```bash
/shop price
```

> [!CAUTION]
> Usage:

```bash
/shop price [item] [price] [min stock] [max stock]
```

> [!IMPORTANT]
> Description:

```ini
Sets item price and stock limits.
Controls buying/selling availability based on stock levels.
```

> ---

> [!NOTE]
> Function:

```bash
Shop
```

> [!WARNING]
> Command:

```bash
/shop remove
```

> [!CAUTION]
> Usage:

```bash
/shop remove [item]
```

> [!IMPORTANT]
> Description:

```ini
Removes item from shop stock.
```

> ---

> [!NOTE]
> Function:

```bash
Shop
```

> [!WARNING]
> Command:

```bash
/shop
```

> [!CAUTION]
> Usage:

```bash
/shop [page]
```

> [!IMPORTANT]
> Description:

```ini
Displays shop inventory (up to 40 items per page).
```

> ---

> [!NOTE]
> Function:

```bash
Bank
```

> [!WARNING]
> Command:

```bash
/basebank
```

> [!CAUTION]
> Usage:

```bash
/basebank withdraw [credits]
```

> [!IMPORTANT]
> Description:

```ini
Withdraws credits from base bank.
```

> ---

> [!NOTE]
> Function:

```bash
Bank
```

> [!WARNING]
> Command:

```bash
/basebank
```

> [!CAUTION]
> Usage:

```bash
/basebank deposit [credits]
```

> [!IMPORTANT]
> Description:

```ini
Deposits credits into base bank.
```

> ---

> [!NOTE]
> Function:

```bash
Bank
```

> [!WARNING]
> Command:

```bash
/basebank
```

> [!CAUTION]
> Usage:

```bash
/basebank status
```

> [!IMPORTANT]
> Description:

```ini
Shows base bank balance.
```

> ---

> [!NOTE]
> Function:

```bash
BaseInfo
```

> [!WARNING]
> Command:

```bash
/base info
```

> [!CAUTION]
> Usage:

```bash
/base info
```

> [!IMPORTANT]
> Description:

```ini
Sets base infocard description.
```

> ---

> [!NOTE]
> Function:

```bash
BaseFacMod
```

> [!WARNING]
> Command:

```bash
/base facmod
```

> [!CAUTION]
> Usage:

```bash
/base facmod
```

> [!IMPORTANT]
> Description:

```ini
Manages factory modules.
```

> ---

> [!NOTE]
> Function:

```bash
BaseDefMod
```

> [!WARNING]
> Command:

```bash
/base defmod
```

> [!CAUTION]
> Usage:

```bash
/base defmod
```

> [!IMPORTANT]
> Description:

```ini
Manages defense modules.
```

> ---

> [!NOTE]
> Function:

```bash
BaseShieldMod
```

> [!WARNING]
> Command:

```bash
/base shieldmod
```

> [!CAUTION]
> Usage:

```bash
/base shieldmod
```

> [!IMPORTANT]
> Description:

```ini
Manages shield modules.
```

> ---

> [!NOTE]
> Function:

```bash
BaseBuildMod
```

> [!WARNING]
> Command:

```bash
/base buildmod
```

> [!CAUTION]
> Usage:

```bash
/base buildmod
```

> [!IMPORTANT]
> Description:

```ini
Controls construction and destruction of base modules and upgrades.
```

> ---
