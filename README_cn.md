# UdonSharp 反编译器

反编译VRChat地图中的脚本

# 注意

在你反编译之前，你需要先反汇编，[反汇编器](https://github.com/extremeblackliu/UdonSharpDisassembler)

这是我第一次编写反编译器，它存在很大的逻辑上的问题，并且由于我从来没了解过反编译器的运行原理，我在写这个的时候完全由自己构想，使用了分析反汇编结果的字符串并且分析单条opcode
它只能反编译单层的逻辑，例如：

```CS
// 这个函数只有一层逻辑，所以反编译器可以正常的反编译这个函数
void TestFunc()
{
    bool IsBigger = 5 > 3;
    if(IsBigger)
    {
        Debug.Log("yea 5 is bigger than 3");
    }
}


// 这个函数有多层的逻辑，反编译器将会出错
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
# 示例输出结果
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
结果中的一些函数调用可能因为缺少该函数的信息导致分析失败，这是因为在dump的时候没有将依赖的函数信息同时dump出来。

你可以在Globals.h中的第73行的m_bRequiredThisPtr设置为true来获取不同的结果(基本上是将一些thiscall调用的函数直接引用该this变量)：
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
        __1_intnl_SystemInt32 = toggleObjects.get_Length();
        while(true)
        {
        __0_intnl_SystemBoolean = __0_intnl_SystemInt32 < __1_intnl_SystemInt32;
        if(__0_intnl_SystemBoolean)
                break;
        __0_toggleObject_GameObject = toggleObjects.Get(__0_intnl_SystemInt32);
        __1_intnl_SystemBoolean = __0_toggleObject_GameObject.get_activeSelf();
        __2_intnl_SystemBoolean = __1_intnl_SystemBoolean;
        __0_toggleObject_GameObject.SetActive(__2_intnl_SystemBoolean);
        __0_intnl_SystemInt32 = __0_intnl_SystemInt32 + 1;
        }
        __0_intnl_returnTarget_UInt32 = __0_toggleObject_GameObject;
}
```

我意识到这个问题的时候已经太晚了，除非我把所有的代码都重写，否则这个编译器可能就只能这样了。

但是我已经很长时间没碰这个了，留给我自己也没用，所以我还是把它公开了
