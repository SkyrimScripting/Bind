scriptName _Bind_Object_FormID extends ObjectReference
{
    # The Merchant Chest in WEMerchantChests
    !BIND 0xBBCD1
}

event OnInit()
    string script = StringUtil.Substring(self, 1, StringUtil.Find(self, " ") - 1)
    Debug.Trace("[!BIND] Script " + script + " bound to " + self + " " + GetBaseObject().GetName())
endEvent
