# zinx框架测试用例二

### 用例2-退出程序

需求：在官方用例的基础上增加功能：当用户输入exit四个字母时，不做回显，直接退出程序

思路：

- 创建退出程序功能类 继承AZinxHandler类 这个类负责过滤exit  这个不是传递给下一个处理者  如果是则退出框架
- 标准输入类的下一个处理者应该设置为这个类

为什么不直接在处理信息类Echo那里 进行判断呢

> 如果这里直接来判定的话 
> 会违反开闭原则  不利于添加新的功能
> 思路 这里创建退出程序类先过滤

因为可能之后会添加其他的判断  这样直接写入类中的话 不利于扩展

退出程序功能类

```c++

//exit 退出信息处理类 
//Framework 框架
class ExitFramework : public AZinxHandler
{
	// 通过 AZinxHandler 继承
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, obyte, _oInput);

		if (obyte.szData == "exit")
		{
			ZinxKernel::Zinx_Exit(); //ZinxKernel框架的退出类
			return nullptr;
		}
        //创建交给下一个环节处理的数据
		return new BytesMsg(obyte);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}
} *poExit = new ExitFramework();

```

需要修改的类

接收信息管道类

```c++
class TestStdin : public Ichannel
{
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return poExit;
	}
};
```





