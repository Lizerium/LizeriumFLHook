<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./BOOST_DEPENDENCIES.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

---

- To significantly reduce the amount of code required for simple tasks, we started using the Boost library.  
  The compatible version with FLHook is **MSVC 14.0 32-bit** (64-bit will NOT work).

- The last used version:  
  https://sourceforge.net/projects/boost/files/boost-binaries/1.64.0/boost_1_64_0-msvc-14.1-32.exe/download

- To simplify dependency management, the Boost path in the project is specified **relative to the solution**.

- Example paths:
  - `C:\flhook_2015\dependencies\boost`
  - `D:\kittens\flhook_apples\dependencies\boost`

- For this reason, Boost is excluded from Git, as it is too large.  
  Installation and setup are left to you.
