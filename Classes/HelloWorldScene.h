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
#define OFFSETX 20 //�������̵�X����ƫ����
#define OFFSETY 70//�������̵�Y����ƫ����

class Situation //����һ�������࣬����������ָ�룬���������ж������Ƿ���ͬ��
{
public:
	unsigned long long sinum; //����ǵ�ǰ�о������ѧ��ʾֵ
	Situation * p_branch; //ͬ�����һ����֧
	Situation * p_next; //��һ�����ֵ
	Situation(unsigned long long num = 0)
	{
		sinum = num;
		p_branch = NULL;
		p_next = NULL;
	}
};
class Step //����һ�������࣬������ʵ�ַ��ء�ǰ���ȹ���
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


	//���¿�ʼ�Զ�����
	void regret(Ref* ref);
	void advance(Ref* ref);//�����ǰ����ť
	void setSituation(Ref* ref,Menu* m); //����ǰھ�ʱ�ĳ�ʼ�����������廯������������
	void setOK(Ref*, Menu*, Menu*);

	void initSituation(Ref*);//��ʼ�����̾���
	void updateL();      //�����������̵ľ���֮��
	bool checkClick(int x, int y)//��������Ƿ������з������ӣ��Ƿ��棬�񷵼�
	{
		int a = (_situation[y][x]);
		int b = (rc << _turn);
		if (( a&b ) == b)
			return true;
		else return false;
	}

	char _situation[10][9];//��������������
	enum {rc=16,rm,rx,rs,shuai,rp,rb,anhong,bc=32,bm,bx,bs,jiang,bp,bz,anhei}; //ö�����ӵ�����
	int _originalX, _originalY, _aimX, _aimY;//�ֱ���ԭʼ�����Ŀ�ĵ����꣬������������
	short _select;//����ѡȡ����������
	bool _turn;   //true���ֵ��ڷ����壬false���ֵ��췽����
	bool _darkMark[10][9];//����ǰ����������ϵı��
	bool _darkStone[2][15];//�������Ǻ��˫���İ��ӱ��֣�����������¼�������İ��ӣ�true�������ڣ�
	bool _shuaiExist, _jiangExist;
	bool _seting;  //�����������Ƿ��ڰ��������
	int _rStoneCount, _bStoneCount;//��������������ھ�ʱ�Ѿ��ڷŵ����ӣ����ܴ���16��

	Layer* _myL;      //����ǲ㾫�飬������ʾ���̾����
	Widget* _UI;  //����ǻ������������ʱҪ�õ��������Ǹ������ӡ���ť����
	Sprite* _sacrificeStone[2][15];//���˫�����������ӡ�
	Sprite* _invalidMove;
	Sprite* _rw, *_bw, *_harmony;//����3�����飬һ���Ǻ�ʤСͼ��һ���Ǻ�ʤСͼ,һ������Сͼ


private:
	bool checkMove();
	bool ruleOfChe();
	bool ruleOfMa();
	bool ruleOfXiang();
	bool ruleOfShi();
	bool ruleOfKing();
	bool ruleOfPao();
	bool ruleOfBing();

	int randBorn();   //��������ƶ�����ʱ�����������ʲô����
	void setFlag(int,bool);//������ÿ��ܳ��ֵı��֣����ӱ�����ھ�ʱ�õ�
	void setStone();  //������ǰھ�ʱ�ľ��廯����
	void initSacrifice(); //��ʼ�����������ǵľ���
	bool checkKing(); //���һ���������������˷����棬��Ȼ���ؼ�


	//���¿�ʼ�������ļ������ݵĿ����ˣ���Ҫ����ͬ���ж�����׽�����ж���
private:
	int _invalidStepCount[2], //����ͳ�Ʒ��������0�Ǻ���ģ�1�Ǻ����
		_emptyStepCount,     //����ͳ�ƿղ����������ӵ����壩
		_stepCount;     //������¼�岽
	Step _step[300];//��������׼�¼�����ֻ�ܼ�¼300����

	Situation * _siNumP[2]; //������������0�±��Ǻ���ģ�1�±��Ǻ����

	bool checkGreedy(); //���������������巽�Ƿ��ڳ������߳�׽
	bool checkOver(bool);//���˧���߽��ǲ��Ǳ��Ե���
	void announceWin(int); //����ĳһ��ʤ��
	bool checkSituation(); //�����������������������ǲ�����ͬ��
	bool reviewCheck(int x, int y);//��������������鱻��в����������һ���ǲ��ǰ�ȫ��
	void spliceNode(int i, Situation * tp);//�����������ƴ�Ӿ�����
	void delSitua(Situation * tp); //������������ݹ��ͷž�����
	unsigned long long getNum(int y); //�������������ȡĳһ�еľ�����ѧֵ
};

#endif // __HELLOWORLD_SCENE_H__
