# Bind

[![Download Bind](https://github.com/SkyrimScripting/SkyrimScripting/raw/main/Resources/DownloadButton.png)](https://github.com/SkyrimScripting/Bind/releases/download/v1/BIND.Papyrus.Scripts.7z)

> Bind Papyrus Scripts On Game Load

![Bind](Images/Logo.png)

# Papyrus Development

This is a **Developer Tool:** _for testing Papyrus scripts!_

# `v1`

The goal for `v1` is to be _as minimal as possible_:

- Bindings are defined in `Scripts\Bindings\`
- Each file should contain 1 line per desired "binding" (_attach script to something_)
- Support for binding to objects by Form ID
  ```
  NameOfScript 0x14
  ```
- Support for binding to objects by Editor ID
  ```
  NameOfScript dlc1serana
  ```
- Support for binding to the Player (_as an `Actor` Form, not a `ReferenceAlias`_)
  ```
  NameOfScript $Player
  ```
- Support for binding to an anonymous/generated `Quest`
  ```
  NameOfScript $Quest
  ```
- Support for binding to an anonymous/generated `Quest` with a provided editor ID
  ```
  NameOfScript $Quest(MyCoolQuest)
  ```
- Support for binding to an anonymous/generated `ObjectReference`
  ```
  NameOfScript $Object
  ```
  > _Objects are ALWAYS placed in the `WEMerchantChests` cell. This is not configurable._
- Support for binding to a generated `ObjectReference` of a specified base Form ID
  ```
  NameOfScript $Object(0x7)
  ```
- Support for binding to a generated `ObjectReference` of a specified base Editor ID
  ```
  NameOfScript $Object(FoodSweetroll)
  ```
- Support for determining what to bind to automatically based on script `extends`
  ```
  NameOfScript
  ```
  ```psc
  scriptName NameOfScript extends Quest ; Anonymous Quest
  scriptName NameOfScript extends Actor ; Player
  scriptName NameOfScript extends ObjectReference ; Anonymous Object
  scriptName NameOfScript extends CustomType ; CustomType parent(s) checked
  ```
- Nothing is configurable

<details><summary>Future Planned Version Features</summary>

# `v1.1` Champollion support (**Planned**)

> _This has not yet been implemented but is planned for the next release._

- Read every `.pex` (_file mtime support_)
- Auto register any including this comment:
  ```psc
  scriptName Whatever
  {
    BIND: Player
    BIND: $NewQuest$
  }
  ```
- Or Simply
  ```psc
  scriptName Whatever
  {BIND}
  ```

</details>
