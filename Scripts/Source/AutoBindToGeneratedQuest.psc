scriptName AutoBindToGeneratedQuest extends Quest

event OnInit()
    ; If this will be an 0xFF dynamic form, do not print out `self` (it't not deterministic, which we want for tests)
    string script = StringUtil.Substring(self, 1, StringUtil.Find(self, " ") - 1)
    Debug.Trace("[!BIND] Script " + script + " bound to " + GetID())
    Debug.Trace("[bind generated object ] " + self) ; <--- not picked up by tests, but viewable in logs
endEvent
