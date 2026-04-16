<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Язык: </strong>
  
  <span style="color: #F5F752; margin: 0 10px;">
    ✅ 🇷🇺 Русский (текущий)
  </span>
  | 
  <a href="./BOOST_DEPENDENCIES.md" style="color: #0891b2; margin: 0 10px;">
    🇺🇸 English
  </a>
</div>

---

- Чтобы значительно сократить объем программирования, необходимого для простых задач, мы начали использовать библиотеку Boost.
  Совместимая с FLHook версия — MSVC 14.0 32-битная (64-битная версия не будет работать).

- Последняя используемая версия: https://sourceforge.net/projects/boost/files/boost-binaries/1.64.0/boost_1_64_0-msvc-14.1-32.exe/download

- Для упрощения управления зависимостями Boost в проекте, произвольный путь к Boost указывается относительно решения.

- Например: C:\flhook_2015\dependencies\boost
- Или даже: D:\kittens\flhook_apples\dependencies\boost

- По этой причине она была исключена из Git, так как слишком ресурсоемка. Правильная установка — на ваше усмотрение.
