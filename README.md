# Bind

[![Download Bind](https://github.com/SkyrimScripting/SkyrimScripting/raw/main/Resources/DownloadButton.png)](https://github.com/SkyrimScripting/Bind/releases/download/v1/BIND.Papyrus.Scripts.7z)

> Bind Papyrus Scripts On Game Load

![Bind](Images/Logo.png)

# Papyrus Development

This is a **Developer Tool:** _for testing Papyrus scripts!_

# Usage

1. [Install Bind](https://github.com/SkyrimScripting/Bind/releases/download/v1/BIND.Papyrus.Scripts.7z) (_it's an SKSE .dll compatible with SE/AE/GOG/VR_)
1. Create a Papyrus script
   >  _if you want it to extend a `Quest` or `ObjectReference`, specify an `extends`_
1. Create a file `Data\Scripts\Bindings\<your mod name>.txt`
1. Put the name of each of your scripts in the file, one line per script
   ```
   MyScriptOne
   MyScriptTwo
   ```
1. Run the game!
   > Each of your scripts will automatically be bound to a game object  
   > and the `event OnInit()` of each script will run

## Specifying _what_ to bind the script to.

If you create `scriptName Whatever extends Quest`, it will bind to an generated quest.

If you create `scriptName Whatever extends ObjectRefenrence`, it will bind to a generated object.

If you create `scriptName Whatever extends Actor`, it will bind to the Player.

If you want to extend a _specific_ object, see the [`v1`](#v1) details below for this release.

```sh
SomeScript 0x123
SomeScript AnEditorID
```

#### Logs

> ℹ ️ If you run into any problems, check the `SkyrimScripting.Bind.log` in your `My Games\Skyrim Special Edition\SKSE\` folder.

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
- Binding to objects from mods (_if you don't know the full Form ID_) is not supported in `v1`
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
