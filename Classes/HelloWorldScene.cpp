#include "HelloWorldScene.h"
using namespace cocos2d::extension;

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
	auto visibleSize = Director::getInstance()->getVisibleSize();

	_turn = 0;  //默认红方开始先行
	_select = 0;//默认没有选中任何棋子
	_seting = false; //默认没有在摆局状态
	_stepCount = 0;  //默认没有走棋；
	
	_myL = Layer::create(); //创建一个层，用来显示棋盘上的棋子
	/*updateL(); */
	_myL->setPosition(OFFSETX+28, OFFSETY+28); //将这个层重叠在背景棋盘之上
	addChild(_myL, 9);

	Sprite* desk = Sprite::create("floor.png");//首先设置好背景
	desk->setPosition(visibleSize.width/2,visibleSize.height/2);
	desk->setScale(visibleSize.width / desk->getContentSize().width);
	addChild(desk);

	Sprite* board = Sprite::create("board.png"); //然后加载棋盘图
	board->setAnchorPoint(Vec2(0, 0));
	board->setPosition(OFFSETX, OFFSETY);
	addChild(board);

	Menu* menu = Menu::create(); //这个是加载菜单按钮选项
	menu->setPosition(600, 200);
	addChild(menu);

	MenuItem* start = MenuItemImage::create("start.jpg", "start.jpg", 
		CC_CALLBACK_1(HelloWorld::initSituation, this));
	start->setPosition(0, 0);
	menu->addChild(start); //先加载一个开始按钮

	MenuItem* r = MenuItemImage::create("regret.jpg", "regret.jpg", 
		CC_CALLBACK_1(HelloWorld::regret, this));
	r->setPosition(0, 70);
	menu->addChild(r); //再加载一个悔棋按钮

	MenuItem* advance = MenuItemImage::create("advance.jpg", "advance.jpg",
		CC_CALLBACK_1(HelloWorld::advance, this));
	advance->setPosition(0, 120);
	menu->addChild(advance); //再加载一个前进按钮，对应悔棋的

	MenuItem* setsituation = MenuItemImage::create("setsituation.jpg", "setsituation.jpg",
		CC_CALLBACK_1(HelloWorld::setSituation, this, menu));
	setsituation->setPosition(0, 200);
	menu->addChild(setsituation); //最后再加载一个编辑棋局的按钮
	setsituation->setScale(114.f / 456.f); //因为图片被我动过，这里要重新调整这个编辑按钮的外观
	
	_UI = cocostudio::GUIReader::getInstance()->
		widgetFromJsonFile("ChessUI_1.ExportJson"); //先把画布设置好,摆局时要用到（正常状态下不用）
	_UI->setVisible(false);
	addChild(_UI,99);//这个布局画布应该是最靠前的了

	_invalidMove = Sprite::create("invalid.jpg");
	_invalidMove->setPosition(300, 350);
	_invalidMove->setVisible(false);
	addChild(_invalidMove,1000);
	
	_rw = Sprite::create("redwin.jpg");_bw = Sprite::create("blackwin.jpg");_harmony = Sprite::create("harmony.jpg");
	_rw->setPosition(300, 350);        _bw->setPosition(300, 350);          _harmony->setPosition(300, 350);
	_rw->setVisible(false);            _bw->setVisible(false);              _harmony->setVisible(false);
	addChild(_rw, 1000);               addChild(_bw, 1000);                 addChild(_harmony, 1000);
	_rw->setScale(5);                  _bw->setScale(5);                    _harmony->setScale(5);

	initSacrifice(); //最后再初始化一下统计暗子

	
	//以下这段代码中的内容，有的只是为了输出触点的坐标，APP成功后删除掉
	auto l = EventListenerTouchOneByOne::create();
	l->onTouchBegan = [&](Touch* t, Event* e)
	{
		Point pos1 = t->getLocation();
		Point pos2 = t->getLocationInView();
		Point pos3 = Director::getInstance()->convertToGL(pos2);
		log("pos1.x=%f,pos1.y=%f", pos1.x, pos1.y);
		log("pos2.x=%f,pos2.y=%f", pos2.x, pos2.y);
		log("pos3.x=%f,pos3.y=%f", pos3.x, pos3.y);

		
		if (pos1.x > OFFSETX&&pos1.x<OFFSETX + 56 * 9 
			&& pos1.y>OFFSETY&&pos1.y < OFFSETY + 560) //如果触点在棋盘之内。。。
		{
			_invalidMove->setVisible(false);
			if (_seting) //如果游戏在摆设棋局状态
			{
				_originalX = (pos1.x - OFFSETX) / 56;
				_originalY = (pos1.y - OFFSETY) / 56;

				if (_situation[_originalY][_originalX])//如果点击的是已经存在的棋子，那么消掉它
				{
					if (bc <= _situation[_originalY][_originalX])//如果该棋子是黑的
					{
						_turn = 1;
						//setFlag(_situation[_originalY][_originalX], true);//恢复该棋子的可选状态
						_bStoneCount--;
					}
					else//不然把红方计数变量减一
					{
						_turn = 0;
						//setFlag(_situation[_originalY][_originalX], true);//恢复该棋子的可选状态
						_rStoneCount--;
					}
					_situation[_originalY][_originalX] = 0; //消掉该棋子
				}
				else //不然点击的是空地，那么弹出画布用来摆局
				{
					if (_originalY > 6) _UI->setPositionY(pos1.y - 256);
					else   _UI->setPositionY(pos1.y);
					if (_originalX > 4) _UI->setPositionX(pos1.x - 256);
					else   _UI->setPositionX(pos1.x);
					_UI->setVisible(true);
				}
				updateL(); //更新一下棋盘
			}
			else   //不然游戏就是对战状态
			{
				if (checkClick((pos1.x - OFFSETX) / 56, (pos1.y - OFFSETY) / 56) == true)//如果点击的是轮行方棋子
				{
					_originalX = (pos1.x - OFFSETX) / 56;
					_originalY = (pos1.y - OFFSETY) / 56;
					_select = _situation[_originalY][_originalX];

					//以下几行代码是记录棋谱
					_step[_stepCount].OX = _originalX;
					_step[_stepCount].OY = _originalY;
					_step[_stepCount].ONum = _step[_stepCount].DONum = _select; 
					_step[_stepCount].ODark = false;
					
				}
				else if (_select) //如果已经选取了棋子（那么这个点击可能就是移动该棋子了）
				{
					if (0 == _situation[(int)(pos1.y - OFFSETY) / 56][(int)(pos1.x - OFFSETX) / 56]//如果点击的是空地
						||                                                                         //或者
						false == checkClick((pos1.x - OFFSETX) / 56, (pos1.y - OFFSETY) / 56))     //点击的不是轮行方棋子
					{
						_aimX = (pos1.x - OFFSETX) / 56;
						_aimY = (pos1.y - OFFSETY) / 56;
						if (checkMove()) //如果走法成立
						{
							//以下几行代码是记录棋谱
							_step[_stepCount].AX = _aimX;_step[_stepCount].AY = _aimY;
							_step[_stepCount].ANum = _situation[_aimY][_aimX];
							_step[_stepCount].ADark = false;

							if (_situation[_aimY][_aimX]) _emptyStepCount = 0;//吃子了，无效步清零

							if (_darkMark[_originalY][_originalX]) //如果原先点击的是暗子
							{
								_select = randBorn(); //随机生成兵种
								_step[_stepCount].DONum = _select; //记录下新翻出的棋子，方便悔棋等功能
								_situation[_aimY][_aimX] = _select; //让新棋子移动到目标位置
								_darkMark[_originalY][_originalX] = false; //把原先的暗子位置变成没有暗子
								setFlag(_select,false);  //重新设置剩余的暗子可能兵种

								_step[_stepCount].ODark = true;//别忘了棋谱上的对应位置要记录成有暗子
							}
							else
								_situation[_aimY][_aimX] = _select;//把棋子移动到目标位置

							if (_darkMark[_aimY][_aimX]) //如果目标位置是暗子（吃掉该暗子）
							{
								_step[_stepCount].DANum = _situation[_originalY][_originalX];//记录下这个被吃的暗子，方便悔棋
								_step[_stepCount].ADark = true;//棋谱记录吃了一个暗子，方便悔棋
								_darkMark[_aimY][_aimX] = false; //把这里的暗子位置变成没有暗子
								_situation[_originalY][_originalX] = 0; //再把原来位置清空

								_step[_stepCount].ADark = true; //别忘了棋谱上的对应位置要记录成有暗子
							}
							else
								_situation[_originalY][_originalX] = 0; //再把原来位置清空

							updateL();//更新一下局面
							
							_turn = !_turn; //然后再更换红黑走棋权
							_stepCount++;   //棋谱步数+1
							_emptyStepCount++;//无效步数+1
							
							if (checkKing()) //检测一下走棋后是不是王不见王
								regret(nullptr); //如果见了，回一步棋

							if (checkSituation()) //如果局面相同了
							{
								if (checkGreedy()) //如果走棋方犯规了
								{
									_invalidMove->setVisible(true);
									_invalidStepCount[!_turn]++;
									if (5 < _invalidStepCount[!_turn]) announceWin(_turn);
								}
							}
							else //如果局面没有相同
							{
								_invalidStepCount[!_turn] = 0;//清空走棋方的违规记录
								checkOver(_turn); //看看走棋方是不是把人家的王杀了
							}
							if (_emptyStepCount >= 60) announceWin(3);//无效步数超过60，强制和棋
						}
					}
					_select = 0; //点击移动失败，重置选取棋子状态（没选中任何棋子）
				}
			}
		}
		
		
		return true;
	};//lamdam函数到这里结束了

	_eventDispatcher->addEventListenerWithSceneGraphPriority(l, this);
    return true;
}


