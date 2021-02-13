#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <time.h>
#include "SimpleAudioEngine.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include "editor-support/cocostudio/CCSGUIReader.h"
#include "ui/CocosGUI.h"
using namespace cocos2d::ui;
using namespace cocostudio;
USING_NS_CC;
#define TAG_SPRITE 1
#define OFFSETX 20 //设置棋盘的X坐标偏移量
#define OFFSETY 70//设置棋盘的Y坐标偏移量

class Situation //这是一个辅助类，里面是两个指针，用来辅助判定局面是否相同了
{
public:
	unsigned long long sinum; //这个是当前行局面的数学表示值
	Situation * p_branch; //同层的下一个分支
	Situation * p_next; //下一层局面值
	Situation(unsigned long long num = 0)
	{
		sinum = num;
		p_branch = NULL;
		p_next = NULL;
	}
};
class Step //这是一个棋谱类，用它来实现返回、前进等功能
{
public:
	int OX, OY, ONum, DONum, AX, AY, ANum, DANum;
	bool ODark, ADark;
};
class HelloWorld : public cocos2d::Scene
{
public:
	
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);


	//以下开始自定义了
	void regret(Ref* ref);
	void advance(Ref* ref);//这个是前进按钮
	void setSituation(Ref* ref,Menu* m); //这个是摆局时的初始化工作，具体化工作不在这里
	void setOK(Ref*, Menu*, Menu*);

	void initSituation(Ref*);//初始化棋盘局面
	void updateL();      //用来更新棋盘的局面之层
	bool checkClick(int x, int y)//检测点击的是否是轮行方的棋子，是返真，否返假
	{
		int a = (_situation[y][x]);
		int b = (rc << _turn);
		if (( a&b ) == b)
			return true;
		else return false;
	}

	char _situation[10][9];//这个代表局面数组
	enum {rc=16,rm,rx,rs,shuai,rp,rb,anhong,bc=32,bm,bx,bs,jiang,bp,bz,anhei}; //枚举棋子的类型
	int _originalX, _originalY, _aimX, _aimY;//分别是原始坐标和目的地坐标，用它们来走棋
	short _select;//代表选取的棋子类型
	bool _turn;   //true是轮到黑方走棋，false是轮到红方走棋
	bool _darkMark[10][9];//这个是暗子在棋盘上的标记
	bool _darkStone[2][15];//这两个是红黑双方的暗子兵种，用来辅助记录已现世的暗子（true代表还存在）
	bool _shuaiExist, _jiangExist;
	bool _seting;  //这个代表局面是否在摆设棋局中
	int _rStoneCount, _bStoneCount;//这两个用来计算摆局时已经摆放的棋子（不能大于16）

	Layer* _myL;      //这个是层精灵，用来表示棋盘局面的
	Widget* _UI;  //这个是画布，摆设棋局时要用到（里面是各种棋子“按钮”）
	Sprite* _sacrificeStone[2][15];//红黑双方牺牲的棋子。
	Sprite* _invalidMove;
	Sprite* _rw, *_bw, *_harmony;//创建3个精灵，一个是红胜小图，一个是黑胜小图,一个和棋小图


private:
	bool checkMove();
	bool ruleOfChe();
	bool ruleOfMa();
	bool ruleOfXiang();
	bool ruleOfShi();
	bool ruleOfKing();
	bool ruleOfPao();
	bool ruleOfBing();

	int randBorn();   //这个决定移动暗子时，会随机翻成什么兵种
	void setFlag(int,bool);//这个设置可能出现的兵种，暗子变明或摆局时用到
	void setStone();  //这个才是摆局时的具体化工作
	void initSacrifice(); //初始化牺牲棋子们的精灵
	bool checkKing(); //检测一下王不见王，见了返回真，不然返回假


	//以下开始第三个文件中内容的开发了，主要包括同局判定，长捉长将判定等
private:
	int _invalidStepCount[2], //用来统计犯规次数，0是红棋的，1是黑棋的
		_emptyStepCount,     //用来统计空步数（不吃子的走棋）
		_stepCount;     //用来记录棋步
	Step _step[300];//这个是棋谱记录，最多只能记录300步棋

	Situation * _siNumP[2]; //两个局面链，0下标是红棋的，1下标是黑棋的

	bool checkGreedy(); //用这个函数检查行棋方是否在长将或者长捉
	bool checkOver(bool);//检查帅或者将是不是被吃掉了
	void announceWin(int); //宣布某一方胜利
	bool checkSituation(); //用这个函数来检查走棋后局面是不是相同了
	bool reviewCheck(int x, int y);//用这个函数来复查被威胁的棋子在上一步是不是安全的
	void spliceNode(int i, Situation * tp);//用这个函数来拼接局面链
	void delSitua(Situation * tp); //用这个函数来递归释放局面链
	unsigned long long getNum(int y); //用这个函数来获取某一行的局面数学值
};

#endif // __HELLOWORLD_SCENE_H__
