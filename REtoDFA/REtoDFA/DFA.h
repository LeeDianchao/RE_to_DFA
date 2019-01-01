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

struct DFA			//定义DFA
{
	int startState;			//初态
	set<int> endStates;		//终态集
	set<char> End_symbol;	//终结符集
	map<int, set<int>> Dfa_set;   //存储合并后的dfa状态集(分组合并nfa的状态)
	DFA()
	{
		startState = 0;
	}
};

struct DFAtra_Edge	//DFA的转换
{
	char input;			//转换条件
	int transTo;		//转换到的状态号
	DFAtra_Edge()
	{
		input = ' ';
		transTo = -1;
	}
};
struct DfaState		//DFA状态
{
	bool valid;         //状态是否有效
	bool isEnd;			//是否为终态
	int index;			//状态号
	int edgeNum;		//DFA状态上的射出弧数
	DFAtra_Edge Edges[62]; //DFA状态上的射出弧 (0-9,A-Z,a-z)
	set<int> closure;      //NFA的ε-move()闭包
	DfaState()
	{
		valid = false;
		isEnd = false;
	}

};



class DFA_class
{
private:
	DfaState DfaStates[MAX];	//DFA状态数组
	int DFAStateNum;		//DFA状态总数
	vector<NfaState> NfaStates;	//NFA状态数组
	int NFAStateNum;			//NFA状态总数
	NFA nfa;
	DFA dfa;
	vector < vector<int>> transMatrix;//状态转移表
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
	//判断一个状态是否为终态
	bool IsEnd(NFA n, set<int> s)
	{
		int endindex = n.tail->index;//nfa终态
		set<int>::iterator it;
		//遍历该状态所包含的nfa状态集
		for (it = s.begin(); it != s.end(); it++)
		{
			if (*it == endindex)  return true;
		}
		return false;
	}

	//求出ε-closure(s)
	//ε-closure(s)表示由状态s经由条件ε可以到达的所有状态的集合 
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
			//取出当前状态集中的一个状态，遍历它通过ε能转换到的状态集
			for (iter = NfaStates[temp].E_trans_to.begin(); iter != NfaStates[temp].E_trans_to.end(); iter++)
			{
				//如果当前状态没有在集合中出现
				if (!s.count(*iter))
				{	//则把状态加入集合中							
					s.insert(*iter);
					//更新初始的状态集，向下求（传递性）
					e_Stack.push(*iter);
				}
			}
		}
		return s;
	}

	//求一个状态集s的ε-cloure(move(ch))
	//状态A经过转换图中所有转换条件ch得到的状态
	set<int> moveE_ploure(set<int> s, char ch)
	{
		set<int> movecploure;
		set<int>::iterator it;
		//遍历当前集合s中的每个元素
		for (it = s.begin(); it != s.end(); it++)
		{
			if (NfaStates[*it].input == ch)	//如果对应转换弧上的值			
			{
				//把通过该弧转换到的状态加入到集合中
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
		dfa.startState = 0;//假设dfa初态标号为0
		set<int> tempSet;
		tempSet.insert(nfa.head->index);
		DfaStates[0].valid = true;
		DfaStates[0].closure = e_cloure(tempSet);//求dfa的初态的ε-cloure集
		DfaStates[0].isEnd = IsEnd(nfa, DfaStates[0].closure);//是否为终态
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
				//计算当前DFA状态对于每个终结符的ε-cloure(move(ch))
				set<int> temp = moveE_ploure(DfaStates[num].closure, *it);
				bool have = false;
				int nSize = dfa.Dfa_set.size();
				//判断求出来的状态集与之前求出来的状态集是否不同
				for (int nindex = 0; nindex < nSize; nindex++)
				{
					set<int>SET = dfa.Dfa_set[nindex];
					if (temp == SET)
					{
						have = true;
						break;
					}
				}
				if (!temp.empty() && have == false)//新的状态集求出来的状态集不为空且与之前求出来的状态集不同
				{
					//将新求出来的状态集加入到状态集合中
					dfa.Dfa_set.insert(pair<int, set<int>>(DFAStateNum, temp));
					//新的dfa状态集，更新
					DfaState*NewState = &DfaStates[DFAStateNum];
					NewState->closure = temp;
					NewState->valid = true;
					//判断新的dfa状态是否为终态
					NewState->isEnd = IsEnd(nfa, NewState->closure);

					//当前的dfa状态通过该终结符转移到新状态
					DfaState*NowState = &DfaStates[num];
					NowState->Edges[NowState->edgeNum].input = *it;
					NowState->Edges[NowState->edgeNum].transTo = DFAStateNum;//新的状态号

					NowState->edgeNum++;
					//计算偏移,当前终结符在终结符集中的下标
					int offerset = getoffset(*it);
					transMatrix[num][offerset] = DFAStateNum;//更新状态转移表

					q.push(DFAStateNum);//新的状态号
					DFAStateNum++;
				}
				else //求出来的状态集在之前求出的某个状态集相同
				{
					for (int i = 0; i < DFAStateNum; i++)
					{
						if (temp == DfaStates[i].closure)//找到与该集合相同的DFA状态
						{
							//当前的dfa状态转移到已存在的状态
							DfaState*NowState = &DfaStates[num];
							NowState->Edges[NowState->edgeNum].input = *it;
							NowState->Edges[NowState->edgeNum].transTo = i;//已存在的状态i
							NowState->edgeNum++;
							//计算偏移,当前终结符在终结符集中的下标
							int offerset = getoffset(*it);
							transMatrix[num][offerset] = i;//更新状态转移表
							break;
						}
					}
				}
			}
		}
		for (int i = 0; i < DFAStateNum; i++)//计算dfa的终态集
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
		out << "将NFA状态重新分组编号:" << '\n';
		for (int nindex = 0; nindex < nSize; nindex++)
		{
			set<int>SET = dfa.Dfa_set[nindex];
			out << "(" << nindex << ")：";
			set<int>::iterator it;
			for (it = SET.begin(); it != SET.end(); it++)
			{
				out << *it << ", ";
			}
			out << '\n'<<'\n';
		}
		out << "DFA总状态个数:" << DFAStateNum << '\n';
		out << "初态:" << dfa.startState << '\n'<< '\n';
		out << "终结符集｛ ";
		set<char>::iterator it;
		for (it = dfa.End_symbol.begin(); it != dfa.End_symbol.end(); it++)
		{
			out << *it << ", ";
		}
		out << '}' << endl << endl;
		out << "终态集为｛ ";
		set<int>::iterator iter;
		for (iter = dfa.endStates.begin(); iter != dfa.endStates.end(); iter++)
		{
			out << *iter << ", ";
		}
		out << '}' << '\n';
		out << endl << "状态转移表：" << '\n' << "\t";
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