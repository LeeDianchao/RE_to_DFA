#pragma once
#include"NFA.h"
#include<iostream>
#include<cstring>
#include<string>
#include<stack>
#include<vector>
#include<set>
#include<map>
#include<queue>
using namespace std;

struct DFA			//����DFA
{
	int startState;			//��̬
	set<int> endStates;		//��̬��
	set<char> End_symbol;	//�ս����
	map<int, set<int>> Dfa_set;   //�洢�ϲ����dfa״̬��(����ϲ�nfa��״̬)
	DFA()
	{
		startState = 0;
	}
};

struct DFAtra_Edge	//DFA��ת��
{
	char input;			//ת������
	int transTo;		//ת������״̬��
	DFAtra_Edge()
	{
		input = ' ';
		transTo = -1;
	}
};
struct DfaState		//DFA״̬
{
	bool valid;         //״̬�Ƿ���Ч
	bool isEnd;			//�Ƿ�Ϊ��̬
	int index;			//״̬��
	int edgeNum;		//DFA״̬�ϵ��������
	DFAtra_Edge Edges[62]; //DFA״̬�ϵ������ (0-9,A-Z,a-z)
	set<int> closure;      //NFA�Ħ�-move()�հ�
	DfaState()
	{
		valid = false;
		isEnd = false;
	}

};



class DFA_class
{
private:
	DfaState DfaStates[MAX];	//DFA״̬����
	int DFAStateNum;		//DFA״̬����
	vector<NfaState> NfaStates;	//NFA״̬����
	int NFAStateNum;			//NFA״̬����
	NFA nfa;
	DFA dfa;
	vector < vector<int>> transMatrix;//״̬ת�Ʊ�
public:
	DFA_class(NFA_class nc)
	{
		DFAStateNum = 0;
		NfaStates = nc.getNfaStates();
		NFAStateNum = nc.getNFAStateNum();
		nfa = nc.getResultNFA();
		transMatrix = vector < vector<int>>(NFAStateNum, vector<int>(62, -1));//(0-9,A-Z,a-z)
		for (int i = 0; i < MAX; i++)
		{
			DfaStates[i].index = i;
		}
	}
	~DFA_class() {}
	int getDFAStateNum()
	{
		return DFAStateNum;
	}
	DfaState *getDfaStates()
	{
		return DfaStates;
	}
	DFA getDFA()
	{
		return dfa;
	}
	//�ж�һ��״̬�Ƿ�Ϊ��̬
	bool IsEnd(NFA n, set<int> s)
	{
		int endindex = n.tail->index;//nfa��̬
		set<int>::iterator it;
		//������״̬��������nfa״̬��
		for (it = s.begin(); it != s.end(); it++)
		{
			if (*it == endindex)  return true;
		}
		return false;
	}

	//�����-closure(s)
	//��-closure(s)��ʾ��״̬s���������ſ��Ե��������״̬�ļ��� 
	set<int> e_cloure(set<int> s)
	{
		stack<int> e_Stack;
		set<int>::iterator it;
		for (it = s.begin(); it != s.end(); it++)
		{
			e_Stack.push(*it);
		}
		while (!e_Stack.empty())
		{
			int temp = e_Stack.top();
			e_Stack.pop();
			set<int>::iterator iter;
			//ȡ����ǰ״̬���е�һ��״̬��������ͨ������ת������״̬��
			for (iter = NfaStates[temp].E_trans_to.begin(); iter != NfaStates[temp].E_trans_to.end(); iter++)
			{
				//�����ǰ״̬û���ڼ����г���
				if (!s.count(*iter))
				{	//���״̬���뼯����							
					s.insert(*iter);
					//���³�ʼ��״̬���������󣨴����ԣ�
					e_Stack.push(*iter);
				}
			}
		}
		return s;
	}

	//��һ��״̬��s�Ħ�-cloure(move(ch))
	//״̬A����ת��ͼ������ת������ch�õ���״̬
	set<int> moveE_ploure(set<int> s, char ch)
	{
		set<int> movecploure;
		set<int>::iterator it;
		//������ǰ����s�е�ÿ��Ԫ��
		for (it = s.begin(); it != s.end(); it++)
		{
			if (NfaStates[*it].input == ch)	//�����Ӧת�����ϵ�ֵ			
			{
				//��ͨ���û�ת������״̬���뵽������
				movecploure.insert(NfaStates[*it].transTo);
			}
		}
		movecploure = e_cloure(movecploure);
		return movecploure;
	}

