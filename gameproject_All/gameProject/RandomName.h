#pragma once
#include <list>
#include <string>
#include <vector>
//姓和  名 组成的线性表
class FirstName {
public:
	std::string m_first;
	std::vector<std::string> m_last_list;
};


class RandomName
{
private:
	//姓名池
	std::vector<FirstName*> m_pool;
public:
	RandomName();

	//从姓名池中取名字  并将该名字移除姓名池
	std::string GetName();

	//将名字还给姓名池
	void Release(std::string _name);

	void LoadFile();
	virtual ~RandomName();
};

