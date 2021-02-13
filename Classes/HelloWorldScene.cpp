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

	_turn = 0;  //Ĭ�Ϻ췽��ʼ����
	_select = 0;//Ĭ��û��ѡ���κ�����
	_seting = false; //Ĭ��û���ڰھ�״̬
	_stepCount = 0;  //Ĭ��û�����壻
	
	_myL = Layer::create(); //����һ���㣬������ʾ�����ϵ�����
	/*updateL(); */
	_myL->setPosition(OFFSETX+28, OFFSETY+28); //��������ص��ڱ�������֮��
	addChild(_myL, 9);

	Sprite* desk = Sprite::create("floor.png");//�������úñ���
	desk->setPosition(visibleSize.width/2,visibleSize.height/2);
	desk->setScale(visibleSize.width / desk->getContentSize().width);
	addChild(desk);

	Sprite* board = Sprite::create("board.png"); //Ȼ���������ͼ
	board->setAnchorPoint(Vec2(0, 0));
	board->setPosition(OFFSETX, OFFSETY);
	addChild(board);

	Menu* menu = Menu::create(); //����Ǽ��ز˵���ťѡ��
	menu->setPosition(600, 200);
	addChild(menu);

	MenuItem* start = MenuItemImage::create("start.jpg", "start.jpg", 
		CC_CALLBACK_1(HelloWorld::initSituation, this));
	start->setPosition(0, 0);
	menu->addChild(start); //�ȼ���һ����ʼ��ť

	MenuItem* r = MenuItemImage::create("regret.jpg", "regret.jpg", 
		CC_CALLBACK_1(HelloWorld::regret, this));
	r->setPosition(0, 70);
	menu->addChild(r); //�ټ���һ�����尴ť

	MenuItem* advance = MenuItemImage::create("advance.jpg", "advance.jpg",
		CC_CALLBACK_1(HelloWorld::advance, this));
	advance->setPosition(0, 120);
	menu->addChild(advance); //�ټ���һ��ǰ����ť����Ӧ�����

	MenuItem* setsituation = MenuItemImage::create("setsituation.jpg", "setsituation.jpg",
		CC_CALLBACK_1(HelloWorld::setSituation, this, menu));
	setsituation->setPosition(0, 200);
	menu->addChild(setsituation); //����ټ���һ���༭��ֵİ�ť
	setsituation->setScale(114.f / 456.f); //��ΪͼƬ���Ҷ���������Ҫ���µ�������༭��ť�����
	
	_UI = cocostudio::GUIReader::getInstance()->
		widgetFromJsonFile("ChessUI_1.ExportJson"); //�Ȱѻ������ú�,�ھ�ʱҪ�õ�������״̬�²��ã�
	_UI->setVisible(false);
	addChild(_UI,99);//������ֻ���Ӧ�����ǰ����

	_invalidMove = Sprite::create("invalid.jpg");
	_invalidMove->setPosition(300, 350);
	_invalidMove->setVisible(false);
	addChild(_invalidMove,1000);
	
	_rw = Sprite::create("redwin.jpg");_bw = Sprite::create("blackwin.jpg");_harmony = Sprite::create("harmony.jpg");
	_rw->setPosition(300, 350);        _bw->setPosition(300, 350);          _harmony->setPosition(300, 350);
	_rw->setVisible(false);            _bw->setVisible(false);              _harmony->setVisible(false);
	addChild(_rw, 1000);               addChild(_bw, 1000);                 addChild(_harmony, 1000);
	_rw->setScale(5);                  _bw->setScale(5);                    _harmony->setScale(5);

	initSacrifice(); //����ٳ�ʼ��һ��ͳ�ư���

	
	//������δ����е����ݣ��е�ֻ��Ϊ�������������꣬APP�ɹ���ɾ����
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
			&& pos1.y>OFFSETY&&pos1.y < OFFSETY + 560) //�������������֮�ڡ�����
		{
			_invalidMove->setVisible(false);
			if (_seting) //�����Ϸ�ڰ������״̬
			{
				_originalX = (pos1.x - OFFSETX) / 56;
				_originalY = (pos1.y - OFFSETY) / 56;

				if (_situation[_originalY][_originalX])//�����������Ѿ����ڵ����ӣ���ô������
				{
					if (bc <= _situation[_originalY][_originalX])//����������Ǻڵ�
					{
						_turn = 1;
						//setFlag(_situation[_originalY][_originalX], true);//�ָ������ӵĿ�ѡ״̬
						_bStoneCount--;
					}
					else//��Ȼ�Ѻ췽����������һ
					{
						_turn = 0;
						//setFlag(_situation[_originalY][_originalX], true);//�ָ������ӵĿ�ѡ״̬
						_rStoneCount--;
					}
					_situation[_originalY][_originalX] = 0; //����������
				}
				else //��Ȼ������ǿյأ���ô�������������ھ�
				{
					if (_originalY > 6) _UI->setPositionY(pos1.y - 256);
					else   _UI->setPositionY(pos1.y);
					if (_originalX > 4) _UI->setPositionX(pos1.x - 256);
					else   _UI->setPositionX(pos1.x);
					_UI->setVisible(true);
				}
				updateL(); //����һ������
			}
			else   //��Ȼ��Ϸ���Ƕ�ս״̬
			{
				if (checkClick((pos1.x - OFFSETX) / 56, (pos1.y - OFFSETY) / 56) == true)//�������������з�����
				{
					_originalX = (pos1.x - OFFSETX) / 56;
					_originalY = (pos1.y - OFFSETY) / 56;
					_select = _situation[_originalY][_originalX];

					//���¼��д����Ǽ�¼����
					_step[_stepCount].OX = _originalX;
					_step[_stepCount].OY = _originalY;
					_step[_stepCount].ONum = _step[_stepCount].DONum = _select; 
					_step[_stepCount].ODark = false;
					
				}
				else if (_select) //����Ѿ�ѡȡ�����ӣ���ô���������ܾ����ƶ��������ˣ�
				{
					if (0 == _situation[(int)(pos1.y - OFFSETY) / 56][(int)(pos1.x - OFFSETX) / 56]//���������ǿյ�
						||                                                                         //����
						false == checkClick((pos1.x - OFFSETX) / 56, (pos1.y - OFFSETY) / 56))     //����Ĳ������з�����
					{
						_aimX = (pos1.x - OFFSETX) / 56;
						_aimY = (pos1.y - OFFSETY) / 56;
						if (checkMove()) //����߷�����
						{
							//���¼��д����Ǽ�¼����
							_step[_stepCount].AX = _aimX;_step[_stepCount].AY = _aimY;
							_step[_stepCount].ANum = _situation[_aimY][_aimX];
							_step[_stepCount].ADark = false;

							if (_situation[_aimY][_aimX]) _emptyStepCount = 0;//�����ˣ���Ч������

							if (_darkMark[_originalY][_originalX]) //���ԭ�ȵ�����ǰ���
							{
								_select = randBorn(); //������ɱ���
								_step[_stepCount].DONum = _select; //��¼���·��������ӣ��������ȹ���
								_situation[_aimY][_aimX] = _select; //���������ƶ���Ŀ��λ��
								_darkMark[_originalY][_originalX] = false; //��ԭ�ȵİ���λ�ñ��û�а���
								setFlag(_select,false);  //��������ʣ��İ��ӿ��ܱ���

								_step[_stepCount].ODark = true;//�����������ϵĶ�Ӧλ��Ҫ��¼���а���
							}
							else
								_situation[_aimY][_aimX] = _select;//�������ƶ���Ŀ��λ��

							if (_darkMark[_aimY][_aimX]) //���Ŀ��λ���ǰ��ӣ��Ե��ð��ӣ�
							{
								_step[_stepCount].DANum = _situation[_originalY][_originalX];//��¼��������Եİ��ӣ��������
								_step[_stepCount].ADark = true;//���׼�¼����һ�����ӣ��������
								_darkMark[_aimY][_aimX] = false; //������İ���λ�ñ��û�а���
								_situation[_originalY][_originalX] = 0; //�ٰ�ԭ��λ�����

								_step[_stepCount].ADark = true; //�����������ϵĶ�Ӧλ��Ҫ��¼���а���
							}
							else
								_situation[_originalY][_originalX] = 0; //�ٰ�ԭ��λ�����

							updateL();//����һ�¾���
							
							_turn = !_turn; //Ȼ���ٸ����������Ȩ
							_stepCount++;   //���ײ���+1
							_emptyStepCount++;//��Ч����+1
							
							if (checkKing()) //���һ��������ǲ�����������
								regret(nullptr); //������ˣ���һ����

							if (checkSituation()) //���������ͬ��
							{
								if (checkGreedy()) //������巽������
								{
									_invalidMove->setVisible(true);
									_invalidStepCount[!_turn]++;
									if (5 < _invalidStepCount[!_turn]) announceWin(_turn);
								}
							}
							else //�������û����ͬ
							{
								_invalidStepCount[!_turn] = 0;//������巽��Υ���¼
								checkOver(_turn); //�������巽�ǲ��ǰ��˼ҵ���ɱ��
							}
							if (_emptyStepCount >= 60) announceWin(3);//��Ч��������60��ǿ�ƺ���
						}
					}
					_select = 0; //����ƶ�ʧ�ܣ�����ѡȡ����״̬��ûѡ���κ����ӣ�
				}
			}
		}
		
		
		return true;
	};//lamdam���������������

	_eventDispatcher->addEventListenerWithSceneGraphPriority(l, this);
    return true;
}


