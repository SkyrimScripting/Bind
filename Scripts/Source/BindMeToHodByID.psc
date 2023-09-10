scriptName BindMeToHodByID extends Actor

event OnInit()
    string script = StringUtil.Substring(self, 1, StringUtil.Find(self, " ") - 1)
    Debug.Trace("OnInit [BIND] Script " + script + " bound to " + self + " " + GetBaseObject().GetName())
endEvent

event OnPlayerLoadGame()
    string script = StringUtil.Substring(self, 1, StringUtil.Find(self, " ") - 1)
    Debug.Trace("OnPlayerLoadGame [BIND] Script " + script + " bound to " + self + " " + GetBaseObject().GetName())
endEvent