	//NFA  to  DFA
	void NFAtoDFA(string str)
	{
		dfa.End_symbol = nfa.End_symbol;
		dfa.startState = 0;//����dfa��̬���Ϊ0
		set<int> tempSet;
		tempSet.insert(nfa.head->index);
		DfaStates[0].valid = true;
		DfaStates[0].closure = e_cloure(tempSet);//��dfa�ĳ�̬�Ħ�-cloure��
		DfaStates[0].isEnd = IsEnd(nfa, DfaStates[0].closure);//�Ƿ�Ϊ��̬
		dfa.Dfa_set.insert(pair<int, set<int>>(DFAStateNum, DfaStates[0].closure));
		DFAStateNum++;
		queue<int> q;
		q.push(dfa.startState);
		while (!q.empty())
		{
			int num = q.front();
			q.pop();
			set<char>::iterator it;
			for (it = dfa.End_symbol.begin(); it != dfa.End_symbol.end(); it++)
			{
				//���㵱ǰDFA״̬����ÿ���ս���Ħ�-cloure(move(ch))
				set<int> temp = moveE_ploure(DfaStates[num].closure, *it);
				bool have = false;
				int nSize = dfa.Dfa_set.size();
				//�ж��������״̬����֮ǰ�������״̬���Ƿ�ͬ
				for (int nindex = 0; nindex < nSize; nindex++)
				{
					set<int>SET = dfa.Dfa_set[nindex];
					if (temp == SET)
					{
						have = true;
						break;
					}
				}
				if (!temp.empty() && have == false)//�µ�״̬���������״̬����Ϊ������֮ǰ�������״̬����ͬ
				{
					//�����������״̬�����뵽״̬������
					dfa.Dfa_set.insert(pair<int, set<int>>(DFAStateNum, temp));
					//�µ�dfa״̬��������
					DfaState*NewState = &DfaStates[DFAStateNum];
					NewState->closure = temp;
					NewState->valid = true;
					//�ж��µ�dfa״̬�Ƿ�Ϊ��̬
					NewState->isEnd = IsEnd(nfa, NewState->closure);

					//��ǰ��dfa״̬ͨ�����ս��ת�Ƶ���״̬
					DfaState*NowState = &DfaStates[num];
					NowState->Edges[NowState->edgeNum].input = *it;
					NowState->Edges[NowState->edgeNum].transTo = DFAStateNum;//�µ�״̬��

					NowState->edgeNum++;
					//����ƫ��,��ǰ�ս�����ս�����е��±�
					int offerset = getoffset(*it);
					transMatrix[num][offerset] = DFAStateNum;//����״̬ת�Ʊ�

					q.push(DFAStateNum);//�µ�״̬��
					DFAStateNum++;
				}
				else //�������״̬����֮ǰ�����ĳ��״̬����ͬ
				{
					for (int i = 0; i < DFAStateNum; i++)
					{
						if (temp == DfaStates[i].closure)//�ҵ���ü�����ͬ��DFA״̬
						{
							//��ǰ��dfa״̬ת�Ƶ��Ѵ��ڵ�״̬
							DfaState*NowState = &DfaStates[num];
							NowState->Edges[NowState->edgeNum].input = *it;
							NowState->Edges[NowState->edgeNum].transTo = i;//�Ѵ��ڵ�״̬i
							NowState->edgeNum++;
							//����ƫ��,��ǰ�ս�����ս�����е��±�
							int offerset = getoffset(*it);
							transMatrix[num][offerset] = i;//����״̬ת�Ʊ�
							break;
						}
					}
				}
			}
		}
		for (int i = 0; i < DFAStateNum; i++)//����dfa����̬��
		{
			if (DfaStates[i].isEnd == true)
			{
				dfa.endStates.insert(i);
			}
		}
		saveDFA();
	}
	void saveDFA()
	{
		out << "***************     DFA     ***************" << '\n'<< '\n';
		int nSize = dfa.Dfa_set.size();
		out << "��NFA״̬���·�����:" << '\n';
		for (int nindex = 0; nindex < nSize; nindex++)
		{
			set<int>SET = dfa.Dfa_set[nindex];
			out << "(" << nindex << ")��";
			set<int>::iterator it;
			for (it = SET.begin(); it != SET.end(); it++)
			{
				out << *it << ", ";
			}
			out << '\n'<<'\n';
		}
		out << "DFA��״̬����:" << DFAStateNum << '\n';
		out << "��̬:" << dfa.startState << '\n'<< '\n';
		out << "�ս������ ";
		set<char>::iterator it;
		for (it = dfa.End_symbol.begin(); it != dfa.End_symbol.end(); it++)
		{
			out << *it << ", ";
		}
		out << '}' << endl << endl;
		out << "��̬��Ϊ�� ";
		set<int>::iterator iter;
		for (iter = dfa.endStates.begin(); iter != dfa.endStates.end(); iter++)
		{
			out << *iter << ", ";
		}
		out << '}' << '\n';
		out << endl << "״̬ת�Ʊ�" << '\n' << "\t";
		set<char>::iterator t;
		for (t = dfa.End_symbol.begin(); t != dfa.End_symbol.end(); t++)
		{
			out << *t << "\t";
		}
		out << '\n';
		for (int i = 0; i < DFAStateNum; i++)
		{
			if (dfa.endStates.count(i))
				out << '<' << i << ">\t";
			else
				out << ' ' << i << " \t";

			set<char>::iterator it;
			for (it = dfa.End_symbol.begin(); it != dfa.End_symbol.end(); it++)
			{
				if (transMatrix[i][getoffset(*it)] != -1)
					out << transMatrix[i][getoffset(*it)] << "\t";
				else
					out << "\t";
			}
			out << '\n';
		}
		out << '\n'<< '\n';
	}
};