scriptName _Bind_Object_EditorID extends ObjectReference
{
    # I can't think of objects with Editor IDs except for Actors
    !BIND SvenRef
}

event OnInit()
    string script = StringUtil.Substring(self, 1, StringUtil.Find(self, " ") - 1)
    Debug.Trace("[!BIND] Script " + script + " bound to " + self + " " + GetBaseObject().GetName())
endEvent
