#include "AOIWorld.h"


//通过构造函数指定矩形的大小和分割颗粒
AOIWorld::AOIWorld(int _x_begin, int _x_end, int _y_begin, int _y_end, int _x_count, int _y_count)
	: x_begin(_x_begin),x_end(_x_end),y_begin(_y_begin),y_end(_y_end),x_count(_x_count),y_count(_y_count)
{
	//x轴网格宽度=(x轴结束坐标-x轴起始坐标)/x轴网格数量；y轴的计算方式相同
	x_width = (x_end - x_begin) / x_count;
	y_width = (y_end - y_begin) / y_count;

	//创建网格类
	for (int i = 0; i < x_count * y_count; i++)
	{
		Grid tmp;
		m_grids.emplace_back(tmp);
	}
}

AOIWorld::~AOIWorld()
{
}
//获取周围玩家
std::list<Player*> AOIWorld::GetSrdPlayers(Player* _player)
{
	std::list<Player *> ret;
	
	//计算所属编号
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;
	//访问周围的网格

	//计算读取网格的横纵网格数目位置
	int x_index = grid_id % x_count;
	int y_index = grid_id / x_count;

	//左上 grid_id - 1 - x_count
	if (x_index > 0 && y_index > 0)
	{
		std::list<Player*>& cur_list = m_grids[grid_id - 1 - x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	
	//正上方 grid_id  - x_count
	if (y_index > 0)
	{
		std::list<Player*>& cur_list = m_grids[grid_id  - x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}

	//右上 grid_id - x_count + 1
	if (x_index < x_count - 1 && y_index > 0)
	{
		std::list<Player*>& cur_list = m_grids[grid_id - x_count + 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}

	//正左 grid_id - 1
	if (x_index > 0)
	{
		std::list<Player*>& cur_list = m_grids[grid_id - 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	
	{
		//自己 正中 grid_id
		std::list<Player*>& cur_list = m_grids[grid_id].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	

	//正右 grid_id + 1
	if (x_index < x_count - 1)
	{
		std::list<Player*>& cur_list = m_grids[grid_id + 1].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}

	//左下 grid_id - 1 + x_count
	if (x_index > 0 && y_index < y_count - 1)
	{
		std::list<Player*>& cur_list = m_grids[grid_id - 1 + x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}

	//正下 grid_id + x_count
	if (y_index < y_count - 1)
	{
		std::list<Player*>& cur_list = m_grids[grid_id + x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}

	//右下 grid_id + 1 + x_count
	if (x_index < x_count - 1  && y_index < y_count - 1)
	{
		std::list<Player*>& cur_list = m_grids[grid_id + 1 + x_count].m_players;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}


	return ret;
}

//添加玩家到AOI网格
bool AOIWorld::AddPlayer(Player* _player)
{
	//计算所属网格号
	//添加到该网格中
	// //x轴网格宽度=(x轴结束坐标-x轴起始坐标)/x轴网格数量；y轴的计算方式相同
	
	//网格编号 = (x-x轴起始坐标)/x轴网格宽度 + (y-y轴起始坐标)/y轴宽度 * x轴网格数量 
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;

	//添加到该网格中
	m_grids[grid_id].m_players.emplace_back(_player);

	
	return true;
}

//摘除玩家 (玩家不一定要被删除 可能只是移动到下一个网格中
void AOIWorld::DelPlayer(Player* _player)
{
	int grid_id = (_player->GetX() - x_begin) / x_width + (_player->GetY() - y_begin) / y_width * x_count;
	
	m_grids[grid_id].m_players.remove(_player);
}
