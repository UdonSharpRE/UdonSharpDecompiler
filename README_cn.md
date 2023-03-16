# UdonSharp 反编译器

反编译VRChat地图中的脚本

# 注意

在你反编译之前，你需要先反汇编，[反汇编器](https://github.com/extremeblackliu/UdonSharpDisassembler)

这是我第一次编写反编译器，它存在很大的逻辑上的问题，它只能反编译单层的逻辑，例如：

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

我意识到这个问题的时候已经太晚了，除非我把所有的代码都重写，否则这个编译器可能就只能这样了。

但是我已经很长时间没碰这个了，留给我自己也没用，所以我还是把它公开了
