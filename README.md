[中文](https://github.com/extremeblackliu/UdonSharpDecompiler/blob/master/README_cn.md)

# UdonSharpDecompiler

VRChat World Script Decompiler.

# BE AWARE

before decompile, you need disassemble it, here is [UdonSharpDisassembler](https://github.com/extremeblackliu/UdonSharpDisassembler)

for example:

Original Code:
```CS
using UdonSharp;
using UnityEngine;
using VRC.SDKBase;
using VRC.Udon;

public class Logic: UdonSharpBehaviour {
  void Update() {
    float dt = Time.deltaTime;
    if (dt > 0.0 f) {
      if (dt > 3.0 f) {
        Debug.Log("low fps!!!");
      }
    } else {
      Debug.Log("can this happening??");
      if (dt > 2.0 f) {
        Debug.Log("also for this??");
      }
    }

    for (int i = 0; i < 100; i++) {
      Debug.Log("now spamming you");
    }

    int x = 500;
    while (x > 0) {
      Debug.Log("while now spamming you");
      x--;
    }
  }
}
```

Decompiled:
```CS
Loaded Functions: 1
Loaded Symbols:
Address: 0x0000000000000002 | Symbol: __0_i_Int32
Address: 0x0000000000000003 | Symbol: __0_x_Int32
Address: 0x0000000000000004 | Symbol: __0_dt_Single
Address: 0x0000000000000005 | Symbol: __3_const_intnl_SystemInt32
Address: 0x0000000000000006 | Symbol: __2_const_intnl_SystemInt32
Address: 0x0000000000000007 | Symbol: __1_const_intnl_SystemInt32
Address: 0x0000000000000008 | Symbol: __0_const_intnl_SystemInt32
Address: 0x0000000000000009 | Symbol: __2_const_intnl_SystemSingle
Address: 0x000000000000000A | Symbol: __1_const_intnl_SystemSingle
Address: 0x000000000000000B | Symbol: __0_const_intnl_SystemSingle
Address: 0x000000000000000C | Symbol: __4_const_intnl_SystemString
Address: 0x000000000000000D | Symbol: __3_const_intnl_SystemString
Address: 0x000000000000000E | Symbol: __2_const_intnl_SystemString
Address: 0x000000000000000F | Symbol: __1_const_intnl_SystemString
Address: 0x0000000000000010 | Symbol: __0_const_intnl_SystemString
Address: 0x0000000000000011 | Symbol: __0_const_intnl_SystemUInt32
Address: 0x0000000000000012 | Symbol: __1_intnl_SystemBoolean
Address: 0x0000000000000013 | Symbol: __0_intnl_SystemBoolean
Address: 0x0000000000000014 | Symbol: __0_intnl_SystemInt32
Address: 0x0000000000000015 | Symbol: __0_intnl_returnTarget_UInt32
void Func_8()
{
        __0_dt_Single = UnityEngineTime.get_deltaTime();
        __0_intnl_SystemBoolean = __0_dt_Single > 0;
        if(__0_intnl_SystemBoolean)
        {
			__1_intnl_SystemBoolean = __0_dt_Single > 3;
			if(__1_intnl_SystemBoolean)
			{
				UnityEngineDebug.Log("low fps!!!");
LABEL_136:
				goto LABEL_224;
			}
			else {
				goto LABEL_136;
			}
			goto LABEL_224;
LABEL_144:
        }
        else {
            goto LABEL_144;
        }
        UnityEngineDebug.Log("can this happening??");
        __1_intnl_SystemBoolean = __0_dt_Single > 2;
        if(__1_intnl_SystemBoolean)
        {
			UnityEngineDebug.Log("also for this??");
LABEL_224:
        }
        else {
            goto LABEL_224;
        }
        __0_i_Int32 = 0;
LABEL_244:
        __1_intnl_SystemBoolean = __0_i_Int32 < 100;
        if(__1_intnl_SystemBoolean)
        {
			UnityEngineDebug.Log("now spamming you");
			__0_intnl_SystemInt32 = __0_i_Int32;
			__0_i_Int32 = __0_intnl_SystemInt32 + 1;
			goto LABEL_244;
LABEL_368:
        }
        else {
            goto LABEL_368;
        }
        __0_x_Int32 = 500;
LABEL_388:
        __1_intnl_SystemBoolean = __0_x_Int32 > 0;
        if(__1_intnl_SystemBoolean)
        {
			UnityEngineDebug.Log("while now spamming you");
			__0_intnl_SystemInt32 = __0_x_Int32;
			__0_x_Int32 = __0_intnl_SystemInt32 - 1;
			goto LABEL_388;
LABEL_512:
        }
        else {
                goto LABEL_512;
        }
        __0_intnl_returnTarget_UInt32 = __0_intnl_returnTarget_UInt32;
}
```
the output maybe wrong due to no information of Game functions.

Simplified by AI(maybe wrong but its pretty much readable):
```CS
void Func_8()
{
    float deltaTime = UnityEngine.Time.deltaTime;
    
    if (deltaTime > 0)
    {
        if (deltaTime > 3)
        {
            UnityEngine.Debug.Log("low fps!!!");
        }
        
        UnityEngine.Debug.Log("can this happening??");
        
        if (deltaTime > 2)
        {
            UnityEngine.Debug.Log("also for this??");
        }
        
        for (int i = 0; i < 100; i++)
        {
            UnityEngine.Debug.Log("now spamming you");
        }
        
        int x = 500;
        
        while (x > 0)
        {
            UnityEngine.Debug.Log("while now spamming you");
            x--;
        }
    }
}
```
it was simplified by https://codepal.ai/code-simplifier (not ad)

you can try set line 73 (m_bRequiredThisPtr) to true in Globals.h to get different output

# TODO
- optimize decompile output to make it generate more readable code.
