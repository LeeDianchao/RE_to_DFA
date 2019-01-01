#pragma once
#include"NFA.h"
#include"DFA.h"
#include<iostream>
#include<cstring>
#include<string>
#include<stack>
#include<vector>
#include<set>
#include<map>
#include<queue>
using namespace std;

struct DivideSet
{
	bool isdiv;//成功通过分割标准分割
	char div_char;//分割标准
	bool isStart;//是否包含初态
	bool isEnd;//是否包含终态
	set<int> Set;//分割集所包含的DFA状态编号
	DivideSet()
	{
		isdiv = false; isStart = false; isEnd = false;
	}
};

struct MinDfaState
{
	int index;//minDFA状态编号
	DivideSet mD_Set;//分割状态集
};

class MinDFA
{
private:
	vector<MinDfaState> minDfaStates;//minDfa状态数组
	int minDfaStateNum;			//minDfa的状态总数
	DfaState *DfaStates;	//DFA状态数组
	int DFAStateNum;		//DFA状态总数
	DFA dfa;
	vector < vector<int>> MintransMatrix;	//状态转移表
public:
	MinDFA(DFA_class dc)
	{
		minDfaStateNum = 0;
		DfaStates = dc.getDfaStates();
		DFAStateNum = dc.getDFAStateNum();
		dfa = dc.getDFA();
		MintransMatrix = vector < vector<int>>(DFAStateNum, vector<int>(62, -1));//(0-9,A-Z,a-z)
	}
	~MinDFA() {}

	set<int> S_A(set<int>S, set<int>A)//求差集
	{
		set<int>B;
		set<int>::iterator t;
		for (t = S.begin(); t != S.end(); t++)
		{
			int temp = *t;
			if (!A.count(temp))
			{
				B.insert(temp);
			}
		}
		return B;
	}

	bool isHaveEnd(set<int>endset, set<int>now)//是否存在包含终态
	{
		set<int>::iterator ti;
		for (ti = now.begin(); ti != now.end(); ti++)
		{
			int temp = *ti;
			if (endset.count(temp)) {
				return true;
			}
		}

		return false;
	}


