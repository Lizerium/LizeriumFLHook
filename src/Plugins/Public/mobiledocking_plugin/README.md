### MOBILE DOCKING

> [!IMPORTANT]
>
> - Plugin loaded: dock **(mobiledock.dll)**
>
>   Описание

```ini
Плагин добавляющий систему мобильной стыковки между игроками в Freelancer.
```

> ---

> 1.  [Пример конфигурации `dockingmodules.cfg`](../../../Binaries/bin-conf/flhook_plugins/dockingmodules.cfg)

---

> [!NOTE]
> Функция:

```bash
UserCmd_Process
```

> ---
>
> [!WARNING]
> Команда:

```bash
/listdocked
```

> [!CAUTION]
> Применение:

```bash
/listdocked
```

> [!IMPORTANT]
> Описание:

```ini
Показывает список всех кораблей, которые в данный момент пристыкованы к вашему кораблю-носителю
```

> ---

> [!NOTE]
> Функция:

```bash
UserCmd_Process
```

> ---
>
> [!WARNING]
> Команда:

```bash
/allowdock
```

> [!CAUTION]
> Применение:

```bash
/allowdock
```

> [!IMPORTANT]
> Описание:

```ini
Разрешает текущему выбранному игроку пристыковаться к вашему кораблю, если у вас есть свободный docking module
```

> ---

> [!NOTE]
> Функция:

```bash
UserCmd_Process
```

> ---
>
> [!WARNING]
> Команда:

```bash
/jettisonship
```

> [!CAUTION]
> Применение:

```bash
/jettisonship <charname>
```

> [!IMPORTANT]
> Описание:

```ini
Принудительно выбрасывает указанный пристыкованный корабль из вашего корабля-носителя обратно в космос
```

> ---

> [!NOTE]
> Функция:

```bash
Dock_Call
```

> ---
>
> [!WARNING]
> Действие:

```bash
Попытка стыковки с кораблём игрока
```

> [!CAUTION]
> Применение:

```bash
Навестись на корабль-носитель и выполнить стандартную попытку dock
```

> [!IMPORTANT]
> Описание:

```ini
Создаёт запрос на мобильную стыковку с выбранным кораблём игрока, если тот подходит по условиям
```

> ---

> [!NOTE]
> Функция:

```bash
ExecuteCommandString_Callback
```

> ---
>
> [!WARNING]
> Команда:

```bash
logactivity
```

> [!CAUTION]
> Применение:

```bash
logactivity
```

> [!IMPORTANT]
> Описание:

```ini
Создаёт диагностический лог состояния mobile docking системы, включая носители, пристыкованные корабли и активных игроков
```
