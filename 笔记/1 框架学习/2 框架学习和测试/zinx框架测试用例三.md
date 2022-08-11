# zinx框架测试用例三

### 用例3-关闭输出

需求：在上例基础上，若输入的是close，不做回显，关掉标准输出。若输入的是open，则开启标准输出

思路：因为功能趋于复杂，所以多个功能处理对象之间的关系难以维护，需要用一个类管理用户输入。 作为分发函数

- 创建命令处理类 继承AZinxHandler类 这个类用来处理用户的open或者close命令 如果是这两个命令则对应的将标准输出类对象从kernel中拿出或添加 如果不是 则交给下一个处理类
- 下一个处理类有两个 所以需要根据待处理消息方法 若内容是exit 则分发给退出程序类 否则分发给回显类
- 标准输入的下一个处理者应该设置为该对象



命令创建分发类

```c++

//数据分发处理功能类 
class CmdHandler : public AZinxHandler
{
	// 通过 AZinxHandler 继承
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		/*判断输入是否是open或close，执行不同操作*/
		GET_REF2DATA(BytesMsg, oBytes, _oInput);

		if (oBytes.szData == "open")
		{
			ZinxKernel::Zinx_Add_Channel(*poOut);
			return nullptr;
		}
		else if (oBytes.szData == "close")
		{
			ZinxKernel::Zinx_Del_Channel(*poOut);
			return nullptr;
		}

		return new BytesMsg(oBytes);
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		GET_REF2DATA(BytesMsg, oBytes, _oNextMsg);
		if (oBytes.szData == "exit")
		{
			return poExit;
		}
		else {
			return poEcho;
		}
		
	}
	//int status = 0;
} *poCmd = new CmdHandler();
```

需要修改的类

接收消息管道类 让它接收消息后转发到命令数据处理类

```c++
class TestStdin : public Ichannel
{
    virtual AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override
	{
		return poCmd;
	}
};
```



添加命令处理类

- 创建命令处理分发类 继承AZinxHandler 重写处理函数和获取下一个处理环节的函数
- 处理函数内 根据输入内容不同 要么添加输出通道 要么摘除输出通道 要么将数据发送到 获取下一个处理环节函数中 
-  在这个函数中 判断是否为exit  来决定下一个环节是退出类还是回显类
- 设定数据输入通道类的下一个环节是该对象poCmd