void HelloWorld::initSituation(Ref* ref) //初始化棋盘局面，对应新局按钮的
{
	_turn = false; //默认红棋先走
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++) _situation[y][x] = 0; //清空局面
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++) _darkMark[y][x] = false;//清空所有暗子标记


	_situation[0][0] = rc, _situation[0][1] = rm, _situation[0][2] = rx, _situation[0][3] = rs; 
	_situation[0][5] = rs, _situation[0][6] = rx, _situation[0][7] = rm, _situation[0][8] = rc;
	_situation[2][1] = _situation[2][7] = rp;//初始化黑棋的双炮位置
	_situation[3][0] = _situation[3][2] = _situation[3][4] = _situation[3][6] = _situation[3][8] = rb;
	_situation[0][4] = shuai, _shuaiExist = true; //最后是红帅的初始化

	_situation[9][0] = bc, _situation[9][1] = bm, _situation[9][2] = bx, _situation[9][3] = bs; 
	_situation[9][5] = bs, _situation[9][6] = bx, _situation[9][7] = bm, _situation[9][8] = bc;
	_situation[7][1] = _situation[7][7] = bp;//初始化黑棋的双炮位置
	_situation[6][0] = _situation[6][2] = _situation[6][4] = _situation[6][6] = _situation[6][8] = bz;
	_situation[9][4] = jiang, _jiangExist = true;

	////////////以下是初始化暗子的位置//////////////
	_darkMark[0][0] = true, _darkMark[0][1] = true, _darkMark[0][2] = true, _darkMark[0][3] = true;
	_darkMark[0][5] = true, _darkMark[0][6] = true, _darkMark[0][7] = true, _darkMark[0][8] = true;
	_darkMark[2][1] = _darkMark[2][7] = true;//初始化黑棋的双炮位置
	_darkMark[3][0] = _darkMark[3][2] = _darkMark[3][4] = _darkMark[3][6] = _darkMark[3][8] = true;

	_darkMark[9][0] = true, _darkMark[9][1] = true, _darkMark[9][2] = true, _darkMark[9][3] = true;
	_darkMark[9][5] = true, _darkMark[9][6] = true, _darkMark[9][7] = true, _darkMark[9][8] = true;
	_darkMark[7][1] = _darkMark[7][7] = true;//初始化黑棋的双炮位置
	_darkMark[6][0] = _darkMark[6][2] = _darkMark[6][4] = _darkMark[6][6] = _darkMark[6][8] = true;

	for (int i = 0;i < 15;i++)
		_darkStone[0][i] = _darkStone[1][i] = true; //设置好所有的暗子标记位置

	_siNumP[0] = new Situation(255); //之所以用255这个数，是因为最下面那一行不可能出现两个王
	_siNumP[1] = new Situation(255);

	for (int c = 0;c < 300;c++)_step[c].ONum = 255; //清空棋谱记录
	_stepCount = 0;//清空棋步计数
	_rw->setVisible(false);            _bw->setVisible(false);  _harmony->setVisible(false);//隐藏双方的宣布胜利图
	_invalidStepCount[0] = _invalidStepCount[1] = 0;//清空双方的违规记录
	_emptyStepCount = 0;
	updateL();
}

