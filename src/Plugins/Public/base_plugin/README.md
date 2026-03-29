### 🌲 BASE 🌲

> ---
>
> [!IMPORTANT]
>
> - Plugin loaded: base **(base.dll)**
>
>   Описание:

```ini
Добавляет строительство базы со своим банком и системой прав
```

> ---

> 1.  [Пример конфигурации `base.cfg`](../../../Binaries/bin-conf/flhook_plugins/base.cfg)
> 2.  [Пример конфигурации `base_recipe_items.cfg`](../../../Binaries/bin-conf/flhook_plugins/base_recipe_items.cfg)
> 3.  [Пример конфигурации `base_recipe_modules.cfg`](../../../Binaries/bin-conf/flhook_plugins/base_recipe_modules.cfg)
> 4.  [Пример конфигурации `base_archtypes.cfg`](../../../Binaries/bin-conf/flhook_plugins/base_archtypes.cfg)
>
> ---

> [!NOTE]
> Функция:

```bash
BaseHelp
```

> ---
>
> [!TIP]
> Команда:

```bash
/base
```

> [!CAUTION]
> Применение:

```bash
/base help [page]
```

> [!IMPORTANT]
> Описание:

```ini
Показать эту страницу помощи. Укажите номер страницы, чтобы увидеть следующую страницу.
```

> ---

> [!NOTE]
> Функция:

```bash
BaseLogin
```

> ---
>
> [!TIP]
> Команда:

```bash
/base login
```

> [!CAUTION]
> Применение:

```bash
/base login [password]
```

> [!IMPORTANT]
> Описание:

```ini
Войдите как администратор базы.
Следующие команды доступны только в том случае, если вы вошли в систему как базовый администратор.
```

> ---

> [!NOTE]
> Функция:

```bash
BaseAddPwd
```

> ---
>
> [!TIP]
> Команда:

```bash
/base addpwd
```

> [!CAUTION]
> Применение:

```bash
/base addpwd [password]
```

```bash
/base addpwd [password] [viewshop]
```

> [!IMPORTANT]
> Описание:

```ini
Добавляйте пароли администратора для базы
Добавьте viewhop в addpwd, чтобы разрешить просмотр магазина только с помощью пароля.
```

> ---

> [!NOTE]
> Функция:

```bash
BaseRmPwd
```

> ---
>
> [!TIP]
> Команда:

```bash
/base rmpwd
```

> [!CAUTION]
> Применение:

```bash
/base rmpwd [password]
```

> [!IMPORTANT]
> Описание:

```ini
Удаляйте пароли администратора для базы
```

> ---

> [!NOTE]
> Функция:

```bash
BaseLstPwd
```

> ---
>
> [!TIP]
> Команда:

```bash
/base lstpwd
```

> [!CAUTION]
> Применение:

```bash
/base lstpwd
```

> [!IMPORTANT]
> Описание:

```ini
Перечисляйте пароли администратора для базы
```

> ---

> [!NOTE]
> Функция:

```bash
BaseSetMasterPwd
```

> ---
>
> [!TIP]
> Команда:

```bash
/base setmasterpwd
```

> [!CAUTION]
> Применение:

```bash
/base setmasterpwd [old password] [new password]
```

> [!IMPORTANT]
> Описание:

```ini
Установите мастер-пароль для базы.
```

> ---

> [!NOTE]
> Функция:

```bash
BaseRmHostileTag
BaseAddAllyTag
```

> ---
>
> [!TIP]
> Команда:

```bash
/base addtag
```

> [!CAUTION]
> Применение:

```bash
/base addtag [tag]
```

> [!IMPORTANT]
> Описание:

```ini
Добавляйте теги союзников для базы.
```

> ---

> [!NOTE]
> Функция:

```bash
BaseRmAllyTag
```

> ---
>
> [!TIP]
> Команда:

```bash
/base rmtag
```

> [!CAUTION]
> Применение:

```bash
/base rmtag [tag]
```

> [!IMPORTANT]
> Описание:

```ini
Удаляйте теги союзников для базы.
```

> ---

> [!NOTE]
> Функция:

```bash
BaseLstAllyTag
```

> ---
>
> [!TIP]
> Команда:

```bash
/base lsttag
```

> [!CAUTION]
> Применение:

```bash
/base lsttag
```

> [!IMPORTANT]
> Описание:

```ini
Перечисляйте теги союзников для базы.
```

> ---

```bash
BaseRmAllyTag
BaseAddHostileTag
```

> ---
>
> [!TIP]
> Команда:

```bash
/base addhostile
```

> [!CAUTION]
> Применение:

```bash
/base addhostile [tag]
```

> [!IMPORTANT]
> Описание:

```ini
Добавляйте в черный список теги базы.
Они будут расстреляны на месте, поэтому используйте полные теги, такие как like =LSF= или IMG| или shipname like Crunchy_Salad
```

> ---

```bash
BaseRmHostileTag
```

> ---
>
> [!TIP]
> Команда:

```bash
/base rmhostile
```

> [!CAUTION]
> Применение:

```bash
/base rmhostile [tag]
```

> [!IMPORTANT]
> Описание:

```ini
Удаляйте из черного списка теги базы.
Они будут расстреляны на месте, поэтому используйте полные теги, такие как like =LSF= или IMG| или shipname like Crunchy_Salad
```

> ---

