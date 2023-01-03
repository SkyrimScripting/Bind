scriptName _Bind_Quest_FormID extends Quest
{
    Truth Ore Consequences
    !BIND 0x5B2DE
    ^--- attaching to a quest that starts on game start can result in multiple OnInit() calls
}

event OnInit()
    GotoState("Initialized")
    ; If this will be an 0xFF dynamic form, do not print out `self` (it't not deterministic, which we want for tests)
    string script = StringUtil.Substring(self, 1, StringUtil.Find(self, " ") - 1)
    Debug.Trace("[!BIND] Script " + script + " bound to " + GetID())
    Debug.Trace("[bind generated object ] " + self) ; <--- not picked up by tests, but viewable in logs
endEvent

state Initialized
    event OnInit()
        ; If this will be an 0xFF dynamic form, do not print out `self` (it't not deterministic, which we want for tests)
        string script = StringUtil.Substring(self, 1, StringUtil.Find(self, " ") - 1)
        Debug.Trace("[!BIND] ALREADY INITIALIZED Script " + script + " bound to " + GetID())
        Debug.Trace("[bind generated object ] " + self) ; <--- not picked up by tests, but viewable in logs
    endEvent
endState
