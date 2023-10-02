# UdonSharp 反编译器

反编译VRChat地图中的脚本

# 注意

在你反编译之前，你需要先反汇编，[反汇编器](https://github.com/extremeblackliu/UdonSharpDisassembler)

示例输出:

原始代码:
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

反编译后:
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
输出结果可能因为没有游戏函数的进一步信息而产生错误，这点将会在未来改进。

使用AI简化代码之后(有可能是错的，但是已经非常可读了):
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
我使用了 https://codepal.ai/code-simplifier 来简化(不是广告)

你可以更改 Globals.h 中的第 73 行的 (m_bRequiredThisPtr) 为 true 来获得略微不同的输出结果。