void HelloWorld::updateL() //更新局面之层
{
	_myL->removeAllChildren();
	for (int i = 0;i < 15;i++)
	{
		_sacrificeStone[_turn][i]->setVisible(_darkStone[_turn][i]);
	}
	for(int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++)
		{
			
			switch (_situation[y][x])
			{
				Sprite* s14, *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9, *s10, *s11, *s12, *s13;
			
			case rc:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhong.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else
				{
					s1 = Sprite::create("rche.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				break;
			case rm:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhong.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else
				{
					s2 = Sprite::create("rma.png");
					s2->setPosition(56 * x, 56 * y);
					_myL->addChild(s2);
				}
				break;
			case rx:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhong.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else
				{
					s3 = Sprite::create("rxiang.png");
					s3->setPosition(56 * x, 56 * y);
					_myL->addChild(s3);
				}
				break;
			case rs:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhong.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else
				{
					s4 = Sprite::create("rshi.png");
					s4->setPosition(56 * x, 56 * y);
					_myL->addChild(s4);
				}
				break;
			case shuai:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhong.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else
				{
					s5 = Sprite::create("rshuai.png");
					s5->setPosition(56 * x, 56 * y);
					_myL->addChild(s5);
				}
				break;
			case rp:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhong.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else
				{
					s6 = Sprite::create("rpao.png");
					s6->setPosition(56 * x, 56 * y);
					_myL->addChild(s6);
				}
				break;
			case rb:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhong.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else
				{
					s7 = Sprite::create("rbing.png");
					s7->setPosition(56 * x, 56 * y);
					_myL->addChild(s7);
				}
				break;
			case bc:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhei.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else
				{
					s8 = Sprite::create("bche.png");
					s8->setPosition(56 * x, 56 * y);
					_myL->addChild(s8);
				}
				break;
			case bm:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhei.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else {
					s9 = Sprite::create("bma.png");
					s9->setPosition(56 * x, 56 * y);
					_myL->addChild(s9);
				}
				break;
			case bx:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhei.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else {
					s10 = Sprite::create("bxiang.png");
					s10->setPosition(56 * x, 56 * y);
					_myL->addChild(s10);
				}
				break;
			case bs:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhei.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else {
					s11 = Sprite::create("bshi.png");
					s11->setPosition(56 * x, 56 * y);
					_myL->addChild(s11);
				}
				break;
			case jiang:
				 s12 = Sprite::create("bjiang.png");
				s12->setPosition(56 * x, 56 * y);
				_myL->addChild(s12);
				break;
			case bp:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhei.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else {
					s13 = Sprite::create("bpao.png");
					s13->setPosition(56 * x, 56 * y);
					_myL->addChild(s13);
				}
				break;
			case bz:
				if (_darkMark[y][x]) //如果暗子标记还存在
				{
					s1 = Sprite::create("anhei.png");
					s1->setPosition(56 * x, 56 * y);
					_myL->addChild(s1);
				}
				else {
					s14 = Sprite::create("bzu.png");
					s14->setPosition(56 * x, 56 * y);
					_myL->addChild(s14);
				}
				break;
			}
		}
}



























//#pragma once
//#include "HelloWorldScene.h"

bool HelloWorld::checkMove()
{
	switch (_select)
	{
	case rc:
	case bc:
		return ruleOfChe();

	case rm:
	case bm:
		return ruleOfMa();

	case rx:
	case bx:
		return ruleOfXiang();

	case rs:
	case bs:
		return ruleOfShi();

	case shuai:
	case jiang:
		return ruleOfKing();

	case rp:
	case bp:
		return ruleOfPao();

	case rb:
	case bz:
		return ruleOfBing();
	}
	return false;
}

bool HelloWorld::ruleOfChe() //车的走法
{
	if (_originalX == _aimX) //如果在横线相同
	{
		int min = _originalY < _aimY ? _originalY : _aimY;
		int max = _originalY > _aimY ? _originalY : _aimY;
		while (++min < max)
		{
			if (_situation[min][_aimX])
				return false;
		}
		return true;
	}
	else if (_originalY == _aimY) //如果在横线相同
	{
		int min = _originalX < _aimX ? _originalX : _aimX;
		int max = _originalX > _aimX ? _originalX : _aimX;
		while (++min < max)
		{
			if (_situation[_aimY][min]) return false;
		}
		return true;
	}
	return false;
}

bool HelloWorld::ruleOfMa() //马的走法
{
	if (!_situation[_originalY + 1][_originalX])//如果上一格马腿是空
	{
		if (_aimY - _originalY == 2)
			return 1 == (_aimX - _originalX)*(_aimX - _originalX);
	}
	if (!_situation[_originalY - 1][_originalX])//如果下一格马腿是空
	{
		if (2 == _originalY - _aimY)
			return 1 == (_aimX - _originalX)*(_aimX - _originalX);
	}
	if (!_situation[_originalY][_originalX + 1])//如果右一格马腿是空
	{
		if (2 == _aimX - _originalX)
			return 1 == (_aimY - _originalY)*(_aimY - _originalY);
	}
	if (!_situation[_originalY][_originalX - 1])//如果左一格马腿是空
	{
		if (-2 == _aimX - _originalX)
			return 1 == (_aimY - _originalY)*(_aimY - _originalY);
	}
	return false;
}

bool HelloWorld::ruleOfXiang()//相的走法
{
	if (!_situation[_originalY + 1][_originalX + 1])//如果右上的象眼为空
	{
		if ((2 == _aimY - _originalY) && (2 == _aimX - _originalX))
			return true;
	}
	if (!_situation[_originalY + 1][_originalX - 1])//如果左上的象眼为空
	{
		if ((2 == _aimY - _originalY) && (2 == _originalX - _aimX))
			return true;
	}
	if (!_situation[_originalY - 1][_originalX + 1])//如果右下的象眼为空
	{
		if ((2 == _originalY - _aimY) && (2 == _aimX - _originalX))
			return true;
	}
	if (!_situation[_originalY - 1][_originalX - 1])//如果左下的象眼为空
	{
		if ((2 == _originalY - _aimY) && (2 == _originalX - _aimX))
			return true;
	}
	return false;
}

bool HelloWorld::ruleOfShi()//士的走法
{
	return 1 == (_aimY - _originalY)*(_aimY - _originalY)*(_aimX - _originalX)*(_aimX - _originalX);
}

bool HelloWorld::ruleOfKing()
{
	if (_turn)//这是黑将的走法
	{
		if (_aimX > 2 && _aimX < 6 && _aimY>6)//如果是在九宫之内
		{
			if (_aimX == _originalX &&
				1 == (_aimY - _originalY)*(_aimY - _originalY))
				return true;
			if (_aimY == _originalY &&
				1 == (_aimX - _originalX)*(_aimX - _originalX))
				return true;
		}
	}
	else //这是红帅的走法
	{
		if (_aimX > 2 && _aimX < 6 && _aimY<3)//如果是在九宫之内
		{
			if (_aimX == _originalX &&
				1 == (_aimY - _originalY)*(_aimY - _originalY))
				return true;
			if (_aimY == _originalY &&
				1 == (_aimX - _originalX)*(_aimX - _originalX))
				return true;
		}
	}
	return false;
}

bool HelloWorld::ruleOfPao() //炮的走法
{
	if (_situation[_aimY][_aimX]) //如果目标位置是棋子（只能是敌人棋子），那么。。。
	{
		int count = 0;
		if (_originalX == _aimX)
		{
			int min = _originalY < _aimY ? _originalY : _aimY;
			int max = _originalY > _aimY ? _originalY : _aimY;
			while (++min < max)
			{
				if (_situation[min][_aimX])
					count++;
			}
			return 1 == count;
		}
		else if (_originalY == _aimY)
		{
			int min = _originalX < _aimX ? _originalX : _aimX;
			int max = _originalX > _aimX ? _originalX : _aimX;
			while (++min < max)
			{
				if (_situation[_aimY][min])
					count++;
			}
			return 1 == count;
		}
		return false;
	}
	else //如果目标位置是空地，那么炮的检测就是以下。。。
	{
		if (_originalX == _aimX) //如果在横线相同
		{
			int min = _originalY < _aimY ? _originalY : _aimY;
			int max = _originalY > _aimY ? _originalY : _aimY;
			while (++min < max)
			{
				if (_situation[min][_aimX])
					return false;
			}
			return true;
		}
		else if (_originalY == _aimY) //如果在横线相同
		{
			int min = _originalX < _aimX ? _originalX : _aimX;
			int max = _originalX > _aimX ? _originalX : _aimX;
			while (++min < max)
			{
				if (_situation[_aimY][min]) return false;
			}
			return true;
		}
		return false;
	}
}

bool HelloWorld::ruleOfBing()
{
	if (_turn)//这是卒子的走法
	{
		if (5 > _originalY) //如果小卒过河了
		{
			if (_aimX == _originalX && -1 == _aimY - _originalY)
				return true;
			if (_aimY == _originalY &&
				1 == (_aimX - _originalX)*(_aimX - _originalX))
				return true;
		}
		else //不然
		{
			if (_aimX == _originalX && -1 == _aimY - _originalY)
				return true;
		}
	}
	else //这是小兵的走法
	{
		if (5 <= _originalY) //如果小兵过河了
		{
			if (_aimX == _originalX && 1 == _aimY - _originalY)
				return true;
			if (_aimY == _originalY &&
				1 == (_aimX - _originalX)*(_aimX - _originalX))
				return true;
		}
		else //不然
		{
			if (_aimX == _originalX && 1 == _aimY - _originalY)
				return true;
		}
	}
	return false;
}


int HelloWorld::randBorn()
{
	int i = 0;
	srand((unsigned)time(NULL));
start:
	i = rand() % 15;
	switch (i)
	{
	case 0:case 1:case 2:case 3:case 4:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 6; //返回兵或卒
		break;
	case 5:case 6:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 0; //返回车
		break;
	case 7: case 8:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 1; //返回马
		break;
	case 9:case 10:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 2; //返回象
		break;
	case 11:case 12:
		if (_darkStone[_turn][i]) //返回士
			return (rc << _turn) + 3;
		break;
	case 13:case 14:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 5; //返回炮
		break;
	}
	goto start;
}

void HelloWorld::setFlag(int i, bool b)
{
	switch (i)
	{
	case rc:case bc: //设置双方的车有没有现世的可能
		if (!b == _darkStone[_turn][5])
			_darkStone[_turn][5] = b;
		else _darkStone[_turn][6] = b; //这里不用加if判断也行
		break;

	case rm:case bm: //设置双反的马
		if (!b == _darkStone[_turn][7])
			_darkStone[_turn][7] = b;
		else if (!b == _darkStone[_turn][8])
			_darkStone[_turn][8] = b;
		break;

	case rx:case bx: //设置相
		if (!b == _darkStone[_turn][9])
			_darkStone[_turn][9] = b;
		else _darkStone[_turn][10] = b;
		break;

	case rs:case bs: //设置士
		if (!b == _darkStone[_turn][11])
			_darkStone[_turn][11] = b;
		else _darkStone[_turn][12] = b;
		break;

	case rp:case bp: //设置炮
		if (!b == _darkStone[_turn][13])
			_darkStone[_turn][13] = b;
		else _darkStone[_turn][14] = b;
		break;

	case rb:case bz: //设置兵卒
		for (int n = 0;n < 5;n++)
		{
			if (!b == _darkStone[_turn][n])
			{
				_darkStone[_turn][n] = b;
				return;
			}
		}
	}
}

bool HelloWorld::checkKing()
{
	for(int y=0;y<10;y++)
		for (int x = 0;x < 9;x++)
		{
			if (_situation[y][x] == shuai)
			{
				for (int a = y + 1;a < 10;a++)
				{
					if (_situation[a][x] == jiang) return true;
					else if (_situation[a][x]) return false;
				}
			}
		}
	return false;
}

void HelloWorld::setSituation(Ref* r, Menu* m)
{
	_seting = true; //把游戏设置为摆局状态
	setStone(); //现在调用这个函数是为了把那些lambdn函数注册了
	delSitua(_siNumP[0]); delSitua(_siNumP[1]); //递归释放以前走棋建立的局面链
	_rStoneCount = _bStoneCount = 0; //把棋子计数变量清零，每摆放一个棋子，对应一方的变量+1
	_shuaiExist = _jiangExist = false;
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++) _situation[y][x] = 0;
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++) _darkMark[y][x] = false; //清空棋盘与暗子标记


														   ////////////以下重新初始化暗子的位置//////////////
	_darkMark[0][0] = true, _darkMark[0][1] = true, _darkMark[0][2] = true, _darkMark[0][3] = true;
	_darkMark[0][5] = true, _darkMark[0][6] = true, _darkMark[0][7] = true, _darkMark[0][8] = true;
	_darkMark[2][1] = _darkMark[2][7] = true;//初始化黑棋的双炮位置
	_darkMark[3][0] = _darkMark[3][2] = _darkMark[3][4] = _darkMark[3][6] = _darkMark[3][8] = true;

	_darkMark[9][0] = true, _darkMark[9][1] = true, _darkMark[9][2] = true, _darkMark[9][3] = true;
	_darkMark[9][5] = true, _darkMark[9][6] = true, _darkMark[9][7] = true, _darkMark[9][8] = true;
	_darkMark[7][1] = _darkMark[7][7] = true;//初始化黑棋的双炮位置
	_darkMark[6][0] = _darkMark[6][2] = _darkMark[6][4] = _darkMark[6][6] = _darkMark[6][8] = true;

	for (int i = 0;i < 15;i++)
		_darkStone[0][i] = _darkStone[1][i] = true;

	m->setVisible(false);
	Menu* mm = Menu::create();
	MenuItem* MI = MenuItemImage::create("setOK.jpg", "setOK.jpg",
		CC_CALLBACK_1(HelloWorld::setOK, this, m, mm));
	mm->setPosition(600, 300);
	mm->addChild(MI);
	addChild(mm);

	updateL();
}

