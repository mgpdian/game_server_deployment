# zinx框架测试用例四

用例四- 添加时间

添加日期前缀

​	 1. 创建添加日期类，继承AzinxHandler。重写处理函数和获取下一环节函数

2. 处理函数：将日期和输入字符串拼接后，new一个对象返回

3. 获取下一环节函数：返回回显对象

4. 在命令处理类的处理函数中：根据输入命令设置当前是否要添加前缀的状态位

5. 在命令处理类的获取下一环节函数中，

  判断当前状态，需要添加前缀  --》返回添加日期前缀的对象；

  不需要添加前缀--》返回回显对象



接下来是具体实现

数据分发处理功能类 添加一个date_state 来记录当前是否要添加日期前缀

如果需要添加日期前缀 的话  根据date_state的状态 判断是否转发到AddDate数据日期前缀处理类   然后该类处理后 转发到poEcho类中

```c++
//数据分发处理功能类 
class CmdHandler : public AZinxHandler
{
	bool date_state = false;
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
		else if (oBytes.szData == "date")
		{
			date_state = true;
			return nullptr;
		}
		else if (oBytes.szData == "cleardate")
		{
			date_state = false;

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
			if (date_state == true)
			{
				return poAddDate;
			}
			else {
				return poEcho;
			}
			
		}
		
	}
	//int status = 0;
} *poCmd = new CmdHandler();


```

消息日期前缀 处理功能类

```c++
class AddDate : public AZinxHandler
{
	// 通过 AZinxHandler 继承
	virtual IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
	{
		GET_REF2DATA(BytesMsg, oBytes, _oInput);
		time_t tmp;
		time(&tmp);
		std::string szNew = std::string(ctime(&tmp)) + oBytes.szData;

		BytesMsg* pret = new BytesMsg(oBytes);
		pret->szData = szNew;
		return pret;
	}
	virtual AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
	{
		return poEcho;
	}
} *poAddDate = new AddDate();


```

