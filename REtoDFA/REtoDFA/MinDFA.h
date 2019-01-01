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
	bool isdiv;//�ɹ�ͨ���ָ��׼�ָ�
	char div_char;//�ָ��׼
	bool isStart;//�Ƿ������̬
	bool isEnd;//�Ƿ������̬
	set<int> Set;//�ָ��������DFA״̬���
	DivideSet()
	{
		isdiv = false; isStart = false; isEnd = false;
	}
};

struct MinDfaState
{
	int index;//minDFA״̬���
	DivideSet mD_Set;//�ָ�״̬��
};

class MinDFA
{
private:
	vector<MinDfaState> minDfaStates;//minDfa״̬����
	int minDfaStateNum;			//minDfa��״̬����
	DfaState *DfaStates;	//DFA״̬����
	int DFAStateNum;		//DFA״̬����
	DFA dfa;
	vector < vector<int>> MintransMatrix;	//״̬ת�Ʊ�
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

	set<int> S_A(set<int>S, set<int>A)//��
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

	bool isHaveEnd(set<int>endset, set<int>now)//�Ƿ���ڰ�����̬
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
		minDfa.End_symbol = dfa.End_symbol;	//�ս����
		int startindex = 0;//DFA��̬���
		set<int> endSet;//DFA��̬���
		set<int> otherSet;
		//������̬�����̬
		for (int i = 0; i < DFAStateNum; i++)
		{
			if (DfaStates[i].isEnd == false)		//�����dfa״̬������̬
			{
				otherSet.insert(DfaStates[i].index);
			}
			else									//�����dfa״̬����̬
			{
				endSet.insert(DfaStates[i].index);
			}
		}

		queue< DivideSet> Div_Queue;//��¼��ǰ�ѽ����ֵ�״̬��
		set<int> trueSet;//������ת�Ʒָ��״̬
		set<int> falseSet;//�����ܲ���ת�Ʒָ��״̬
		
