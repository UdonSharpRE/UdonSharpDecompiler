[中文](https://github.com/extremeblackliu/UdonSharpDecompiler/)

# UdonSharpDecompiler

VRChat World Script Decompiler.

# BE AWARE

this is my first time writing of decompiler and it does have big logic issue,
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

when i realize the decompiler issue, its too late, i have to rewrite all the code or it will fucks up. 

but im not longer touch this project, i decided to public this
