# Reminder: deploy this file to your mod folder for testing ;)

# Explicit bindings
BindMeToPlayer $Player
BindMeToHodByID 0x1348A
BindMeToHodByEditorID HodRef
BindMeToMultipleTargets SvenRef|0x1348A|$Player|HildeRef
BindMeToGeneratedQuest $Quest
BindMeToGeneratedQuestWithName $Quest(MyCoolQuest)
BindMeToGeneratedObject $Object
BindMeToGeneratedObjectEditorID $Object(FoodSweetroll)
BindMeToGeneratedObjectFormID $Object(0x64B3D)

# Auto
AutoBindToGeneratedQuest
AutoBindToGeneratedObject
AutoBindToPlayer
