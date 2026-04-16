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

> [!NOTE]
> This project is part of the **Lizerium** ecosystem and belongs to:
>
> - [`Lizerium.Software.Structs`](https://github.com/Lizerium/Lizerium.Software.Structs)
>
> If you are looking for related engineering and utility tools, start there.

> **26.11.2020 23:26 — present**

# ⏬Description⏬

FLHook (**_8.0.0_**)

> ## Version: **Dvurechensky**
>
> ## Game: **Freelancer Lizerium**

This is originally a private **FLHook** repository from **Freelancer Lizerium**.

It is a **hybrid of multiple repositories** from various old/dead Freelancer servers.  
It includes a **client hook** and a large number of plugins built on top of it.

---

> [!WARNING]
> This project has been successfully tested on the  
> [Lizerium](https://lizup.ru/) server by me — [Dvurechensky](https://www.dvurechensky.pro/)

> [!CAUTION]
> If you are seeing this project publicly, it means I decided to share not only this repository,  
> but the entire **[Lizerium](https://lizup.ru/)** ecosystem.

---

## 📚 Documentation

- [FLHook Plugin Development](docs/PLUGIN_DOCS/Разработка%20плагинов%20для%20FLHook.md)
- [Available Hooks — FLHook Hooks](docs/PLUGIN_DOCS/Available%20Hooks%20—%20Хуки%20FLHook.md)
- [FLHook Plugin How-To](docs/PLUGIN_DOCS/Plugin%20How-To%20для%20FLHook.md)
- [SDK Files & Inter-Plugin Communication](docs/PLUGIN_DOCS/SDK%20Files%20%26%20Inter-Plugin%20Communication%20для%20FLHook.md)
- [FLHook Plugin Troubleshooting](docs/PLUGIN_DOCS/Troubleshooting%20для%20FLHook%20Plugins.md)
- [Boost](docs/BOOST_DEPENDENCIES.md)

---

## 🈴 IMPORTANT 🈴

> No one helped me with this. Everyone either refused or ignored it.

---

## ⚙️ Requirements / Environment

- Configured for **Visual Studio 2022 Professional**
- **Windows SDK 8.1**
- Compiler: **VC141**
- ![MFC](src/Media/MFC.png)

---

## 📳 Usage 📳

- Do NOT use the solution in **DEBUG** configuration
- **Always use:**  
  👉 **RELEASE** 🌱 **WIN32**

- Compiled plugins are located in:

```text
Binaries/bin-vc14/flhook_plugins
```

> ⚠️ Some plugins may also be located inside their respective `Release` folders

---

## 📳 Specification 📳

### 📳 Table of Contents 📳

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
29. [MINE CONTROL V1](#MINE-CONTROL-V1) | [README](src/Plugins/Public/minecontrol_plugin/README.md)
30. [MISC COMMANDS](#MISC-COMMANDS) | [README](src/Plugins/Public/MiscellaneousCommands/README.md)
31. [NPC CONTROL](#NPC-CONTROL) | [README](src/Plugins/Public/npc_control/README.md)
32. [REG ARMOUR](#REG-ARMOUR) | [README](src/Plugins/Public/reg_armour/README.md)
33. [TEMP BAN](#TEMP-BAN) | [README](src/Plugins/Public/tempban/README.md)
34. [TEMPLATE](#TEMPLATE) | [README](src/Plugins/Public/__PLUGIN_TEMPLATE/README.md)
35. [PVE CONTROLLER](#PVE-CONTROLLER) | [README](src/Plugins/Public/pvecontroller/README.md)
36. [STORAGE](#STORAGE) | [README](src/Plugins/Public/storage/README.md)
37. [PLAYER CONTROLLER](#PLAYER-CONTROLLER) | [README](src/Plugins/Public/playercntl_plugin/README.md)

---

> **26.11.2020 23:26 — present**