void HelloWorld::initSituation(Ref* ref) //��ʼ�����̾��棬��Ӧ�¾ְ�ť��
{
	_turn = false; //Ĭ�Ϻ�������
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++) _situation[y][x] = 0; //��վ���
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++) _darkMark[y][x] = false;//������а��ӱ��


	_situation[0][0] = rc, _situation[0][1] = rm, _situation[0][2] = rx, _situation[0][3] = rs; 
	_situation[0][5] = rs, _situation[0][6] = rx, _situation[0][7] = rm, _situation[0][8] = rc;
	_situation[2][1] = _situation[2][7] = rp;//��ʼ�������˫��λ��
	_situation[3][0] = _situation[3][2] = _situation[3][4] = _situation[3][6] = _situation[3][8] = rb;
	_situation[0][4] = shuai, _shuaiExist = true; //����Ǻ�˧�ĳ�ʼ��

	_situation[9][0] = bc, _situation[9][1] = bm, _situation[9][2] = bx, _situation[9][3] = bs; 
	_situation[9][5] = bs, _situation[9][6] = bx, _situation[9][7] = bm, _situation[9][8] = bc;
	_situation[7][1] = _situation[7][7] = bp;//��ʼ�������˫��λ��
	_situation[6][0] = _situation[6][2] = _situation[6][4] = _situation[6][6] = _situation[6][8] = bz;
	_situation[9][4] = jiang, _jiangExist = true;

	////////////�����ǳ�ʼ�����ӵ�λ��//////////////
	_darkMark[0][0] = true, _darkMark[0][1] = true, _darkMark[0][2] = true, _darkMark[0][3] = true;
	_darkMark[0][5] = true, _darkMark[0][6] = true, _darkMark[0][7] = true, _darkMark[0][8] = true;
	_darkMark[2][1] = _darkMark[2][7] = true;//��ʼ�������˫��λ��
	_darkMark[3][0] = _darkMark[3][2] = _darkMark[3][4] = _darkMark[3][6] = _darkMark[3][8] = true;

	_darkMark[9][0] = true, _darkMark[9][1] = true, _darkMark[9][2] = true, _darkMark[9][3] = true;
	_darkMark[9][5] = true, _darkMark[9][6] = true, _darkMark[9][7] = true, _darkMark[9][8] = true;
	_darkMark[7][1] = _darkMark[7][7] = true;//��ʼ�������˫��λ��
	_darkMark[6][0] = _darkMark[6][2] = _darkMark[6][4] = _darkMark[6][6] = _darkMark[6][8] = true;

	for (int i = 0;i < 15;i++)
		_darkStone[0][i] = _darkStone[1][i] = true; //���ú����еİ��ӱ��λ��

	_siNumP[0] = new Situation(255); //֮������255�����������Ϊ��������һ�в����ܳ���������
	_siNumP[1] = new Situation(255);

	for (int c = 0;c < 300;c++)_step[c].ONum = 255; //������׼�¼
	_stepCount = 0;//����岽����
	_rw->setVisible(false);            _bw->setVisible(false);  _harmony->setVisible(false);//����˫��������ʤ��ͼ
	_invalidStepCount[0] = _invalidStepCount[1] = 0;//���˫����Υ���¼
	_emptyStepCount = 0;
	updateL();
}