		set<char>::iterator oit1;
		char Last_termsymbol;//���һ���ս��
		for (oit1 = minDfa.End_symbol.begin(); oit1 != minDfa.End_symbol.end(); oit1++)
		{
			Last_termsymbol = *oit1;
		}
		//ʶ�������̬�����ܹ�ת������̬���е�״̬
		//���ݸ����ս������״̬
		for (oit1 = minDfa.End_symbol.begin(); oit1 != minDfa.End_symbol.end(); oit1++)
		{	//����״̬���еĸ���״̬���зָ�
			set<int>::iterator it2;
			for (it2 = otherSet.begin(); it2 != otherSet.end(); it2++)
			{
				int size = DfaStates[*it2].edgeNum;
				for (int i = 0; i < size; i++)
				{
					//�ҵ�����̬�����ܹ�ת������̬���е�״̬
					if (*oit1 == DfaStates[*it2].Edges[i].input && !otherSet.count(DfaStates[*it2].Edges[i].transTo))
					{
						trueSet.insert(*it2);
						break;
					}
				}
			}
			if (trueSet.size() != 0)//����״̬�������Ҳ��뵽Div_Queue
			{
				DivideSet d1;
				d1.div_char = *oit1;//�ָ��ս��
				d1.isdiv = true;//ͨ���ָ�
				d1.Set = trueSet;
				d1.isStart = trueSet.count(startindex);
				Div_Queue.push(d1);

				DivideSet d2;
				d2.div_char = *oit1;//�ָ��ս��
				d2.isdiv = false;//���ָܷ�
				falseSet = S_A(otherSet, trueSet);//�
				d2.Set = falseSet;
				if (d2.Set.size() != 0)
				{
					d2.isStart = falseSet.count(startindex);
					Div_Queue.push(d2);
				}

				break;
			}
		}
		if (Div_Queue.empty())//������̬������Ҫ�ָ�
		{	//��ǰ�ָ����Ҫ�ָ�����µ���С��DFA״̬
			DivideSet d1;
			d1.div_char = Last_termsymbol;
			d1.isdiv = false;
			d1.Set = endSet;
			d1.isStart = otherSet.count(startindex);
			MinDfaState Md;//�����µ���С��DFA״̬
			Md.index = minDfaStateNum;
			Md.mD_Set = d1;
			minDfaStates.push_back(Md);
			minDfaStateNum++;

		}
		//�ָ�����״̬
		while (!Div_Queue.empty())
		{
			//ȡ����ǰ���е��׸����ּ�
			DivideSet nowDS = Div_Queue.front();
			Div_Queue.pop();
			trueSet.clear();
			falseSet.clear();
			set<int> nowSet = nowDS.Set;
			//���ݻ��ּ��е�ת���ս������һ���ս�������ָ�
			for (oit1 = minDfa.End_symbol.find(nowDS.div_char); oit1 != minDfa.End_symbol.end(); oit1++)
			{
				if (nowSet.size() == 1)//���ּ���ֻ��һ��״̬�������µ���С��DFA״̬
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
					//oit1++;//���ݻ��ּ��е�ת���ս������һ���ս�������ָ�
					//������ǰ״̬���еĸ���״̬���зָ�
					set<int>::iterator it2;
					for (it2 = nowSet.begin(); it2 != nowSet.end(); it2++)
					{
						int size = DfaStates[*it2].edgeNum;
						for (int i = 0; i < size; i++)
						{
							if (*oit1 == DfaStates[*it2].Edges[i].input && !nowSet.count(DfaStates[*it2].Edges[i].transTo))
							{
								trueSet.insert(*it2);//�ҵ��µķָ�
								break;
							}
						}
					}
					if (trueSet.size() != 0)//����״̬�������Ҳ��뵽Div_Queue
					{
						if (nowDS.div_char == *oit1&&nowSet == trueSet)
						{
							if (nowDS.div_char == Last_termsymbol)//���һ���ս�����޷��ָ�����µ�״̬
							{
								MinDfaState Md;
								Md.index = minDfaStateNum;
								Md.mD_Set = nowDS;
								minDfaStates.push_back(Md);
								minDfaStateNum++;
								break;
							}
							else//������һ���ս���Ƿ���Էָ�
							{
								trueSet.clear();
								continue;
							}
						}
						else
						{
							DivideSet d1;
							d1.div_char = *oit1;//�ָ��ս��
							d1.isdiv = true;//ͨ���ָ�
							d1.Set = trueSet;
							d1.isStart = trueSet.count(startindex);
							Div_Queue.push(d1);

							DivideSet d2;
							d2.div_char = *oit1;//�ָ��ս��
							d2.isdiv = false;//���ָܷ�
							falseSet = S_A(nowSet, trueSet);//�
							d2.Set = falseSet;
							if (d2.Set.size() != 0)//falseSetΪ�ռ�
							{
								d2.isStart = falseSet.count(startindex);
								Div_Queue.push(d2);
							}
							break;
						}
					}
					else
					{	
						if (nowDS.div_char == Last_termsymbol)//���һ���ս�������ָܷ�����µ�״̬
						{
							MinDfaState Md;
							Md.index = minDfaStateNum;
							Md.mD_Set = nowDS;
							minDfaStates.push_back(Md);
							minDfaStateNum++;
							break;
						}
						if (nowDS.div_char == *oit1)//������һ���ս���Ƿ��ָܷ�
						{
							trueSet.clear();
							continue;
						}
						else//��ǰ�ָ����Ҫ�ָ�����µ���С��DFA״̬
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

		//ʶ�����̬�����ܹ�ת��������̬���е�״̬
		set<char>::iterator it1;
		//���ݸ����ս������״̬
		for (it1 = minDfa.End_symbol.begin(); it1 != minDfa.End_symbol.end(); it1++)
		{	//����״̬���еĸ���״̬���зָ�
			set<int>::iterator it2;
			for (it2 = endSet.begin(); it2 != endSet.end(); it2++)
			{
				int size = DfaStates[*it2].edgeNum;
				for (int i = 0; i < size; i++)
				{	//�ҵ���̬�����ܹ�ת��������̬���е�״̬
					if (*it1 == DfaStates[*it2].Edges[i].input && !endSet.count(DfaStates[*it2].Edges[i].transTo))
					{
						trueSet.insert(*it2);
						break;
					}
				}
			}
			if (trueSet.size() != 0)//����״̬�������Ҳ��뵽Div_Queue
			{
				DivideSet d1;
				d1.div_char = *it1;//�ָ��ս��
				d1.isdiv = true;//ͨ���ָ�
				d1.Set = trueSet;
				d1.isEnd = isHaveEnd(endSet, trueSet);
				Div_Queue.push(d1);

				DivideSet d2;
				d2.div_char = *it1;//�ָ��ս��
				d2.isdiv = false;//���ָܷ�
				falseSet = S_A(endSet, trueSet);//��
				d2.Set = falseSet;
				if (d2.Set.size() != 0)
				{
					d2.isEnd = isHaveEnd(endSet, falseSet);
					Div_Queue.push(d2);
				}

				break;
			}
		}
		if (Div_Queue.empty())//����̬������Ҫ�ָ�
		{	//��ǰ�ָ����Ҫ�ָ�����µ���С��DFA״̬
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

		//�ָ�����״̬
		while (!Div_Queue.empty())
		{	//ȡ����ǰ���е��׸����ּ�
			DivideSet nowDS = Div_Queue.front();
			Div_Queue.pop();
			trueSet.clear();
			falseSet.clear();
			set<int> nowSet = nowDS.Set;
			//���ݻ��ּ��е�ת���ս������һ���ս�������ָ�
			for (it1 = minDfa.End_symbol.find(nowDS.div_char); it1 != minDfa.End_symbol.end(); it1++)
			{
				if (nowSet.size() == 1)//���ּ���ֻ��һ��״̬�������µ���С��DFA״̬
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
					//it1++;//���ݻ��ּ��е�ת���ս������һ���ս�������ָ�
					//������ǰ״̬���еĸ���״̬���зָ�
					set<int>::iterator it2;
					for (it2 = nowSet.begin(); it2 != nowSet.end(); it2++)
					{
						int size = DfaStates[*it2].edgeNum;
						for (int i = 0; i < size; i++)
						{
							if (*it1 == DfaStates[*it2].Edges[i].input && !nowSet.count(DfaStates[*it2].Edges[i].transTo))
							{
								trueSet.insert(*it2);//�ҵ��µķָ�
								break;
							}
						}
					}
					if (trueSet.size() != 0)//����״̬�������Ҳ��뵽Div_Queue
					{
						if (nowDS.div_char == *it1&&nowSet == trueSet)
						{
							if (nowDS.div_char == Last_termsymbol)//���һ���ս�����޷��ָ�����µ�״̬
							{
								MinDfaState Md;
								Md.index = minDfaStateNum;
								Md.mD_Set = nowDS;
								minDfaStates.push_back(Md);
								minDfaStateNum++;
								break;
							}
							else//������һ���ս���Ƿ���Էָ�
							{
								trueSet.clear();
								continue;
							}

						}
						else
						{
							DivideSet d1;
							d1.div_char = *it1;//�ָ��ս��
							d1.isdiv = true;//ͨ���ָ�
							d1.Set = trueSet;
							d1.isStart = trueSet.count(startindex);
							Div_Queue.push(d1);

							DivideSet d2;
							d2.div_char = *it1;//�ָ��ս��
							d2.isdiv = false;//���ָܷ�
							falseSet = S_A(nowSet, trueSet);//�
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
						if (nowDS.div_char == Last_termsymbol)//���һ���ս�������ָܷ�����µ�״̬
						{
							MinDfaState Md;
							Md.index = minDfaStateNum;
							Md.mD_Set = nowDS;
							minDfaStates.push_back(Md);
							minDfaStateNum++;
							break;
						}
						if (nowDS.div_char == *it1)//������һ���ս���Ƿ��ָܷ�
						{
							trueSet.clear();
							continue;
						}
						else//��ǰ�ָ����Ҫ�ָ�����µ���С��DFA״̬
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
		for (int i = 0; i < minDfaStateNum; i++)//״̬ת�Ʊ�
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
		out << "��DFA״̬���·�����:" << '\n';
		for (int i = 0; i < minDfaStateNum; i++)
		{
			set<int> temp = minDfaStates[i].mD_Set.Set;

			if (isHaveEnd(endSet, temp))
			{
				out << "<" << i << ">��";
				minDfaStates[i].mD_Set.isEnd = true;
			}
			else
			{
				out << "(" << i << ")��";
			}
			set<int>::iterator it;
			for (it = temp.begin(); it != temp.end(); it++)
			{
				out << *it << ", ";
			}
			out << '\n';
		}

		out << '\n' << "minDFA��״̬����:" << minDfaStateNum << '\n';
		
		for (int i = 0; i < minDfaStateNum; i++)
		{
			if (minDfaStates[i].mD_Set.isStart) {
				out << "��̬:" << i << '\n' << '\n';
				break;
			}
		}
		out << "�ս������ ";
		set<char>::iterator it;
		for (it = dfa.End_symbol.begin(); it != dfa.End_symbol.end(); it++)
		{
			out << *it << ", ";
		}
		out << '}' << endl << endl;
		out << "��̬��Ϊ�� ";
		for (int i = 0; i < minDfaStateNum; i++)
		{
			if (minDfaStates[i].mD_Set.isEnd)
				out << i << ", ";
		}
		out << '}' << '\n';
		out << '\n' << "״̬ת�Ʊ�" << '\n' << "\t";
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
