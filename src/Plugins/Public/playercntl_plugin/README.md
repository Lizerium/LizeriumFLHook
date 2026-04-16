# 💳 PLAYER CONTROLLER 💳

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

## ⛄ Table of Contents

- [💳 PLAYER CONTROLLER 💳](#-player-controller-)
  - [⛄ Table of Contents](#-table-of-contents)
    - [🌁 Installation](#-installation)
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
      - [🌀 `maildel`](#-maildel)
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
>   Description:

```ini
Provides a wide range of gameplay and administrative functions
```

1. [Example configuration `playercntl.ini`](../../../Binaries/bin-conf/flhook_plugins/playercntl.ini)
2. [Example configuration `jump_allowedsystems.cfg`](../../../Binaries/bin-conf/flhook_plugins/jump_allowedsystems.cfg)
3. [Example configuration `jump.cfg`](../../../Binaries/bin-conf/flhook_plugins/jump.cfg)
4. [Example configuration `alley_lockedships.cfg`](../../../Binaries/bin-conf/flhook_plugins/alley_lockedships.cfg)

---

### 🌁 Installation

1. Copy the file:

```text
playercntl.dll
```

to:

```text
flhook_plugins\
```

2. Copy the file:

```text
playercntl.ini
```

to:

```text
flhook_plugins\
```

3. In the FLHook console, run the command if you changed `playercntl.ini`:

```text
rehash
```

> to reload the configuration.

---

### 🌁 ADMIN COMMANDS

#### 🌀 `smiteall`

> [!WARNING]
> Command:

```bash
smiteall
```

> [!CAUTION]
> Usage:

```bash
smiteall
```

> [!IMPORTANT]
> Description:

```ini
Strikes all players within 15k radar range (destroys everyone)
```

---

#### 🌀 `bob`

```bash
bob <charname>
```

```ini
Marks a player as wanted
```

---

#### 🌀 `playmusic`

```bash
playmusic <Musicname>
```

```ini
Plays music for players within a 50k radius
```

---

#### 🌀 `playsound`

```bash
playsound <Soundname>
```

```ini
Plays a sound for players within a 50k radius
```

---

#### 🌀 `playnnm`

```bash
playnnm <Soundname>
```

```ini
Plays a sound(?) for players within a 50k radius
```

---

#### 🌀 `beam`

```bash
beam <charname> <basename>
```

```ini
Teleports the admin to a base. Works across systems but pathfinding requires improvement
```

---

#### 🌀 `pull`

```bash
pull <charname>
```

```ini
Pulls a player to you. Works across systems but pathfinding requires improvement
```

---

#### 🌀 `move`

```bash
move <x> <y> <z>
```

```ini
Moves to specified coordinates
```

---

#### 🌀 `chase`

```bash
chase <charname>
```

```ini
Chases a player (teleports to them). Works across systems but pathfinding requires improvement
```

---

#### 🌀 `lrs`

```bash
lrs
```

```ini
Lists ships that are restricted from jumping
```

---

#### 🌀 `makecoord`

```bash
makecoord
```

```ini
Outputs your current coordinates
```

---

#### 🌀 `authchar`

```bash
authchar <charname>
```

```ini
Checks if a character is online and authenticated
```

---

#### 🌀 `reloadbans`

```bash
reloadbans
```

```ini
Reloads IP and ID ban lists
```

---

#### 🌀 `setaccmovecode`

```bash
setaccmovecode <charname> <code>
```

```ini
Sets a move code for all characters in the account of the specified player
```

#### 🌀 `rotatelogs`

> [!WARNING]
> Command:

```bash
rotatelogs
```

> [!CAUTION]
> Usage:

```bash
rotatelogs <charname> <code>
```

> [!IMPORTANT]
> Description:

```ini
Backs up server logs (FLHook.log)
```

---

#### 🌀 `privatemsg`

> [!WARNING]
> Command:

```bash
pm
privatemsg
```

> [!CAUTION]
> Usage:

```bash
pm <charname> <message>
privatemsg <charname> <message>
```

> [!IMPORTANT]
> Description:

```ini
Sends a message that the player will receive upon login
```

---

#### 🌀 `showtags`

```bash
showtags
```

```ini
Displays a list of tags with their passwords, descriptions, and last usage
```

---

#### 🌀 `addtag`

```bash
addtag <tag> <password> <description>
```

```ini
Adds a tag with description and a personal password
```

---

#### 🌀 `droptag`

```bash
droptag <tag>
```

```ini
Removes a tag
```

---

#### 🌀 `reloadlockedships`

```bash
reloadlockedships
```

```ini
Displays a list of locked ships from alley_lockedships.cfg
```

---

### 🌁 USER COMMANDS

#### 🌀 `help`

```bash
/help
/h
/?
```

```ini
Prints custom help overriding the default help system
```

---

#### 🌀 `commandlist`

```bash
/commandlist
```

```ini
Prints a custom list of commands
```

---

#### 🌀 `pos`

```bash
/pos
```

```ini
Displays current position
```

---

#### 🌀 `stuck`

```bash
/stuck
```

```ini
Slightly moves the ship if it is stuck in a base
```

---

#### 🌀 `droprep`

```bash
/droprep
```

```ini
Removes all faction affiliations the player may have
```

---

#### 🌀 `dice`, `roll`, `coin`

```bash
/dice
/roll
/coin
```

```ini
Rolls dice / random values for RNG-based actions
```

---

#### 🌀 `pimpship`

```bash
/pimpship
```

```ini
Commands for configuring ship lighting (via dealer stations)
```

---

#### 🌀 `showsetup`

```bash
/showsetup
```

```ini
Displays ship configuration
```

---

#### 🌀 `showitems`

```bash
/showitems
```

```ini
Displays items that can be modified
```

---

#### 🌀 `setitem`

```bash
/setitem
```

```ini
Changes an item (lighting beacon) in a slot to a specified item
```

---

#### 🌀 `renameme`

```bash
/renameme <charname> <password>
```

```ini
Renames a character (on any base, for a fee). Password is required for special tag names (e.g., joining a faction)
```

#### 🌀 `movechar`

> [!NOTE]
> Function:

```bash
UserCmd_MoveChar
```

> [!WARNING]
> Command:

```bash
/movechar
```

> [!CAUTION]
> Usage:

```bash
/movechar <charname> <code>
```

> [!IMPORTANT]
> Description:

```ini
Moves a character from another account to the current one
```

---

#### 🌀 `set movecharcode`

> [!NOTE]
> Function:

```bash
UserCmd_SetMoveCharCode
```

> [!WARNING]
> Command:

```bash
/set movecharcode
```

> [!CAUTION]
> Usage:

```bash
/set movecharcode <code>
```

> [!IMPORTANT]
> Description:

```ini
If <code> is set to "none", the code is removed and transfers are allowed without it. Otherwise, a transfer code is required
```

---

#### 🌀 `restart`

```bash
/restart <faction>
```

```ini
Restarts the character under a different faction
```

---

#### 🌀 `showrestarts`

```bash
/showrestarts
/showrestarts <private_code>
```

```ini
Displays a list of available character restarts
```

---

#### 🌀 `sendcash`

```bash
/sendcash <charname> <cash> <anon> <comment>
/givecash <charname> <cash> <anon> <comment>
/gc <charname> <cash> <anon> <comment>
```

```ini
Sends money (<cash>) to another player. Can be anonymous (e.g. /givecash Admin 1000000000 anon Gift)
```

---

#### 🌀 `givecasht`

```bash
/givecasht <cash> <anon> <comment>
/gct <cash> <anon> <comment>
```

```ini
Sends money to the currently selected target (can be anonymous)
```

---

#### 🌀 `drawcash`

```bash
/drawcash <charname> <code> <cash>
/drc <charname> <code> <cash>
```

```ini
Transfers money using a code (if required)
```

---

#### 🌀 `set cashcode`

```bash
/set cashcode <code>
```

```ini
Sets a security code for your wallet. Others can view your balance using /showcash with this code
```

---

#### 🌀 `showcash`

```bash
/showcash <charname> <code>
/shc <charname> <code>
```

```ini
Allows a player to view your balance using the code you provided
```

---

#### 🌀 `group`

```bash
/group <message>
/g <message>
```

```ini
Sends a message to group chat
```

---

#### 🌀 `local`

```bash
/local <message>
/l <message>
```

```ini
Sends a message to local system chat
```

#### 🌀 `system`

> [!NOTE]
> Function:

```bash
UserCmd_SystemMsg
```

> [!WARNING]
> Command:

```bash
/system
/s
```

> [!CAUTION]
> Usage:

```bash
/system <message>
/s <message>
```

> [!IMPORTANT]
> Description:

```ini
Sends a message to system chat
```

---

#### 🌀 `invite`

```bash
/invite <charname>
/i <charname>
```

```ini
Invites a player to your group
```

---

#### 🌀 `factioninvite`

```bash
/factioninvite <tag>
/fi <tag>
```

```ini
Sends a faction invite message to all players with the specified tag prefix
```

---

#### 🌀 `setmsg`

```bash
/setmsg <n> <msg_text>
```

```ini
Sets a predefined message. Replace <n> with index and <msg_text> with the message text
Example: /setmsg 1 Hello everyone!
```

---

#### 🌀 `showmsgs`

```bash
/showmsgs
```

```ini
Displays predefined messages
```

---

#### 🌀 `ln`

```bash
/n
/ln
```

```ini
Sends a predefined message (1–9) to local chat
```

---

#### 🌀 `sn`

```bash
/sn
```

```ini
Sends a predefined message (1–9) to system chat
```

---

#### 🌀 `gn`

```bash
/gn
```

```ini
Sends a predefined message (1–9) to group chat
```

---

#### 🌀 `tn`

```bash
/tn
```

```ini
Sends a message to the last/current target
```

---

#### 🌀 `target`

```bash
/target <message>
/t <message>
```

```ini
Sends a message to the last/current target
```

---

#### 🌀 `reply`

```bash
/reply <message>
/r <message>
```

```ini
Replies to the last player who sent you a message
```

---

#### 🌀 `privatemsg`

```bash
/privatemsg <charname> <message>
/pm <charname> <message>

/privatemsg$ <clientid> <message>
/pm$ <clientid> <message>
```

```ini
Sends a private message by character name or client ID
```

---

#### 🌀 `factionmsg`

```bash
/factionmsg <tag> <message>
/fm <tag> <message>
```

```ini
Sends a message to all players with a specific tag prefix
```

---

#### 🌀 `set chattime`

```bash
/set chattime <on|off>
```

```ini
Enables/disables timestamps for chat messages (per-player setting)
```

---

#### 🌀 `set dietime`

```bash
/set dietime <on|off>
```

```ini
Enables/disables timestamps for death messages (per-player setting)
```

#### 🌀 `mail`

> [!NOTE]
> Function:

```bash
UserCmd_MailShow
```

> [!WARNING]
> Command:

```bash
/mail
```

> [!CAUTION]
> Usage:

```bash
/mail <msgnum>
```

> [!IMPORTANT]
> Description:

```ini
Show mailbox messages
```

---

#### 🌀 `maildel`

```bash
/maildel <msgnum>
```

```ini
Delete a mail message
```

---

#### 🌀 `showinfo`

```bash
/si
/showinfo
/showinfo*
```

```ini
Displays player profile information created via /setinfo
```

---

#### 🌀 `setinfo`

```bash
/setinfo <n> <command> <msg>
```

```ini
n: from 1 to 5
command:
  a = add/update info
  d = delete info
msg: text for the selected section (used only with 'a')

Sets player profile information (currently disabled)
```

---

#### 🌀 `time`

```bash
/time
/time*
```

```ini
Displays current time
```

---

#### 🌀 `lights`

```bash
/lights
/lights*
```

```ini
Turns ship lights on
```

---

#### 🌀 `selfdestruct`

```bash
/selfdestruct
/selfdestruct*
```

```ini
Self-destruct the ship
```

---

#### 🌀 `shields`

```bash
/shields
/shields*
```

```ini
Toggle ship shields on/off
```

---

#### 🌀 `survey`

```bash
/survey
```

```ini
Performs some kind of scan (likely related to mission-generated objects, currently disabled)
```

---

#### 🌀 `showcoords`

```bash
/showcoords <n>
```

```ini
Displays saved coordinates (1–9)
```

---

#### 🌀 `savecoords`

```bash
/savecoords <n> <text>
```

```ini
Saves a coordinate preset (1–9)
```

---

#### 🌀 `cn`

```bash
/c1-9
```

```ini
Loads a saved coordinate preset (1–9)
```

---

#### 🌀 `setcoords`

```bash
/setcoords
```

```ini
Sets coordinates for jump drive (disabled)
```

---

#### 🌀 `jump`

```bash
/jump
/jump*
```

```ini
Activates jump drive (disabled)
```

---

#### 🌀 `beacon`

```bash
/beacon
/beacon*
```

```ini
Hyperspace beacon commands (disabled)
```

#### 🌀 `jumpbeacon`

> [!NOTE]
> Function:

```bash
UserCmd_JumpBeacon
```

> [!WARNING]
> Command:

```bash
/jumpbeacon
/jumpbeacon*
```

> [!CAUTION]
> Usage:

```bash
/jumpbeacon
/jumpbeacon*
```

> [!IMPORTANT]
> Description:

```ini
Hyperspace beacon commands (disabled)
```

---

#### 🌀 `charge`

```bash
/charge
/charge*
```

```ini
Jump drive charge commands (disabled)
```

---

#### 🌀 `jumpsys`

```bash
/jumpsys
```

```ini
Analyzes available jumpable systems (disabled)
```

---

#### 🌀 `showscan`

```bash
/showscan
/showscan$
/scan
/scanid
```

```ini
Scan player/system information (disabled)
```

---

#### 🌀 `maketag`

```bash
/maketag <tag> <master password> <description>
```

```ini
Creates a faction tag with its own password and description
```

---

#### 🌀 `droptag`

```bash
/droptag <tag> <master_password>
```

```ini
Deletes a faction tag
```

---

#### 🌀 `settagpass`

```bash
/settagpass <tag> <master_password> <rename_password>
```

```ini
Changes faction tag password
```

---

#### 🌀 `changeship`

```bash
/changeship [id]
/cs [id]
```

```ini
Saves the current ship into a temporary player hangar.

Allows switching to another saved ship (only works in space).

⚠️ There is a server-side limit on the number of stored ships.

To delete a ship from the hangar, use:
/ds or /delship
```

---

#### 🌀 `delship`

```bash
/delship [id]
/ds [id]
```

```ini
Deletes a saved ship from the player’s hangar
```