```bash
BaseLstHostileTag
```

> ---
>
> [!TIP]
> Команда:

```bash
/base lsthostile
```

> [!CAUTION]
> Применение:

```bash
/base lsthostile
```

> [!IMPORTANT]
> Описание:

```ini
Перечисляйте черный список тегов базы.
Они будут расстреляны на месте, поэтому используйте полные теги, такие как like =LSF= или IMG| или shipname like Crunchy_Salad
```

> ---

```bash
BaseRep
```

> ---
>
> [!TIP]
> Команда:

```bash
/base rep
```

> [!CAUTION]
> Применение:

```bash
/base rep
```

```bash
/base rep [clear]
```

> [!IMPORTANT]
> Описание:

```ini
Установите или удалите фракцию, к которой принадлежит эта база.
При настройке принадлежности к фракции выбор падёт на ту к которой принадлежит игрок, выполняющий команду.
```

> ---

```bash
BaseDefenseMode
```

> ---
>
> [!TIP]
> Команда:

```bash
/base defensemode
```

> [!CAUTION]
> Применение:

```bash
/base defensemode
```

> [!IMPORTANT]
> Описание:

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
BaseDeploy
```

> ---
>
> [!TIP]
> Команда:

```bash
/base deploy
```

> [!CAUTION]
> Применение:

```bash
/base deploy <password> <basename>
```

> [!IMPORTANT]
> Описание:

```ini
Создаёт базу при наличии ремонтного корабля, предварительной полной остановке перед созданием, груза для строительства
```

> ---

```bash
Shop
```

> ---
>
> [!TIP]
> Команда:

```bash
/shop price
```

> [!CAUTION]
> Применение:

```bash
/shop price [item] [price] [min stock] [max stock]
```

> [!IMPORTANT]
> Описание:

```ini
Установите [price] для [item]. Если текущий запас меньше [min stock]
тогда предмет нельзя будет купить на пришвартованных кораблях. Если текущий запас больше или равен
[max stock] тогда предмет нельзя будет продать на базу пришвартованными кораблями.
Запретить продажу на базу предмета пришвартованными кораблями при всех условиях, [max stock] установить в 0.
Запретить покупку предмета с базы пристыкованными кораблями при любых условиях, [min stock] установить в 0.
```

> ---

```bash
Shop
```

> ---
>
> [!TIP]
> Команда:

```bash
/shop remove
```

> [!CAUTION]
> Применение:

```bash
/shop remove [item]
```

> [!IMPORTANT]
> Описание:

```ini
Удалить товар из списка акций.
Его нельзя продать на базу пришвартованными кораблями, если они не являются администраторами базы.
```

> ---

```bash
Shop
```

> ---
>
> [!TIP]
> Команда:

```bash
/shop
```

> [!CAUTION]
> Применение:

```bash
/shop [page]
```

> [!IMPORTANT]
> Описание:

```ini
Показать список товаров магазина [page]. На странице отображается максимум 40 элементов.
```

> ---

```bash
Bank
```

> ---
>
> [!TIP]
> Команда:

```bash
/basebank
```

> [!CAUTION]
> Применение:

```bash
/basebank withdraw [credits],
```

> [!IMPORTANT]
> Описание:

```ini
Снимайте кредиты имеющиеся в базовом банке.
```

> ---

```bash
Bank
```

> ---
>
> [!TIP]
> Команда:

```bash
/basebank
```

> [!CAUTION]
> Применение:

```bash
/basebank deposit [credits],
```

> [!IMPORTANT]
> Описание:

```ini
Вносите в базовый банк кредиты.
```

> ---

```bash
Bank
```

> ---
>
> [!TIP]
> Команда:

```bash
/basebank
```

> [!CAUTION]
> Применение:

```bash
/basebank status
```

> [!IMPORTANT]
> Описание:

```ini
Проверяйте состояние кредитов, имеющихся в базовом банке.
```

> ---

```bash
BaseInfo
```

> ---
>
> [!TIP]
> Команда:

```bash
/base info
```

> [!CAUTION]
> Применение:

```bash
/base info
```

> [!IMPORTANT]
> Описание:

```ini
Задайте описание инфокарты базы.
```

> ---

```bash
BaseFacMod
```

> ---
>
> [!TIP]
> Команда:

```bash
/base facmod
```

> [!CAUTION]
> Применение:

```bash
/base facmod
```

> [!IMPORTANT]
> Описание:

```ini
Управление заводскими модулями.
```

> ---

```bash
BaseDefMod
```

> ---
>
> [!TIP]
> Команда:

```bash
/base defmod
```

> [!CAUTION]
> Применение:

```bash
/base defmod
```

> [!IMPORTANT]
> Описание:

```ini
Управляйте модулями защиты.
```

> ---

```bash
BaseShieldMod
```

> ---
>
> [!TIP]
> Команда:

```bash
/base shieldmod
```

> [!CAUTION]
> Применение:

```bash
/base shieldmod
```

> [!IMPORTANT]
> Описание:

```ini
Модули щита управления.
```

> ---

```bash
BaseBuildMod
```

> ---
>
> [!TIP]
> Команда:

```bash
/base buildmod
```

> [!CAUTION]
> Применение:

```bash
/base buildmod
```

> [!IMPORTANT]
> Описание:

```ini
Контролируйте строительство и разрушение базовых модулей и улучшений.
```

> ---
