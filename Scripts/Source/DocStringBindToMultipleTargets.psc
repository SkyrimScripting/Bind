scriptName DocStringBindToMultipleTargets extends Actor
{
    !BIND SvenRef|0x1348A|$Player|HildeRef
}

event OnInit()
    string script = StringUtil.Substring(self, 1, StringUtil.Find(self, " ") - 1)
    Debug.Trace("[!BIND] Script " + script + " bound to " + self + " " + GetBaseObject().GetName())
endEvent
