#pragma once
#include <list>
#include <vector>
//玩家类
//拥有横纵坐标的对象
class Player {
public:
	virtual int GetX() = 0;
	virtual int GetY() = 0;
};

//网格类 (这个块有多少玩家)
class Grid {
public:
	std::list<Player*> m_players; //因为玩家可能会频繁移动
};


//游戏世界矩形 (地图)
class AOIWorld
{
private:
	//地图大小
	int x_begin = 0;
	int x_end = 0;
	int y_begin = 0;
	int y_end = 0;
	// 横纵轴都要分成几份
	//网格大小
	int x_count = 0;
	int y_count = 0;
	int x_width = 0;
	int y_width = 0;
public:
	std::vector<Grid> m_grids; 

	//通过构造函数指定矩形的大小和分割颗粒
	AOIWorld(int _x_begin, int _x_end, int _y_begin, int _y_end,
		int _x_count, int _y_count);
	virtual ~AOIWorld();

	//获取周围玩家
	std::list<Player*> GetSrdPlayers(Player* _player);

	//添加玩家到AOI网格
	bool AddPlayer(Player* _player);

	//摘除玩家 (玩家不一定要被删除 可能只是移动到下一个网格中
	void DelPlayer(Player* _player);
	
};

