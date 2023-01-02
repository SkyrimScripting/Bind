# `{!BIND}`

<a 
    title="Download BIND SKSE Plugin - Install in your Mod Manager"
    href="https://github.com/SkyrimScripting/Bind/releases/download/v2/BIND.v2.7z">
    <img
        alt="Download BIND SKSE Plugin - Install in your Mod Manager"
        src="https://raw.githubusercontent.com/SkyrimScripting/Download/main/DownloadButton_256.png"
        width="100"
        />
</a>

> Bind Papyrus Scripts On Game Load

![Bind](Images/Logo.png)

# Papyrus Development

This is a **Developer Tool:** _for testing Papyrus scripts!_

# Usage

1. [Install Bind](https://github.com/SkyrimScripting/Bind/releases/download/v1/BIND.Papyrus.Scripts.7z) (_it's an SKSE .dll compatible with SE/AE/GOG/VR_)
1. Create a Papyrus script
1. Add a `{!BIND}` comment to the script (_see examples below_)
   ```papyrus
   scriptName MyMCM extends Quest
   {!BIND}

   event OnInit()
     Debug.MessageBox("This is fantastic!") ; <--- automatically runs :)
   endEvent
   ```
1. Run the game!
   > Each of your scripts will automatically be bound to a game object.  
   > _e.g. the `event OnInit()` of each script will run_

# `{!BIND}` is for Papyrus developers

At this time, `{!BIND}` is only intended for **developers**.

Scripts are loaded **every time the game is run**.

There is no concept of "saves" at this time.

> _We might make `{!BIND}` production-ready for distribution with mods in the future._

# `{!BIND}` with no arguments

If your script comment is simply `{!BIND}` with no arguments:

## Bind to Player

```papyrus
; This will bind to the Player
; because it "extends Actor"
scriptName MyScript extends Actor
{!BIND}
```

## Bind to Quest

```papyrus
; This will generate a random quest and bind to it
; because it "extends Quest"
scriptName MyScript extends Quest
{!BIND}
```

## Bind to Object

```papyrus
; This will generate a random object and bind to it
; because it "extends ObjectReference"
scriptName MyScript extends ObjectReference
{!BIND}
```

# `{!BIND}` to specific objects

## Bind by 0x Form ID

```papyrus
; This will bind to the game reference of the "Hod" NPC in Riverwood
; because 0x1348A is the Form ID of this reference
scriptName MyScript extends Actor
{!BIND 0x1348A}
```

## Bind by Editor ID

> _Note: most Editor ID lookups only work if you (and your users) install:_
> - _[powerofthree's Tweaks](https://www.nexusmods.com/skyrimspecialedition/mods/51073) ([VR](https://www.nexusmods.com/skyrimspecialedition/mods/59510))_

```papyrus
; This will bind to the game reference of the "Hod" NPC in Riverwood
; because "HodRef" is an Editor ID matching the NPC
scriptName MyScript extends Actor
{!BIND HodRef}
```

## Bind to a new Quest with a provided Editor ID

```papyrus
; This will generate a new quest and give it the Editor ID "MyCoolQuest"
; and bind to it.
scriptName MyScript extends Quest
{!BIND $Quest(MyCoolQuest)}
```

## Bind to new Object Reference of specific Base Form

> _Note: generated objects for ObjectReference scripts are spawned in the `WEMerchantChests` CELL._  
> _If you don't specify a type of `$Object`, then a `DwarvenFork` is spawned._

### By Form ID

```papyrus
; This will bind to a reference to a generated Sweet Roll
; because 0x64B3D is the Form ID of a Sweet Roll
scriptName MyScript extends ObjectReference
{!BIND $Object(0x64B3D)}
```

### By Editor ID

```papyrus
; This will bind to a reference to a generated Sweet Roll
; because 0x64B3D is the Form ID of a Sweet Roll
scriptName MyScript extends ObjectReference
{!BIND $Object(FoodSweetRoll)}
```

## `{!BIND}` to multiple objects from the same script

A single script can bind to an unlimited number of objects.

## Multiple Actors

```papyrus
; This will bind `MyScript` to the three specified NPCs in Riverwood
scriptName MyScript extends Actor
{
    !BIND HodRef
    !BIND SvenRef
    !BIND HildeRef
}
```

## Multiple Objects

```papyrus
; This will bind `MyScript` to the three specified object references
; You could use Form IDs of any object in the game
scriptName MyScript extends ObjectReference
{
    !BIND 0x123
    !BIND 0x456
    !BIND 0x789
}
```

## Multiple Quests

```papyrus
; This will bind `MyScript` to the three specified Quests
; MQ101 is Unbound
; MQ102 is Before the Storm
; MQ103 is Bleak Falls Barrow
scriptName MyScript extends Quest
{
    !BIND MQ101
    !BIND MQ102
    !BIND MQ103
}
```

## Multiple Targets in a single line

```papyrus
; This will bind `MyScript` to the the player and three specified NPCs in Riverwood using some of the supported binding options
scriptName MyScript extends Actor
{
    !BIND SvenRef|$Player|0x1348A|HildeRef
}
```

# Logs

> ℹ ️ If you run into any problems, check the `SkyrimScripting.Bind.log` in your  
> `My Games\Skyrim Special Edition\SKSE\` folder.