void HelloWorld::updateL() //���¾���֮��
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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
				if (_darkMark[y][x]) //������ӱ�ǻ�����
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

bool HelloWorld::ruleOfChe() //�����߷�
{
	if (_originalX == _aimX) //����ں�����ͬ
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
	else if (_originalY == _aimY) //����ں�����ͬ
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

bool HelloWorld::ruleOfMa() //����߷�
{
	if (!_situation[_originalY + 1][_originalX])//�����һ�������ǿ�
	{
		if (_aimY - _originalY == 2)
			return 1 == (_aimX - _originalX)*(_aimX - _originalX);
	}
	if (!_situation[_originalY - 1][_originalX])//�����һ�������ǿ�
	{
		if (2 == _originalY - _aimY)
			return 1 == (_aimX - _originalX)*(_aimX - _originalX);
	}
	if (!_situation[_originalY][_originalX + 1])//�����һ�������ǿ�
	{
		if (2 == _aimX - _originalX)
			return 1 == (_aimY - _originalY)*(_aimY - _originalY);
	}
	if (!_situation[_originalY][_originalX - 1])//�����һ�������ǿ�
	{
		if (-2 == _aimX - _originalX)
			return 1 == (_aimY - _originalY)*(_aimY - _originalY);
	}
	return false;
}

bool HelloWorld::ruleOfXiang()//����߷�
{
	if (!_situation[_originalY + 1][_originalX + 1])//������ϵ�����Ϊ��
	{
		if ((2 == _aimY - _originalY) && (2 == _aimX - _originalX))
			return true;
	}
	if (!_situation[_originalY + 1][_originalX - 1])//������ϵ�����Ϊ��
	{
		if ((2 == _aimY - _originalY) && (2 == _originalX - _aimX))
			return true;
	}
	if (!_situation[_originalY - 1][_originalX + 1])//������µ�����Ϊ��
	{
		if ((2 == _originalY - _aimY) && (2 == _aimX - _originalX))
			return true;
	}
	if (!_situation[_originalY - 1][_originalX - 1])//������µ�����Ϊ��
	{
		if ((2 == _originalY - _aimY) && (2 == _originalX - _aimX))
			return true;
	}
	return false;
}

bool HelloWorld::ruleOfShi()//ʿ���߷�
{
	return 1 == (_aimY - _originalY)*(_aimY - _originalY)*(_aimX - _originalX)*(_aimX - _originalX);
}

bool HelloWorld::ruleOfKing()
{
	if (_turn)//���Ǻڽ����߷�
	{
		if (_aimX > 2 && _aimX < 6 && _aimY>6)//������ھŹ�֮��
		{
			if (_aimX == _originalX &&
				1 == (_aimY - _originalY)*(_aimY - _originalY))
				return true;
			if (_aimY == _originalY &&
				1 == (_aimX - _originalX)*(_aimX - _originalX))
				return true;
		}
	}
	else //���Ǻ�˧���߷�
	{
		if (_aimX > 2 && _aimX < 6 && _aimY<3)//������ھŹ�֮��
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

bool HelloWorld::ruleOfPao() //�ڵ��߷�
{
	if (_situation[_aimY][_aimX]) //���Ŀ��λ�������ӣ�ֻ���ǵ������ӣ�����ô������
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
	else //���Ŀ��λ���ǿյأ���ô�ڵļ��������¡�����
	{
		if (_originalX == _aimX) //����ں�����ͬ
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
		else if (_originalY == _aimY) //����ں�����ͬ
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
	if (_turn)//�������ӵ��߷�
	{
		if (5 > _originalY) //���С�������
		{
			if (_aimX == _originalX && -1 == _aimY - _originalY)
				return true;
			if (_aimY == _originalY &&
				1 == (_aimX - _originalX)*(_aimX - _originalX))
				return true;
		}
		else //��Ȼ
		{
			if (_aimX == _originalX && -1 == _aimY - _originalY)
				return true;
		}
	}
	else //����С�����߷�
	{
		if (5 <= _originalY) //���С��������
		{
			if (_aimX == _originalX && 1 == _aimY - _originalY)
				return true;
			if (_aimY == _originalY &&
				1 == (_aimX - _originalX)*(_aimX - _originalX))
				return true;
		}
		else //��Ȼ
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
			return (rc << _turn) + 6; //���ر�����
		break;
	case 5:case 6:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 0; //���س�
		break;
	case 7: case 8:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 1; //������
		break;
	case 9:case 10:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 2; //������
		break;
	case 11:case 12:
		if (_darkStone[_turn][i]) //����ʿ
			return (rc << _turn) + 3;
		break;
	case 13:case 14:
		if (_darkStone[_turn][i])
			return (rc << _turn) + 5; //������
		break;
	}
	goto start;
}

void HelloWorld::setFlag(int i, bool b)
{
	switch (i)
	{
	case rc:case bc: //����˫���ĳ���û�������Ŀ���
		if (!b == _darkStone[_turn][5])
			_darkStone[_turn][5] = b;
		else _darkStone[_turn][6] = b; //���ﲻ�ü�if�ж�Ҳ��
		break;

	case rm:case bm: //����˫������
		if (!b == _darkStone[_turn][7])
			_darkStone[_turn][7] = b;
		else if (!b == _darkStone[_turn][8])
			_darkStone[_turn][8] = b;
		break;

	case rx:case bx: //������
		if (!b == _darkStone[_turn][9])
			_darkStone[_turn][9] = b;
		else _darkStone[_turn][10] = b;
		break;

	case rs:case bs: //����ʿ
		if (!b == _darkStone[_turn][11])
			_darkStone[_turn][11] = b;
		else _darkStone[_turn][12] = b;
		break;

	case rp:case bp: //������
		if (!b == _darkStone[_turn][13])
			_darkStone[_turn][13] = b;
		else _darkStone[_turn][14] = b;
		break;

	case rb:case bz: //���ñ���
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
	_seting = true; //����Ϸ����Ϊ�ھ�״̬
	setStone(); //���ڵ������������Ϊ�˰���Щlambdn����ע����
	delSitua(_siNumP[0]); delSitua(_siNumP[1]); //�ݹ��ͷ���ǰ���彨���ľ�����
	_rStoneCount = _bStoneCount = 0; //�����Ӽ����������㣬ÿ�ڷ�һ�����ӣ���Ӧһ���ı���+1
	_shuaiExist = _jiangExist = false;
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++) _situation[y][x] = 0;
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++) _darkMark[y][x] = false; //��������밵�ӱ��


														   ////////////�������³�ʼ�����ӵ�λ��//////////////
	_darkMark[0][0] = true, _darkMark[0][1] = true, _darkMark[0][2] = true, _darkMark[0][3] = true;
	_darkMark[0][5] = true, _darkMark[0][6] = true, _darkMark[0][7] = true, _darkMark[0][8] = true;
	_darkMark[2][1] = _darkMark[2][7] = true;//��ʼ�������˫��λ��
	_darkMark[3][0] = _darkMark[3][2] = _darkMark[3][4] = _darkMark[3][6] = _darkMark[3][8] = true;

	_darkMark[9][0] = true, _darkMark[9][1] = true, _darkMark[9][2] = true, _darkMark[9][3] = true;
	_darkMark[9][5] = true, _darkMark[9][6] = true, _darkMark[9][7] = true, _darkMark[9][8] = true;
	_darkMark[7][1] = _darkMark[7][7] = true;//��ʼ�������˫��λ��
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
	Button* rcbtn = (Button*)Helper::seekWidgetByName(_UI, "rc"); //�������Ǻ쳵�İ���
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
	if (_shuaiExist && _jiangExist) //ֻ��˧�ͽ��������Ӷ����ڣ����ܰھֳɹ�
	{
		m1->setVisible(true); //������ʾ�¾ֵȲ˵���
		_UI->setVisible(false); //�ھֻ���������
		removeChild(m2);      //�ھ�OK�Ĳ˵�������
		_seting = false;      //�ھֽ���
		_turn = 0;//����Ϊ����

		for (int y = 0;y < 10;y++) //��û�����ӵĵط�������λ�ñ�Ƕ��������
			for (int x = 0;x < 9;x++)
				if (!_situation[y][x])
					_darkMark[y][x] = false;

		_siNumP[0] = new Situation(255); //֮������255�����������Ϊ��������һ�в����ܳ���������
		_siNumP[1] = new Situation(255); //�����˸�˫�����������������������ж��Ƿ��ظ�����
		for (int c = 0;c < 300;c++)_step[c].ONum = 255; //������׼�¼
		_stepCount = 0;//����岽����
		_rw->setVisible(false);            _bw->setVisible(false);  _harmony->setVisible(false);//����˫��������ʤ��ͼ
		_invalidStepCount[0] = _invalidStepCount[1] = 0;//���˫����Υ���¼
		_emptyStepCount = 0; //�����Ч������¼
	}
}

void HelloWorld::regret(Ref* ref)//����ĺ���
{
	if (_step[_stepCount - 1].ONum != 255 && _stepCount > 0)//��������Ի���
	{
		_turn = !_turn;//���˻�����Ȩ
		_situation[_step[_stepCount - 1].OY][_step[_stepCount - 1].OX] = _step[_stepCount - 1].ONum;//�ж����ӻ���
		_situation[_step[_stepCount - 1].AY][_step[_stepCount - 1].AX] = _step[_stepCount - 1].ANum;//��������ӣ�����еĻ���
		if(_step[_stepCount-1].ODark)//��������ǰ���
		{
			setFlag(_step[_stepCount - 1].DONum, true);//��ô�Ȱ��Ǹ����Ӹǻ�ȥ���ָ������ֵĳ���Ȩ����
			_darkMark[_step[_stepCount - 1].OY][_step[_stepCount - 1].OX] = true;//Ȼ���λ������Ϊ����
		}
		if (_step[_stepCount - 1].ADark)//������Ե��ǰ���
		{
			_situation[_step[_stepCount - 1].OY][_step[_stepCount - 1].OX] = _step[_stepCount - 1].DANum;//�ָ������������
			_darkMark[_step[_stepCount - 1].AY][_step[_stepCount - 1].AX] = true;//��λ����ΪΪ����
		}
		_stepCount--;//�岽��¼-1
		_emptyStepCount--; //��Ч������¼-1
		updateL();
	}
}

void HelloWorld::advance(Ref* ref)
{
	if (_step[_stepCount].ONum != 255 && _stepCount < 300)//���������ǰ��
	{
		_situation[_step[_stepCount].OY][_step[_stepCount].OX] = 0; //�ж���λ�����
		_situation[_step[_stepCount].AY][_step[_stepCount].AX] = _step[_stepCount].DONum;//Ŀ��λ�ðڷ��ж����ӣ������Ƿ���������
		_darkMark[_step[_stepCount].OY][_step[_stepCount].OX] = _darkMark[_step[_stepCount].AY][_step[_stepCount].AX] = false;
		_turn = !_turn;
		_stepCount++;
		updateL();
	}
}
/***************������Щ��additionFile1�е�����******************/












































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

void HelloWorld::delSitua(Situation * tp) //�ݹ��ͷŶ��еľ�����
{
	if (tp->p_branch != NULL) //����ͬ���֧����һ���ڵ��ǲ��ǿ�
		delSitua(tp->p_branch);

	if (tp->p_next != NULL) //������һ���ǲ��ǿ�
		delSitua(tp->p_next);

	delete tp;
	return;
}

bool HelloWorld::checkSituation()
{
	Situation * tempp = _siNumP[_turn];
	int i = 0;
start:
	if (10 == i) //���ȫ��������ͬ������
		return true; //��ô�����棬�����߳�ͬ����
	else if (getNum(i) == tempp->sinum) //�����ǰ����ֵ��ͬ
	{
		tempp = tempp->p_next;
		i++;
		goto start;
	}
	else if (NULL != tempp->p_branch) //�����ǰ����ѧֵ��ͬ����������һ��ͬ���֧
	{
		tempp = tempp->p_branch;
		goto start; //��ѭ������һ�¸÷�֧�ǲ�����ͬ
	}
	else //���ͬ���֧����ͷ����ô��ʱ��ƴ�����ǵľ��������ˣ���Ϊ��ǰ��û���ֹ����¾���
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

unsigned long long HelloWorld::getNum(int y) //����������ȡ����ĳһ����ֵ��
{
	unsigned long long num = 0;
	for (int x = 0; x < 9; x++)
	{

		if (_darkMark[y][x])//�����ǰ�������ǰ���
		{
			if (_situation[y][x] & rc)//����Ǻ췽����
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
/************************************�����ǵڶ����ļ��е�����***************************************/











































































































/******************************�������ļ��е�����********************************/

bool HelloWorld::checkGreedy()
{
	int flagArr[10][9]; //Ūһ���������飬���������Щλ�����и��ģ��������ӻ������ǣ�

	int x, y;

	bool battery = false;//�������̨��������Ϊ��

	for (y = 0;y < 10;y++)
		for (x = 0;x < 9;x++) flagArr[y][x] = 1;

	for (y = 0;y < 10;y++)
		for (x = 0;x < 9;x++)
		{
			if (_situation[y][x]) //�����λ�ô�������
			{
				switch (_situation[y][x])
				{
				case rc:case bc:
					for (int tempy = y + 1;tempy < 10;tempy++)//����������
					{
						if (_situation[tempy][x])//����������������ӣ����۵���
						{
							if (_situation[y][x] & (rc << _turn)) //�����Դ���Ǳ����ĳ����и����Ϊ��
								flagArr[tempy][x] = 0;
							else flagArr[tempy][x] *= 2; //��Դ���ǵ��˵ĳ���Σ�ձ��Ϊ��
							break;
						}
					}
					for (int tempy = y - 1;tempy >= 0;tempy--)//����������
					{
						if (_situation[tempy][x])//����������������ӣ����۵���
							if (_situation[tempy][x])//����������������ӣ����۵��ң���·��ǽ���
							{
								if (_situation[y][x] & (rc << _turn)) //����Ǳ����ĳ�
									flagArr[tempy][x] = 0; //�и����Ϊ��
								else flagArr[tempy][x] *= 2; //�ǵз��ĳ���Σ�ձ��Ϊ��
								break;
							}
					}
					for (int tempx = x - 1;tempx >= 0;tempx--)//����������
					{
						if (_situation[y][tempx]) //����������������ӣ����۵��ң���·��ǽ���
						{
							if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ����ĳ�
								flagArr[y][tempx] = 0; //�и����Ϊ��
							else flagArr[y][tempx] *= 2;//��Ȼ��Դ���ǵ��˵ĳ���Σ�ձ��Ϊ��
							break;
						}
					}
					for (int tempx = x + 1;tempx < 9;tempx++)//����������
					{
						if (_situation[y][tempx]) //����������������ӣ����۵��ң���·��ǽ���
						{
							if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ����ĳ�
								flagArr[y][tempx] = 0; //�и����Ϊ��
							else flagArr[y][tempx] *= 2;//��Ȼ��Դ���ǵ��˵ĳ���Σ�ձ��Ϊ��
							break;
						}
					}
					break; //���ˣ����ļ�����

				case rm:case bm: //��ļ�⿪ʼ
					if (y < 8 && !_situation[y + 1][x])//��������пռ䲢��������û�б���
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ�������
						{
							//�������У������и����
							flagArr[y + 2][x - 1] = 0;
							flagArr[y + 2][x + 1] = 0;
						}
						else //��Ȼ��Դ���ǵ��˵���
						{
							//�������У�������Σ�ձ��
							flagArr[y + 2][x - 1] *= 2;
							flagArr[y + 2][x + 1] *= 2;
						}
					}

					if (y > 1 && !_situation[y - 1][x])//��������пռ䲢��������û�б���
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ�������
						{
							//�������У������и����
							flagArr[y - 2][x - 1] = 0;
							flagArr[y - 2][x + 1] = 0;
						}
						else //��Ȼ��Դ���ǵ��˵���
						{
							//�������У�����Σ�ձ��
							flagArr[y - 2][x - 1] *= 2;
							flagArr[y - 2][x + 1] *= 2;
						}
					}

					if (x > 1 && !_situation[y][x - 1])//�������пռ䲢��������û�б���
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ�������
						{
							//�������У������и����
							flagArr[y - 1][x - 2] = 0;
							flagArr[y + 1][x - 2] = 0;
						}
						else //��Ȼ��Դ���ǵ��˵���
						{
							//�������У�����Σ�ձ��
							flagArr[y - 1][x - 2] *= 2;
							flagArr[y + 1][x - 2] *= 2;
						}
					}

					if (x < 7 && !_situation[y][x + 1])//����ұ��пռ䲢��������û�б���
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ�������
						{
							//�������У������и����
							flagArr[y - 1][x + 2] = 0;
							flagArr[y + 1][x + 2] = 0;
						}
						else //��Ȼ��Դ���ǵ��˵���
						{
							//�������У�����Σ�ձ��
							flagArr[y - 1][x + 2] *= 2;
							flagArr[y + 1][x + 2] *= 2;
						}
					}
					break; //���ˣ���ļ�����

				case rx:case bx:
					if (y < 8 && x > 1 && !_situation[y + 1][x - 1])//��������пռ䲢����������û�б���
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ����������
						{
							flagArr[y + 2][x - 2] = 0;
						}
						else //��Ȼ��Դ�ؾ��ǵ��˵������
						{
							flagArr[y + 2][x - 2] *= 2;
						}
					}

					if (y < 8 && x < 7 && !_situation[y + 1][x + 1])//��������пռ䲢����������û�б���
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ����������
						{
							flagArr[y + 2][x + 2] = 0;
						}
						else //��Ȼ��Դ�ؾ��ǵ��˵������
						{
							flagArr[y + 2][x + 2] *= 2;
						}
					}

					if (y > 1 && x < 7 && !_situation[y - 1][x + 1])//��������пռ䲢����������û�б���
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ����������
						{
							flagArr[y - 2][x + 2] = 0;
						}
						else //��Ȼ��Դ�ؾ��ǵ��˵������
						{
							flagArr[y - 2][x + 2] *= 2;
						}
					}

					if (y > 1 && x > 1 && !_situation[y - 1][x + 1])//��������пռ䲢����������û�б���
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ����������
						{
							flagArr[y - 2][x - 2] = 0;
						}
						else //��Ȼ��Դ�ؾ��ǵ��˵������
						{
							flagArr[y - 2][x - 2] *= 2;
						}
					}
					break; //���ˣ������ļ�����

				case rs:case bs:
					if (y < 9 && x > 0)//��������пռ�
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ�����ʿ
						{
							flagArr[y + 1][x - 1] = 0;
						}
						else //��Ȼ���ǵ��˵�ʿ
						{
							flagArr[y + 1][x - 1] *= 2;
						}
					}

					if (y < 9 && x < 8)//��������пռ�
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ�����ʿ
						{
							flagArr[y + 1][x + 1] = 0;
						}
						else //��Ȼ���ǵ��˵�ʿ
						{
							flagArr[y + 1][x + 1] *= 2;
						}
					}

					if (y > 0 && x < 9)//��������пռ�
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ�����ʿ
						{
							flagArr[y - 1][x + 1] = 0;
						}
						else //��Ȼ���ǵ��˵�ʿ
						{
							flagArr[y - 1][x + 1] *= 2;
						}
					}

					if (y > 0 && x > 0)//��������пռ�
					{
						if (_situation[y][x] & (rc << _turn))//�����Դ���Ǳ�����ʿ
						{
							flagArr[y - 1][x - 1] = 0;
						}
						else //��Ȼ���ǵ��˵�ʿ
						{
							flagArr[y - 1][x - 1] *= 2;
						}
					}
					break; //���ˣ�ʿ�ļ�����

				case shuai:
					if (y < 2)//˧��������
					{
						if (!_turn)//������ֵ�������
							flagArr[y + 1][x] = 0;
						else flagArr[y + 1][x] *= 2;
					}
					if (y > 0)//˧��������
					{
						if (!_turn)//������ֵ�������
							flagArr[y - 1][x] = 0;
						else flagArr[y - 1][x] *= 2;
					}
					if (x > 3)//˧��������
					{
						if (!_turn)//������ֵ�������
							flagArr[y][x - 1] = 0;
						else flagArr[y][x - 1] *= 2;
					}
					if (x < 5)//˧��������
					{
						if (!_turn)//������ֵ�������
							flagArr[y][x + 1] = 0;
						else flagArr[y][x + 1] *= 2;
					}
					break; //˧�ļ�����

				case jiang:
					if (y < 9)//����������
					{
						if (_turn)//������ֵ�������
							flagArr[y + 1][x] = 0;
						else flagArr[y + 1][x] *= 2;
					}
					if (y > 7)//����������
					{
						if (_turn)//������ֵ�������
							flagArr[y - 1][x] = 0;
						else flagArr[y - 1][x] *= 2;
					}
					if (x > 3)//����������
					{
						if (_turn)//������ֵ�������
							flagArr[y][x - 1] = 0;
						else flagArr[y][x - 1] *= 2;
					}
					if (x < 5)//����������
					{
						if (_turn)//������ֵ�������
							flagArr[y][x + 1] = 0;
						else flagArr[y][x + 1] *= 2;
					}
					break;

				case rp:case bp://�ڵļ�⿪ʼ
					battery = false;
					for (int tempy = y + 1;tempy < 10;tempy++)//�ڵ�������
					{
						if (_situation[tempy][x])//����������������ӣ����۵���
						{
							if (battery) //����Ѿ���������̨
							{
								if (_situation[y][x] & (rc << _turn)) //�����Դ���Ǳ������ڣ��и����Ϊ��
									flagArr[tempy][x] = 0;
								else flagArr[tempy][x] *= 2; //��Դ���ǵ��˵��ڣ�Σ�ձ��Ϊ��
								break;
							}
							else battery = true; //��Ȼ���ǻ�û����̨����ô������ӵ���̨
						}
					}

					battery = false;//�����˰���̨���
					for (int tempy = y - 1;tempy >= 0;tempy--) //�ڵ�������
					{
						if (_situation[tempy][x])//����������������ӣ����۵���
						{
							if (battery) //����Ѿ���������̨
							{
								if (_situation[y][x] & (rc << _turn)) //�����Դ���Ǳ������ڣ��и����Ϊ��
									flagArr[tempy][x] = 0;
								else flagArr[tempy][x] *= 2; //��Դ���ǵ��˵��ڣ�Σ�ձ��Ϊ��
								break;
							}
							else battery = true; //��Ȼ���ǻ�û����̨����ô������ӵ���̨
						}
					}

					battery = false;//�����˰���̨���
					for (int tempx = x - 1;tempx >= 0;tempx--) //�ڵ�������
					{
						if (_situation[y][tempx])//����������������ӣ����۵���
						{
							if (battery) //����Ѿ���������̨
							{
								if (_situation[y][x] & (rc << _turn)) //�����Դ���Ǳ������ڣ��и����Ϊ��
									flagArr[y][tempx] = 0;
								else flagArr[y][tempx] *= 2; //��Դ���ǵ��˵��ڣ�Σ�ձ��Ϊ��
								break;
							}
							else battery = true; //��Ȼ���ǻ�û����̨����ô������ӵ���̨
						}
					}

					battery = false;//�����˰���̨���
					for (int tempx = x + 1;tempx < 9;tempx++) //�ڵ�������
					{
						if (_situation[y][tempx])//����������������ӣ����۵���
						{
							if (battery) //����Ѿ���������̨
							{
								if (_situation[y][x] & (rc << _turn)) //�����Դ���Ǳ������ڣ��и����Ϊ��
									flagArr[y][tempx] = 0;
								else flagArr[y][tempx] *= 2; //��Դ���ǵ��˵��ڣ�Σ�ձ��Ϊ��
								break;
							}
							else battery = true; //��Ȼ���ǻ�û����̨����ô������ӵ���̨
						}
					}
					break; //���ˣ��ڵļ�����

				case rb:
					if (y > 4)//���С���Ѿ�����
					{
						if (y < 9)//������ϻ��пռ�
							flagArr[y + 1][x] *= (_turn * 2);
						if (x > 0)//��������пռ�
							flagArr[y][x - 1] *= (_turn * 2);
						if (x < 8)//������һ��пռ�
							flagArr[y][x + 1] *= (_turn * 2);

					}
					else //û���ӵ�С��
					{
						flagArr[y + 1][x] *= (_turn * 2);
					}
					break;//���ˣ����ļ�����
				case bz:
					if (y > 5)//���С���Ѿ�����
					{
						if (y > 0)//������»��пռ�
							flagArr[y - 1][x] *= (!_turn * 2);
						if (x > 0)//��������пռ�
							flagArr[y][x - 1] *= (!_turn * 2);
						if (x < 8)//������һ��пռ�
							flagArr[y][x + 1] *= (!_turn * 2);
					}
					else flagArr[y - 1][x] *= (!_turn * 2);
					break;//���ˣ�С��ļ�����

				}
			}
		}

	/**********************λ�ñ���Ѿ��������ˣ��������ð���������**************************/
	for (y = 0;y < 10;y++)
		for (x = 0;x < 9;x++)
		{
			if (2 <= flagArr[y][x])//�����λ����Σ�ձ��
				if (_situation[y][x] & (rc << _turn))//���Ҵ������巽������
				{
					//��ô�ͰѾ���ص���һ������������һ���ľ����У���λ�õ������Ƿ�����в
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
		for (x = 0;x < 9;x++) board[y][x] = _situation[y][x]; //����������ǿ�����ǰ����
															  //�������д����Ǹ�����һ���ľ���
	board[_step[_stepCount - 2].OY][_step[_stepCount - 2].OX] = _step[_stepCount - 2].ONum;
	board[_step[_stepCount - 2].AY][_step[_stepCount - 2].AX] = _step[_stepCount - 2].ANum;

	//��ʼ�����
	int i = board[Y][X];
	if (i)//��������λ������һ�������в��ǿյ�
	{
		return false;
	}
	else
		return true;

	//�����λ��
}

void HelloWorld::announceWin(int t)
{
	for (int y = 0;y < 10;y++)
		for (int x = 0;x < 9;x++)
			_situation[y][x] = 0;//����ʲô��������������
	if (1 == t) _bw->setVisible(true); //�����ʤ����ô������ʤ��¶ͼ��
	else if (!t)  _rw->setVisible(true); //������ʤ
	else _harmony->setVisible(true); //��������
}

bool HelloWorld::checkOver(bool t) //t�Ǵ��ݽ���Ҫ������ĳһ��������0�Ǻ��壬1�Ǻ���
{
	bool eat = true;//�ȼ��贫��������������
	if (_turn)//����Ǻ���Ľ�������
	{
		for (int y = 7;y < 10;y++)
			for (int x = 3;x < 6;x++)
				if (_situation[y][x] == jiang) eat = false;
		if (eat)
		{
			announceWin(0);//���ݹ�ȥ����������ʤ
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
			announceWin(1);//���ݹ�ȥ����������ʤ
			return 1;
		}
	}
}
/******************************�����ǵ������ļ��е�����********************************/