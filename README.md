[中文](https://github.com/extremeblackliu/UdonSharpDecompiler/blob/master/README_cn.md)

# UdonSharpDecompiler

VRChat World Script Decompiler.

# BE AWARE

before decompile, you need disassemble it, here is [UdonSharpDisassembler](https://github.com/extremeblackliu/UdonSharpDisassembler)

this is my first time writing a decompiler and it has a big logic issue. also, i dont know how a decompiler works, so i came up with my own idea by analyzing the disassembled string and stepping through the opcode.
the decompiler can only decompile Single-Layered logic branch, 
for example:

```CS
// this function has only 1 Layer branch, so the Decompiler will works fine
void TestFunc()
{
    bool IsBigger = 5 > 3;
    if(IsBigger)
    {
        Debug.Log("yea 5 is bigger than 3");
    }
}


// this function has MORE-THAN-1 Layer branch, so the Decompiler will broken
void TestFunc1()
{
    bool IsBigger = 5 > 3;
    if(IsBigger)
    {
        Debug.Log("yea 5 is greater than 3");
        if(4 < 5)
        {
            Debug.Log("you shouldnt see this because 4 is smaller than 5");
        }
    }
}
```

# Example Output
```CS
Loaded Functions: 1
Loaded Symbols:
Address: 0x0000000000000002 | Symbol: toggleObjects
Address: 0x0000000000000003 | Symbol: __0_toggleObject_GameObject
Address: 0x0000000000000004 | Symbol: __1_const_intnl_SystemInt32
Address: 0x0000000000000005 | Symbol: __0_const_intnl_SystemInt32
Address: 0x0000000000000008 | Symbol: __0_const_intnl_SystemUInt32
Address: 0x0000000000000009 | Symbol: __2_intnl_SystemBoolean
Address: 0x000000000000000A | Symbol: __1_intnl_SystemBoolean
Address: 0x000000000000000B | Symbol: __0_intnl_SystemBoolean
Address: 0x000000000000000C | Symbol: __1_intnl_SystemInt32
Address: 0x000000000000000D | Symbol: __0_intnl_SystemInt32
Address: 0x000000000000000E | Symbol: __0_intnl_returnTarget_UInt32


void Func_0()
{
        __0_intnl_SystemInt32 = 0;
        __1_intnl_SystemInt32 = UnityEngineGameObjectArray.get_Length();
        while(true)
        {
            __0_intnl_SystemBoolean = __0_intnl_SystemInt32 < __1_intnl_SystemInt32;
            if(__0_intnl_SystemBoolean)
                    break;
            __0_toggleObject_GameObject = SystemObjectArray.Get(__0_intnl_SystemInt32);
            __1_intnl_SystemBoolean = UnityEngineGameObject.get_activeSelf();
            __2_intnl_SystemBoolean = __1_intnl_SystemBoolean;
            UnityEngineGameObject.SetActive(__2_intnl_SystemBoolean);
            __0_intnl_SystemInt32 = __0_intnl_SystemInt32 + 1;
        }
        __0_intnl_returnTarget_UInt32 = __0_toggleObject_GameObject;
}
```
the output maybe wrong due to no information of those functions.

im not longer touch this project, i decided to public this.