	void Minimum_DFA()
	{
		DFA minDfa;
		minDfa.End_symbol = dfa.End_symbol;	//终结符集
		int startindex = 0;//DFA初态编号
		set<int> endSet;//DFA终态编号
		set<int> otherSet;
		//划分终态与非终态
		for (int i = 0; i < DFAStateNum; i++)
		{
			if (DfaStates[i].isEnd == false)		//如果该dfa状态不是终态
			{
				otherSet.insert(DfaStates[i].index);
			}
			else									//如果该dfa状态是终态
			{
				endSet.insert(DfaStates[i].index);
			}
		}

		queue< DivideSet> Div_Queue;//记录当前已将划分的状态集
		set<int> trueSet;//保存能转移分割到的状态
		set<int> falseSet;//保存能不能转移分割到的状态
		
		set<char>::iterator oit1;
		char Last_termsymbol;//最后一个终结符
		for (oit1 = minDfa.End_symbol.begin(); oit1 != minDfa.End_symbol.end(); oit1++)
		{
			Last_termsymbol = *oit1;
		}
		//识别出非终态集中能够转换到终态集中的状态
		//根据各个终结符划分状态
		for (oit1 = minDfa.End_symbol.begin(); oit1 != minDfa.End_symbol.end(); oit1++)
		{	//遍历状态集中的各个状态进行分割
			set<int>::iterator it2;
			for (it2 = otherSet.begin(); it2 != otherSet.end(); it2++)
			{
				int size = DfaStates[*it2].edgeNum;
				for (int i = 0; i < size; i++)
				{
					//找到非终态集中能够转换到终态集中的状态
					if (*oit1 == DfaStates[*it2].Edges[i].input && !otherSet.count(DfaStates[*it2].Edges[i].transTo))
					{
						trueSet.insert(*it2);
						break;
					}
				}
			}
			if (trueSet.size() != 0)//划分状态集，并且插入到Div_Queue
			{
				DivideSet d1;
				d1.div_char = *oit1;//分割终结符
				d1.isdiv = true;//通过分割
				d1.Set = trueSet;
				d1.isStart = trueSet.count(startindex);
				Div_Queue.push(d1);

				DivideSet d2;
				d2.div_char = *oit1;//分割终结符
				d2.isdiv = false;//不能分割
				falseSet = S_A(otherSet, trueSet);//差集
				d2.Set = falseSet;
				if (d2.Set.size() != 0)
				{
					d2.isStart = falseSet.count(startindex);
					Div_Queue.push(d2);
				}

				break;
			}
		}
		if (Div_Queue.empty())//若非终态集不需要分割
		{	//当前分割集不需要分割，生成新的最小化DFA状态
			DivideSet d1;
			d1.div_char = Last_termsymbol;
			d1.isdiv = false;
			d1.Set = endSet;
			d1.isStart = otherSet.count(startindex);
			MinDfaState Md;//生成新的最小化DFA状态
			Md.index = minDfaStateNum;
			Md.mD_Set = d1;
			minDfaStates.push_back(Md);
			minDfaStateNum++;

		}
		//分割其余状态
		while (!Div_Queue.empty())
		{
			//取出当前队列的首个划分集
			DivideSet nowDS = Div_Queue.front();
			Div_Queue.pop();
			trueSet.clear();
			falseSet.clear();
			set<int> nowSet = nowDS.Set;
			//根据划分集中的转移终结符的下一个终结符继续分割
			for (oit1 = minDfa.End_symbol.find(nowDS.div_char); oit1 != minDfa.End_symbol.end(); oit1++)
			{
				if (nowSet.size() == 1)//划分集中只有一个状态，生成新的最小化DFA状态
				{
					MinDfaState Md;
					Md.index = minDfaStateNum;
					Md.mD_Set = nowDS;
					minDfaStates.push_back(Md);
					minDfaStateNum++;
					break;
				}
				else
				{
					//oit1++;//根据划分集中的转移终结符的下一个终结符继续分割
					//遍历当前状态集中的各个状态进行分割
					set<int>::iterator it2;
					for (it2 = nowSet.begin(); it2 != nowSet.end(); it2++)
					{
						int size = DfaStates[*it2].edgeNum;
						for (int i = 0; i < size; i++)
						{
							if (*oit1 == DfaStates[*it2].Edges[i].input && !nowSet.count(DfaStates[*it2].Edges[i].transTo))
							{
								trueSet.insert(*it2);//找到新的分割
								break;
							}
						}
					}
					if (trueSet.size() != 0)//划分状态集，并且插入到Div_Queue
					{
						if (nowDS.div_char == *oit1&&nowSet == trueSet)
						{
							if (nowDS.div_char == Last_termsymbol)//最后一个终结符都无法分割，构造新的状态
							{
								MinDfaState Md;
								Md.index = minDfaStateNum;
								Md.mD_Set = nowDS;
								minDfaStates.push_back(Md);
								minDfaStateNum++;
								break;
							}
							else//尝试下一个终结符是否可以分割
							{
								trueSet.clear();
								continue;
							}
						}
						else
						{
							DivideSet d1;
							d1.div_char = *oit1;//分割终结符
							d1.isdiv = true;//通过分割
							d1.Set = trueSet;
							d1.isStart = trueSet.count(startindex);
							Div_Queue.push(d1);

							DivideSet d2;
							d2.div_char = *oit1;//分割终结符
							d2.isdiv = false;//不能分割
							falseSet = S_A(nowSet, trueSet);//差集
							d2.Set = falseSet;
							if (d2.Set.size() != 0)//falseSet为空集
							{
								d2.isStart = falseSet.count(startindex);
								Div_Queue.push(d2);
							}
							break;
						}
					}
					else
					{	
						if (nowDS.div_char == Last_termsymbol)//最后一个终结符都不能分割，构造新的状态
						{
							MinDfaState Md;
							Md.index = minDfaStateNum;
							Md.mD_Set = nowDS;
							minDfaStates.push_back(Md);
							minDfaStateNum++;
							break;
						}
						if (nowDS.div_char == *oit1)//尝试下一个终结符是否能分割
						{
							trueSet.clear();
							continue;
						}
						else//当前分割集不需要分割，生成新的最小化DFA状态
						{
							MinDfaState Md;
							Md.index = minDfaStateNum;
							Md.mD_Set = nowDS;
							minDfaStates.push_back(Md);
							minDfaStateNum++;
							break;
						}
					}
				}
			}
		}

		//识别出终态集中能够转换到非终态集中的状态
		set<char>::iterator it1;
		//根据各个终结符划分状态
		for (it1 = minDfa.End_symbol.begin(); it1 != minDfa.End_symbol.end(); it1++)
		{	//遍历状态集中的各个状态进行分割
			set<int>::iterator it2;
			for (it2 = endSet.begin(); it2 != endSet.end(); it2++)
			{
				int size = DfaStates[*it2].edgeNum;
				for (int i = 0; i < size; i++)
				{	//找到终态集中能够转换到非终态集中的状态
					if (*it1 == DfaStates[*it2].Edges[i].input && !endSet.count(DfaStates[*it2].Edges[i].transTo))
					{
						trueSet.insert(*it2);
						break;
					}
				}
			}
			if (trueSet.size() != 0)//划分状态集，并且插入到Div_Queue
			{
				DivideSet d1;
				d1.div_char = *it1;//分割终结符
				d1.isdiv = true;//通过分割
				d1.Set = trueSet;
				d1.isEnd = isHaveEnd(endSet, trueSet);
				Div_Queue.push(d1);

				DivideSet d2;
				d2.div_char = *it1;//分割终结符
				d2.isdiv = false;//不能分割
				falseSet = S_A(endSet, trueSet);//求差集
				d2.Set = falseSet;
				if (d2.Set.size() != 0)
				{
					d2.isEnd = isHaveEnd(endSet, falseSet);
					Div_Queue.push(d2);
				}

				break;
			}
		}
		if (Div_Queue.empty())//若终态集不需要分割
		{	//当前分割集不需要分割，生成新的最小化DFA状态
			DivideSet d1;
			d1.div_char = Last_termsymbol;
			d1.isdiv = false;
			d1.Set = endSet;
			d1.isEnd = isHaveEnd(endSet, endSet);
			MinDfaState Md;
			Md.index = minDfaStateNum;
			Md.mD_Set = d1;
			minDfaStates.push_back(Md);
			minDfaStateNum++;

		}

		//分割其余状态
		while (!Div_Queue.empty())
		{	//取出当前队列的首个划分集
			DivideSet nowDS = Div_Queue.front();
			Div_Queue.pop();
			trueSet.clear();
			falseSet.clear();
			set<int> nowSet = nowDS.Set;
			//根据划分集中的转移终结符的下一个终结符继续分割
			for (it1 = minDfa.End_symbol.find(nowDS.div_char); it1 != minDfa.End_symbol.end(); it1++)
			{
				if (nowSet.size() == 1)//划分集中只有一个状态，生成新的最小化DFA状态
				{
					MinDfaState Md;
					Md.index = minDfaStateNum;
					Md.mD_Set = nowDS;
					minDfaStates.push_back(Md);
					minDfaStateNum++;
					break;
				}
				else
				{
					//it1++;//根据划分集中的转移终结符的下一个终结符继续分割
					//遍历当前状态集中的各个状态进行分割
					set<int>::iterator it2;
					for (it2 = nowSet.begin(); it2 != nowSet.end(); it2++)
					{
						int size = DfaStates[*it2].edgeNum;
						for (int i = 0; i < size; i++)
						{
							if (*it1 == DfaStates[*it2].Edges[i].input && !nowSet.count(DfaStates[*it2].Edges[i].transTo))
							{
								trueSet.insert(*it2);//找到新的分割
								break;
							}
						}
					}
					if (trueSet.size() != 0)//划分状态集，并且插入到Div_Queue
					{
						if (nowDS.div_char == *it1&&nowSet == trueSet)
						{
							if (nowDS.div_char == Last_termsymbol)//最后一个终结符都无法分割，构造新的状态
							{
								MinDfaState Md;
								Md.index = minDfaStateNum;
								Md.mD_Set = nowDS;
								minDfaStates.push_back(Md);
								minDfaStateNum++;
								break;
							}
							else//尝试下一个终结符是否可以分割
							{
								trueSet.clear();
								continue;
							}

						}
						else
						{
							DivideSet d1;
							d1.div_char = *it1;//分割终结符
							d1.isdiv = true;//通过分割
							d1.Set = trueSet;
							d1.isStart = trueSet.count(startindex);
							Div_Queue.push(d1);

							DivideSet d2;
							d2.div_char = *it1;//分割终结符
							d2.isdiv = false;//不能分割
							falseSet = S_A(nowSet, trueSet);//差集
							d2.Set = falseSet;
							if (d2.Set.size() != 0)
							{
								d2.isStart = falseSet.count(startindex);
								Div_Queue.push(d2);
							}
							break;
						}
					}
					else
					{	
						if (nowDS.div_char == Last_termsymbol)//最后一个终结符都不能分割，构造新的状态
						{
							MinDfaState Md;
							Md.index = minDfaStateNum;
							Md.mD_Set = nowDS;
							minDfaStates.push_back(Md);
							minDfaStateNum++;
							break;
						}
						if (nowDS.div_char == *it1)//尝试下一个终结符是否能分割
						{
							trueSet.clear();
							continue;
						}
						else//当前分割集不需要分割，生成新的最小化DFA状态
						{
							MinDfaState Md;
							Md.index = minDfaStateNum;
							Md.mD_Set = nowDS;
							minDfaStates.push_back(Md);
							minDfaStateNum++;
							break;
						}
					}
				}
			}
		}
		for (int i = 0; i < minDfaStateNum; i++)//状态转移表
		{
			MinDfaState nowMin = minDfaStates[i];
			set<int>temp = nowMin.mD_Set.Set;
			set<int>::iterator it;
			for (it = temp.begin(); it != temp.end(); it++)
			{
				for (int j = 0; j < DfaStates[*it].edgeNum; j++)
				{
					int index = DfaStates[*it].Edges[j].transTo;
					char input = DfaStates[*it].Edges[j].input;
					int transTo = 0;
					for (int k = 0; k < minDfaStateNum; k++)
					{
						MinDfaState nowMin2 = minDfaStates[k];
						set<int>temp2 = nowMin2.mD_Set.Set;
						if (temp2.count(index))
						{
							MintransMatrix[i][getoffset(input)] = k;
							break;
						}
					}
				}
			}
		}
		saveMinDFA(minDfa, endSet);
	}
	void saveMinDFA(DFA minDfa, set<int>endSet)
	{
		out <<'\n';
		out << "***************     minDFA     ***************" << '\n' << '\n';
		out << "将DFA状态重新分组编号:" << '\n';
		for (int i = 0; i < minDfaStateNum; i++)
		{
			set<int> temp = minDfaStates[i].mD_Set.Set;

			if (isHaveEnd(endSet, temp))
			{
				out << "<" << i << ">：";
				minDfaStates[i].mD_Set.isEnd = true;
			}
			else
			{
				out << "(" << i << ")：";
			}
			set<int>::iterator it;
			for (it = temp.begin(); it != temp.end(); it++)
			{
				out << *it << ", ";
			}
			out << '\n';
		}

		out << '\n' << "minDFA总状态个数:" << minDfaStateNum << '\n';
		
		for (int i = 0; i < minDfaStateNum; i++)
		{
			if (minDfaStates[i].mD_Set.isStart) {
				out << "初态:" << i << '\n' << '\n';
				break;
			}
		}
		out << "终结符集｛ ";
		set<char>::iterator it;
		for (it = dfa.End_symbol.begin(); it != dfa.End_symbol.end(); it++)
		{
			out << *it << ", ";
		}
		out << '}' << endl << endl;
		out << "终态集为｛ ";
		for (int i = 0; i < minDfaStateNum; i++)
		{
			if (minDfaStates[i].mD_Set.isEnd)
				out << i << ", ";
		}
		out << '}' << '\n';
		out << '\n' << "状态转移表：" << '\n' << "\t";
		set<char>::iterator t;
		for (t = minDfa.End_symbol.begin(); t != minDfa.End_symbol.end(); t++)
		{
			out << *t << "\t";
		}
		out << '\n';
		for (int i = 0; i < minDfaStateNum; i++)
		{
			if (minDfaStates[i].mD_Set.isEnd)
				out << '<' << i << ">\t";
			else
				out << ' ' << i << " \t";
			set<char>::iterator it;
			for (it = minDfa.End_symbol.begin(); it != minDfa.End_symbol.end(); it++)
			{
				if (MintransMatrix[i][getoffset(*it)] != -1)
					out << MintransMatrix[i][getoffset(*it)] << "\t";
				else
					out << "\t";
			}
			out << '\n';
		}
		out << '\n';
		out.close();
	}
};