void HelloWorld::setStone()
{
	Button* rcbtn = (Button*)Helper::seekWidgetByName(_UI, "rc"); //这两个是红车的摆设
	rcbtn->addClickEventListener([&](Ref* r) {
		if (_rStoneCount < 15 && (_darkStone[0][5] || _darkStone[0][6]))
		{
			_situation[_originalY][_originalX] = rc;
			_darkMark[_originalY][_originalX] = false;
			_turn = 0;
			setFlag(rc, false);
			_rStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* rmbtn = (Button*)Helper::seekWidgetByName(_UI, "rm");
	rmbtn->addClickEventListener([&](Ref* r) {
		if (_rStoneCount < 15 && (_darkStone[0][7] || _darkStone[0][8]))
		{
			_situation[_originalY][_originalX] = rm;
			_darkMark[_originalY][_originalX] = false;
			_turn = 0;
			setFlag(rm, false);
			_rStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* rxbtn = (Button*)Helper::seekWidgetByName(_UI, "rx");
	rxbtn->addClickEventListener([&](Ref* r) {
		if (_rStoneCount < 15 && (_darkStone[0][9] || _darkStone[0][10]))
		{
			_situation[_originalY][_originalX] = rx;
			_darkMark[_originalY][_originalX] = false;
			_turn = 0;
			setFlag(rx, false);
			_rStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* rsbtn = (Button*)Helper::seekWidgetByName(_UI, "rs");
	rsbtn->addClickEventListener([&](Ref* r) {
		if (_rStoneCount < 15 && (_darkStone[0][11] || _darkStone[0][12]))
		{
			_situation[_originalY][_originalX] = rs;
			_darkMark[_originalY][_originalX] = false;
			_turn = 0;
			setFlag(rs, false);
			_rStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* shuaibtn = (Button*)Helper::seekWidgetByName(_UI, "shuai");
	shuaibtn->addClickEventListener([&](Ref* r) {
		if (!_shuaiExist && _originalX > 2 && _originalX < 6 && _originalY<3)
		{
			_situation[_originalY][_originalX] = shuai;
			_darkMark[_originalY][_originalX] = false;
			_shuaiExist = true;
			_UI->setVisible(false);
		}
		updateL();
	});

	Button* rpbtn = (Button*)Helper::seekWidgetByName(_UI, "rp");
	rpbtn->addClickEventListener([&](Ref* r) {
		if (_rStoneCount < 15 && (_darkStone[0][13] || _darkStone[0][14]))
		{
			_situation[_originalY][_originalX] = rp;
			_darkMark[_originalY][_originalX] = false;
			_turn = 0;
			setFlag(rp, false);
			_rStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* rbbtn = (Button*)Helper::seekWidgetByName(_UI, "rb");
	rbbtn->addClickEventListener([&](Ref* r) {
		if (_rStoneCount < 15)
		{
			int i = 0;
			while (i < 5)
			{
				if (_darkStone[0][i])
				{
					_situation[_originalY][_originalX] = rb;
					_darkMark[_originalY][_originalX] = false;
					_darkStone[0][i] = false;
					break;
				}
				i++;
			}
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* anhongbtn = (Button*)Helper::seekWidgetByName(_UI, "anhong");
	anhongbtn->addClickEventListener([&](Ref* r) {
		if (_darkMark[_originalY][_originalX] && _originalY < 4)
		{
			if (3 == _originalY) 
				_situation[_originalY][_originalX] = rb;
			else if (2 == _originalY) _situation[_originalY][_originalX] = rp;
			else switch (_originalX)
			{
			case 0:case 8: _situation[_originalY][_originalX] = rc;
				break;
			case 1:case 7: _situation[_originalY][_originalX] = rm;
				break;
			case 2:case 6: _situation[_originalY][_originalX] = rx;
				break;
			case 3:case 5: _situation[_originalY][_originalX] = rs;
				break;
			}
			_UI->setVisible(false);
			_rStoneCount++;
		}
		updateL();
	});

	Button* bcbtn = (Button*)Helper::seekWidgetByName(_UI, "bc");
	bcbtn->addClickEventListener([&](Ref* r) {
		if (_bStoneCount < 15 && (_darkStone[1][5] || _darkStone[1][6]))
		{
			_situation[_originalY][_originalX] = bc;
			_darkMark[_originalY][_originalX] = false;
			_turn = 1;
			setFlag(bc, false);
			_bStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* bmbtn = (Button*)Helper::seekWidgetByName(_UI, "bm");
	bmbtn->addClickEventListener([&](Ref* r) {
		if (_bStoneCount < 15 && (_darkStone[1][7] || _darkStone[1][8]))
		{
			_situation[_originalY][_originalX] = bm;
			_darkMark[_originalY][_originalX] = false;
			_turn = 1;
			setFlag(bm, false);
			_bStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* bxbtn = (Button*)Helper::seekWidgetByName(_UI, "bx");
	bxbtn->addClickEventListener([&](Ref* r) {
		if (_bStoneCount < 15 && (_darkStone[1][9] || _darkStone[1][10]))
		{
			_situation[_originalY][_originalX] = bx;
			_darkMark[_originalY][_originalX] = false;
			_turn = 1;
			setFlag(bx, false);
			_bStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* bsbtn = (Button*)Helper::seekWidgetByName(_UI, "bs");
	bsbtn->addClickEventListener([&](Ref* r) {
		if (_bStoneCount < 15 && (_darkStone[1][11] || _darkStone[1][12]))
		{
			_situation[_originalY][_originalX] = bs;
			_darkMark[_originalY][_originalX] = false;
			_turn = 1;
			setFlag(bs, false);
			_bStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* jiangbtn = (Button*)Helper::seekWidgetByName(_UI, "jiang");
	jiangbtn->addClickEventListener([&](Ref* r) {
		if (!_jiangExist && _originalX > 2 && _originalX < 6 && _originalY>6)
		{
			_situation[_originalY][_originalX] = jiang;
			_darkMark[_originalY][_originalX] = false;
			_jiangExist = true;
			_UI->setVisible(false);
		}
		updateL();
	});

	Button* bpbtn = (Button*)Helper::seekWidgetByName(_UI, "bp");
	bpbtn->addClickEventListener([&](Ref* r) {
		if (_bStoneCount < 15 && (_darkStone[1][13] || _darkStone[1][14]))
		{
			_situation[_originalY][_originalX] = bp;
			_darkMark[_originalY][_originalX] = false;
			_turn = 1;
			setFlag(bp, false);
			_bStoneCount++;
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* bzbtn = (Button*)Helper::seekWidgetByName(_UI, "bz");
	bzbtn->addClickEventListener([&](Ref* r) {
		if (_rStoneCount < 15)
		{
			int i = 0;
			while (i < 5)
			{
				if (_darkStone[1][i])
				{
					_situation[_originalY][_originalX] = bz;
					_darkMark[_originalY][_originalX] = false;
					_darkStone[1][i] = false;
					break;
				}
				i++;
			}
		}
		_UI->setVisible(false);
		updateL();
	});

	Button* anheibtn = (Button*)Helper::seekWidgetByName(_UI, "anhei");
	anheibtn->addClickEventListener([&](Ref* r) {
		if (_darkMark[_originalY][_originalX] && _originalY > 5)
		{
			if (6 == _originalY) 
				_situation[_originalY][_originalX] = bz;
			else if (7 == _originalY) _situation[_originalY][_originalX] = bp;
			else switch (_originalX)
			{
			case 0:case 8: _situation[_originalY][_originalX] = bc;
				break;
			case 1:case 7: _situation[_originalY][_originalX] = bm;
				break;
			case 2:case 6: _situation[_originalY][_originalX] = bx;
				break;
			case 3:case 5: _situation[_originalY][_originalX] = bs;
				break;
			}
			_bStoneCount++;
			_UI->setVisible(false);
		}
		updateL();
	});
}

void HelloWorld::setOK(Ref* r, Menu* m1, Menu* m2)
{
	if (_shuaiExist && _jiangExist) //只有帅和将两个棋子都存在，才能摆局成功
	{
		m1->setVisible(true); //重载显示新局等菜单项
		_UI->setVisible(false); //摆局画布撤销掉
		removeChild(m2);      //摆局OK的菜单项消掉
		_seting = false;      //摆局结束
		_turn = 0;//设置为红先

		for (int y = 0;y < 10;y++) //把没有棋子的地方，暗子位置标记都给它清空
			for (int x = 0;x < 9;x++)
				if (!_situation[y][x])
					_darkMark[y][x] = false;

		_siNumP[0] = new Situation(255); //之所以用255这个数，是因为最下面那一行不可能出现两个王
		_siNumP[1] = new Situation(255); //别忘了给双方建立起来局面链，用来判定是否重复局面
		for (int c = 0;c < 300;c++)_step[c].ONum = 255; //清空棋谱记录
		_stepCount = 0;//清空棋步计数
		_rw->setVisible(false);            _bw->setVisible(false);  _harmony->setVisible(false);//隐藏双方的宣布胜利图
		_invalidStepCount[0] = _invalidStepCount[1] = 0;//清空双方的违规记录
		_emptyStepCount = 0; //清空无效步数记录
	}
}

void HelloWorld::regret(Ref* ref)//悔棋的函数
{
	if (_step[_stepCount - 1].ONum != 255 && _stepCount > 0)//如果还可以悔棋
	{
		_turn = !_turn;//先退回走棋权
		_situation[_step[_stepCount - 1].OY][_step[_stepCount - 1].OX] = _step[_stepCount - 1].ONum;//行动棋子回来
		_situation[_step[_stepCount - 1].AY][_step[_stepCount - 1].AX] = _step[_stepCount - 1].ANum;//复活被吃棋子（如果有的话）
		if(_step[_stepCount-1].ODark)//如果动的是暗子
		{
			setFlag(_step[_stepCount - 1].DONum, true);//那么先把那个暗子盖回去（恢复该棋种的出生权利）
			_darkMark[_step[_stepCount - 1].OY][_step[_stepCount - 1].OX] = true;//然后该位置设置为暗子
		}
		if (_step[_stepCount - 1].ADark)//如果被吃的是暗子
		{
			_situation[_step[_stepCount - 1].OY][_step[_stepCount - 1].OX] = _step[_stepCount - 1].DANum;//恢复这个暗子棋种
			_darkMark[_step[_stepCount - 1].AY][_step[_stepCount - 1].AX] = true;//该位置设为为暗子
		}
		_stepCount--;//棋步记录-1
		_emptyStepCount--; //无效步数记录-1
		updateL();
	}
}

void HelloWorld::advance(Ref* ref)
{
	if (_step[_stepCount].ONum != 255 && _stepCount < 300)//如果还可以前进
	{
		_situation[_step[_stepCount].OY][_step[_stepCount].OX] = 0; //行动的位置清空
		_situation[_step[_stepCount].AY][_step[_stepCount].AX] = _step[_stepCount].DONum;//目标位置摆放行动棋子，或许是翻出的棋子
		_darkMark[_step[_stepCount].OY][_step[_stepCount].OX] = _darkMark[_step[_stepCount].AY][_step[_stepCount].AX] = false;
		_turn = !_turn;
		_stepCount++;
		updateL();
	}
}
/***************以上这些是additionFile1中的内容******************/












































//#include "HelloWorldScene.h"
//#define S 0.5;
void HelloWorld::initSacrifice()
{
	_sacrificeStone[0][0] = Sprite::create("rbing.png"); _sacrificeStone[0][1] = Sprite::create("rbing.png");
	_sacrificeStone[0][2] = Sprite::create("rbing.png");_sacrificeStone[0][3] = Sprite::create("rbing.png");
	_sacrificeStone[0][4] = Sprite::create("rbing.png");
	_sacrificeStone[0][5] = Sprite::create("rche.png");_sacrificeStone[0][6] = Sprite::create("rche.png");
	_sacrificeStone[0][7] = Sprite::create("rma.png");_sacrificeStone[0][8] = Sprite::create("rma.png");
	_sacrificeStone[0][9] = Sprite::create("rxiang.png");_sacrificeStone[0][10] = Sprite::create("rxiang.png");
	_sacrificeStone[0][11] = Sprite::create("rshi.png");_sacrificeStone[0][12] = Sprite::create("rshi.png");
	_sacrificeStone[0][13] = Sprite::create("rpao.png");_sacrificeStone[0][14] = Sprite::create("rpao.png");
	for (int i = 0;i < 15;i++)
	{
		_sacrificeStone[0][i]->setScale(0.6);
		_sacrificeStone[0][i]->setPosition(40 + 0.6 * 56 * i, 20);
		addChild(_sacrificeStone[0][i], 999);
	}

	_sacrificeStone[1][0] = Sprite::create("bzu.png");_sacrificeStone[1][1] = Sprite::create("bzu.png");
	_sacrificeStone[1][2] = Sprite::create("bzu.png");_sacrificeStone[1][3] = Sprite::create("bzu.png");
	_sacrificeStone[1][4] = Sprite::create("bzu.png");
	_sacrificeStone[1][5] = Sprite::create("bche.png");_sacrificeStone[1][6] = Sprite::create("bche.png");
	_sacrificeStone[1][7] = Sprite::create("bma.png");_sacrificeStone[1][8] = Sprite::create("bma.png");
	_sacrificeStone[1][9] = Sprite::create("bxiang.png");_sacrificeStone[1][10] = Sprite::create("bxiang.png");
	_sacrificeStone[1][11] = Sprite::create("bshi.png");_sacrificeStone[1][12] = Sprite::create("bshi.png");
	_sacrificeStone[1][13] = Sprite::create("bpao.png");_sacrificeStone[1][14] = Sprite::create("bpao.png");

	for (int i = 0;i < 15;i++)
	{
		_sacrificeStone[1][i]->setScale(0.6);
		_sacrificeStone[1][i]->setPosition(40 + 0.6 * 56 * i, 660);
		addChild(_sacrificeStone[1][i], 999);
	}
}

void HelloWorld::delSitua(Situation * tp) //递归释放堆中的局面链
{
	if (tp->p_branch != NULL) //看看同层分支的下一个节点是不是空
		delSitua(tp->p_branch);

	if (tp->p_next != NULL) //看看下一层是不是空
		delSitua(tp->p_next);

	delete tp;
	return;
}

bool HelloWorld::checkSituation()
{
	Situation * tempp = _siNumP[_turn];
	int i = 0;
start:
	if (10 == i) //如果全部局面相同。。。
		return true; //那么返回真，代表走成同局了
	else if (getNum(i) == tempp->sinum) //如果当前行数值相同
	{
		tempp = tempp->p_next;
		i++;
		goto start;
	}
	else if (NULL != tempp->p_branch) //如果当前行数学值不同但链表还有下一个同层分支
	{
		tempp = tempp->p_branch;
		goto start; //再循环测试一下该分支是不是相同
	}
	else //如果同层分支到尽头，那么是时候拼接我们的局面链表了，因为当前是没出现过的新局面
	{
		tempp->p_branch = new Situation(getNum(i));
		spliceNode(i + 1, tempp->p_branch);
		return false;
	}
	return false;
}

void HelloWorld::spliceNode(int i, Situation * tp)
{
	Situation * tempp = tp;
	while (i < 10)
	{
		tempp->p_next = new Situation(getNum(i));
		tempp = tempp->p_next;
		i++;
	}
	return;
}

unsigned long long HelloWorld::getNum(int y) //这是用来获取局面某一行数值的
{
	unsigned long long num = 0;
	for (int x = 0; x < 9; x++)
	{

		if (_darkMark[y][x])//如果当前格子上是暗子
		{
			if (_situation[y][x] & rc)//如果是红方暗子
				num = num * 16 + 7;
			else num = num * 16 + 8;
		}
		else {
			switch (_situation[y][x])
			{
			case 0:num = num * 16 + 0;break;
			case rc: num = num * 16 + 1;break;
			case rm: num = num * 16 + 2;break;
			case rx: num = num * 16 + 3;break;
			case rs: num = num * 16 + 4;break;
			case rp: num = num * 16 + 5;break;
			case rb: num = num * 16 + 6;break;
			case bc: num = num * 16 + 9;break;
			case bm: num = num * 16 + 10;break;
			case bx: num = num * 16 + 11;break;
			case bs: num = num * 16 + 12;break;
			case bp: num = num * 16 + 13;break;
			case bz: num = num * 16 + 14;break;
			case shuai:case jiang:
				num = num * 16 + 15;break;
			}
		}

	}

	return num;
}
/************************************以上是第二个文件中的内容***************************************/











































































































/******************************第三个文件中的内容********************************/

bool HelloWorld::checkGreedy()
{
	int flagArr[10][9]; //弄一个辅助数组，用来标记哪些位置是有根的（己方棋子火力覆盖）

	int x, y;

	bool battery = false;//这个是炮台，先设置为空

	for (y = 0;y < 10;y++)
		for (x = 0;x < 9;x++) flagArr[y][x] = 1;

	for (y = 0;y < 10;y++)
		for (x = 0;x < 9;x++)
		{
			if (_situation[y][x]) //如果该位置存在棋子
			{
				switch (_situation[y][x])
				{
				case rc:case bc:
					for (int tempy = y + 1;tempy < 10;tempy++)//车的向上走
					{
						if (_situation[tempy][x])//如果向上走碰到棋子，不论敌我
						{
							if (_situation[y][x] & (rc << _turn)) //如果发源地是本方的车，有根标记为真
								flagArr[tempy][x] = 0;
							else flagArr[tempy][x] *= 2; //发源地是敌人的车，危险标记为真
							break;
						}
					}
					for (int tempy = y - 1;tempy >= 0;tempy--)//车的向下走
					{
						if (_situation[tempy][x])//如果向下走碰到棋子，不论敌我
							if (_situation[tempy][x])//如果向下走碰到棋子，不论敌我，该路标记结束
							{
								if (_situation[y][x] & (rc << _turn)) //如果是本方的车
									flagArr[tempy][x] = 0; //有根标记为真
								else flagArr[tempy][x] *= 2; //是敌方的车，危险标记为真
								break;
							}
					}
					for (int tempx = x - 1;tempx >= 0;tempx--)//车的向左走
					{
						if (_situation[y][tempx]) //如果向左走碰到棋子，不论敌我，该路标记结束
						{
							if (_situation[y][x] & (rc << _turn))//如果发源地是本方的车
								flagArr[y][tempx] = 0; //有根标记为真
							else flagArr[y][tempx] *= 2;//不然发源地是敌人的车，危险标记为真
							break;
						}
					}
					for (int tempx = x + 1;tempx < 9;tempx++)//车的向右走
					{
						if (_situation[y][tempx]) //如果向右走碰到棋子，不论敌我，该路标记结束
						{
							if (_situation[y][x] & (rc << _turn))//如果发源地是本方的车
								flagArr[y][tempx] = 0; //有根标记为真
							else flagArr[y][tempx] *= 2;//不然发源地是敌人的车，危险标记为真
							break;
						}
					}
					break; //至此，车的检测结束

				case rm:case bm: //马的检测开始
					if (y < 8 && !_situation[y + 1][x])//如果上面有空间并且上马腿没有被蹩
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的马
						{
							//以下两行，设置有根标记
							flagArr[y + 2][x - 1] = 0;
							flagArr[y + 2][x + 1] = 0;
						}
						else //不然发源地是敌人的马
						{
							//以下两行，，设置危险标记
							flagArr[y + 2][x - 1] *= 2;
							flagArr[y + 2][x + 1] *= 2;
						}
					}

					if (y > 1 && !_situation[y - 1][x])//如果下面有空间并且下马腿没有被蹩
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的马
						{
							//以下两行，设置有根标记
							flagArr[y - 2][x - 1] = 0;
							flagArr[y - 2][x + 1] = 0;
						}
						else //不然发源地是敌人的马
						{
							//以下两行，设置危险标记
							flagArr[y - 2][x - 1] *= 2;
							flagArr[y - 2][x + 1] *= 2;
						}
					}

					if (x > 1 && !_situation[y][x - 1])//如果左边有空间并且左马腿没有被蹩
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的马
						{
							//以下两行，设置有根标记
							flagArr[y - 1][x - 2] = 0;
							flagArr[y + 1][x - 2] = 0;
						}
						else //不然发源地是敌人的马
						{
							//以下两行，设置危险标记
							flagArr[y - 1][x - 2] *= 2;
							flagArr[y + 1][x - 2] *= 2;
						}
					}

					if (x < 7 && !_situation[y][x + 1])//如果右边有空间并且右马腿没有被蹩
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的马
						{
							//以下两行，设置有根标记
							flagArr[y - 1][x + 2] = 0;
							flagArr[y + 1][x + 2] = 0;
						}
						else //不然发源地是敌人的马
						{
							//以下两行，设置危险标记
							flagArr[y - 1][x + 2] *= 2;
							flagArr[y + 1][x + 2] *= 2;
						}
					}
					break; //至此，马的检测结束

				case rx:case bx:
					if (y < 8 && x > 1 && !_situation[y + 1][x - 1])//如果左上有空间并且左上象眼没有被蹩
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的相或象
						{
							flagArr[y + 2][x - 2] = 0;
						}
						else //不然发源地就是敌人的相或象
						{
							flagArr[y + 2][x - 2] *= 2;
						}
					}

					if (y < 8 && x < 7 && !_situation[y + 1][x + 1])//如果右上有空间并且右上象眼没有被蹩
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的相或象
						{
							flagArr[y + 2][x + 2] = 0;
						}
						else //不然发源地就是敌人的相或象
						{
							flagArr[y + 2][x + 2] *= 2;
						}
					}

					if (y > 1 && x < 7 && !_situation[y - 1][x + 1])//如果右下有空间并且右下象眼没有被蹩
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的相或象
						{
							flagArr[y - 2][x + 2] = 0;
						}
						else //不然发源地就是敌人的相或象
						{
							flagArr[y - 2][x + 2] *= 2;
						}
					}

					if (y > 1 && x > 1 && !_situation[y - 1][x + 1])//如果左下有空间并且左下象眼没有被蹩
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的相或象
						{
							flagArr[y - 2][x - 2] = 0;
						}
						else //不然发源地就是敌人的相或象
						{
							flagArr[y - 2][x - 2] *= 2;
						}
					}
					break; //至此，相或象的检测结束

				case rs:case bs:
					if (y < 9 && x > 0)//如果左上有空间
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的士
						{
							flagArr[y + 1][x - 1] = 0;
						}
						else //不然就是敌人的士
						{
							flagArr[y + 1][x - 1] *= 2;
						}
					}

					if (y < 9 && x < 8)//如果右上有空间
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的士
						{
							flagArr[y + 1][x + 1] = 0;
						}
						else //不然就是敌人的士
						{
							flagArr[y + 1][x + 1] *= 2;
						}
					}

					if (y > 0 && x < 9)//如果右下有空间
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的士
						{
							flagArr[y - 1][x + 1] = 0;
						}
						else //不然就是敌人的士
						{
							flagArr[y - 1][x + 1] *= 2;
						}
					}

					if (y > 0 && x > 0)//如果左下有空间
					{
						if (_situation[y][x] & (rc << _turn))//如果发源地是本方的士
						{
							flagArr[y - 1][x - 1] = 0;
						}
						else //不然就是敌人的士
						{
							flagArr[y - 1][x - 1] *= 2;
						}
					}
					break; //至此，士的检测结束

				case shuai:
					if (y < 2)//帅能往上走
					{
						if (!_turn)//如果是轮到红棋走
							flagArr[y + 1][x] = 0;
						else flagArr[y + 1][x] *= 2;
					}
					if (y > 0)//帅能往下走
					{
						if (!_turn)//如果是轮到红棋走
							flagArr[y - 1][x] = 0;
						else flagArr[y - 1][x] *= 2;
					}
					if (x > 3)//帅能往左走
					{
						if (!_turn)//如果是轮到红棋走
							flagArr[y][x - 1] = 0;
						else flagArr[y][x - 1] *= 2;
					}
					if (x < 5)//帅能往右走
					{
						if (!_turn)//如果是轮到红棋走
							flagArr[y][x + 1] = 0;
						else flagArr[y][x + 1] *= 2;
					}
					break; //帅的检测结束

				case jiang:
					if (y < 9)//将能往上走
					{
						if (_turn)//如果是轮到黑棋走
							flagArr[y + 1][x] = 0;
						else flagArr[y + 1][x] *= 2;
					}
					if (y > 7)//将能往下走
					{
						if (_turn)//如果是轮到黑棋走
							flagArr[y - 1][x] = 0;
						else flagArr[y - 1][x] *= 2;
					}
					if (x > 3)//将能往左走
					{
						if (_turn)//如果是轮到黑棋走
							flagArr[y][x - 1] = 0;
						else flagArr[y][x - 1] *= 2;
					}
					if (x < 5)//将能往右走
					{
						if (_turn)//如果是轮到黑棋走
							flagArr[y][x + 1] = 0;
						else flagArr[y][x + 1] *= 2;
					}
					break;

				case rp:case bp://炮的检测开始
					battery = false;
					for (int tempy = y + 1;tempy < 10;tempy++)//炮的向上走
					{
						if (_situation[tempy][x])//如果向上走碰到棋子，不论敌我
						{
							if (battery) //如果已经存在了炮台
							{
								if (_situation[y][x] & (rc << _turn)) //如果发源地是本方的炮，有根标记为真
									flagArr[tempy][x] = 0;
								else flagArr[tempy][x] *= 2; //发源地是敌人的炮，危险标记为真
								break;
							}
							else battery = true; //不然就是还没有炮台，那么这个棋子当炮台
						}
					}

					battery = false;//别忘了把炮台清空
					for (int tempy = y - 1;tempy >= 0;tempy--) //炮的向下走
					{
						if (_situation[tempy][x])//如果向下走碰到棋子，不论敌我
						{
							if (battery) //如果已经存在了炮台
							{
								if (_situation[y][x] & (rc << _turn)) //如果发源地是本方的炮，有根标记为真
									flagArr[tempy][x] = 0;
								else flagArr[tempy][x] *= 2; //发源地是敌人的炮，危险标记为真
								break;
							}
							else battery = true; //不然就是还没有炮台，那么这个棋子当炮台
						}
					}

					battery = false;//别忘了把炮台清空
					for (int tempx = x - 1;tempx >= 0;tempx--) //炮的向左走
					{
						if (_situation[y][tempx])//如果向下走碰到棋子，不论敌我
						{
							if (battery) //如果已经存在了炮台
							{
								if (_situation[y][x] & (rc << _turn)) //如果发源地是本方的炮，有根标记为真
									flagArr[y][tempx] = 0;
								else flagArr[y][tempx] *= 2; //发源地是敌人的炮，危险标记为真
								break;
							}
							else battery = true; //不然就是还没有炮台，那么这个棋子当炮台
						}
					}

					battery = false;//别忘了把炮台清空
					for (int tempx = x + 1;tempx < 9;tempx++) //炮的向右走
					{
						if (_situation[y][tempx])//如果向下走碰到棋子，不论敌我
						{
							if (battery) //如果已经存在了炮台
							{
								if (_situation[y][x] & (rc << _turn)) //如果发源地是本方的炮，有根标记为真
									flagArr[y][tempx] = 0;
								else flagArr[y][tempx] *= 2; //发源地是敌人的炮，危险标记为真
								break;
							}
							else battery = true; //不然就是还没有炮台，那么这个棋子当炮台
						}
					}
					break; //至此，炮的检测结束

				case rb:
					if (y > 4)//如果小兵已经过河
					{
						if (y < 9)//如果向上还有空间
							flagArr[y + 1][x] *= (_turn * 2);
						if (x > 0)//如果向左还有空间
							flagArr[y][x - 1] *= (_turn * 2);
						if (x < 8)//如果向右还有空间
							flagArr[y][x + 1] *= (_turn * 2);

					}
					else //没过河的小兵
					{
						flagArr[y + 1][x] *= (_turn * 2);
					}
					break;//至此，兵的检测结束
				case bz:
					if (y > 5)//如果小卒已经过河
					{
						if (y > 0)//如果向下还有空间
							flagArr[y - 1][x] *= (!_turn * 2);
						if (x > 0)//如果向左还有空间
							flagArr[y][x - 1] *= (!_turn * 2);
						if (x < 8)//如果向右还有空间
							flagArr[y][x + 1] *= (!_turn * 2);
					}
					else flagArr[y - 1][x] *= (!_turn * 2);
					break;//至此，小卒的检测结束

				}
			}
		}

	/**********************位置标记已经设置完了，接下来该挨个测试了**************************/
	for (y = 0;y < 10;y++)
		for (x = 0;x < 9;x++)
		{
			if (2 <= flagArr[y][x])//如果该位置是危险标记
				if (_situation[y][x] & (rc << _turn))//并且存在行棋方的棋子
				{
					//那么就把局面回到上一步，看看在上一步的局面中，该位置的棋子是否受威胁
					if (reviewCheck(x, y))
						return true;
				}
		}
	return false;
}

bool HelloWorld::reviewCheck(int X, int Y)
{
	if (_stepCount < 2) return false;
	int x, y;
	char board[10][9];
	for (y = 0;y < 10;y++)
		for (x = 0;x < 9;x++) board[y][x] = _situation[y][x]; //这两句代码是拷贝当前局面
															  //以下两行代码是复现上一步的局面
	board[_step[_stepCount - 2].OY][_step[_stepCount - 2].OX] = _step[_stepCount - 2].ONum;
	board[_step[_stepCount - 2].AY][_step[_stepCount - 2].AX] = _step[_stepCount - 2].ANum;

	//开始检查了
	int i = board[Y][X];
	if (i)//如果传入的位置在上一步局面中不是空地
	{
		return false;
	}
	else
		return true;

	//传入的位置
}

void HelloWorld::announceWin(int t)
{
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++)
			_situation[y][x] = 0;//不管什么结果，先清空棋盘
	if (1 == t) _bw->setVisible(true); //如果黑胜，那么宣布黑胜（露图）
	else if (!t)  _rw->setVisible(true); //宣布红胜
	else _harmony->setVisible(true); //宣布和棋
}

bool HelloWorld::checkOver(bool t) //t是传递进来要被检测的某一方的王，0是红棋，1是黑棋
{
	bool eat = true;//先假设传过来的王被吃了
	if (_turn)//如果是黑棋的将被吃了
	{
		for (int y = 7;y < 10;y++)
			for (int x = 3;x < 6;x++)
				if (_situation[y][x] == jiang) eat = false;
		if (eat)
		{
			announceWin(0);//传递过去，宣布红棋胜
			return 0;
		}
	
	}
	else
	{
		for (int y = 0;y < 3;y++)
			for (int x = 3;x < 6;x++)
				if (_situation[y][x] == shuai) eat = false;
		if (eat)
		{
			announceWin(1);//传递过去，宣布黑棋胜
			return 1;
		}
	}
}
/******************************以上是第三个文件中的内容********************************/