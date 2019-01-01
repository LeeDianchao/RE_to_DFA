#pragma once
#include<iostream>
#include<cstring>
#include<string>
#include<stack>
#include<vector>
#include<set>
#include<map>
#include<queue>
#include <fstream>
using namespace std;
const int MAX = 256;

ofstream out("������.txt");

struct NfaState				//NFA״̬
{
	bool valid;             //״̬�Ƿ���Ч
	int index;				//״̬��
	char input;				//ת������
	int transTo;			//ͨ��ת������ת�Ƶ���״̬��
	set<int> E_trans_to;	//��ǰ״̬ͨ����ת�Ƶ���״̬�ż���
	NfaState()
	{
		valid = false;
		input = ' ';
		transTo = -1;
	}
};

struct NFA
{
	NfaState *head;			//NFA��ͷָ��
	NfaState *tail;			//NFA��βָ��
	set<char> End_symbol;	//�ս����
	NFA() {}
	NFA(int now_statenum, vector<NfaState> &NfaStates)
	{
		/*NfaState now_state1;
		now_state1.index = now_statenum;
		NfaStates.push_back(now_state1);
		NfaState now_state2;
		now_state2.index = now_statenum + 1;
		NfaStates.push_back(now_state2);*/
		head = &NfaStates[now_statenum];
		tail = &NfaStates[now_statenum + 1];
		NfaStates[now_statenum].valid = true;
		NfaStates[now_statenum + 1].valid = true;
	}
	
};

int getoffset(char c)//�����ս��c���ս�����е�λ��
{
	int k = (int)c;
	int offset = 0;
	if (k >= 48 && k <= 57)
	{
		offset = k - 48;
	}
	else if (k >= 65 && k <= 90)
	{
		offset = k - 65 + 10;
	}
	else if (k >= 97 && k <= 122)
	{
		offset = k - 97 + 36;
	}
	return offset;
}

class NFA_class
{
private:
	vector<NfaState> NfaStates;	//NFA״̬����
	int NFAStateNum;			//NFA״̬����
	NFA resultNFA;
	vector < vector<int>> transMatrix;//״̬ת�Ʊ�
public:
	NFA_class()
	{
		NfaStates = vector<NfaState>(MAX);
		NFAStateNum = 0;
		for (int i = 0; i < MAX; i++)
		{
			NfaStates[i].index = i;
		}
	}
	~NFA_class() {}
	int getNFAStateNum()
	{
		return NFAStateNum;
	}
	vector<NfaState> getNfaStates()
	{
		return NfaStates;
	}
	NFA getResultNFA()
	{
		return resultNFA;
	}
	int Level(char ch)//��������ȼ�
	{
		if (ch == '(')
		{
			return 0;
		}
		else if (ch == '|')//��
		{
			return 1;
		}
		else if (ch == '.')//����
		{
			return 2;
		}
		else if (ch == '*')//�հ�
		{
			return 3;
		}
	}
	
	string InfixtoSuffix(string s)//��׺���ʽת��׺���ʽ
	{
		//���ַ���s����Ԥ���������������ӷ� '.'
		int i = 0, length = s.size();
		while (i < length)
		{
			if ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] == '*') || (s[i] == ')'))
			{
				if ((s[i + 1] >= '0' && s[i + 1] <= '9') || (s[i + 1] >= 'a' && s[i + 1] <= 'z') || (s[i + 1] >= 'A' && s[i + 1] <= 'Z') || s[i + 1] == '(')
				{
					s += '$';
					for (int j = s.size() - 1; j > i + 1; j--)
					{
						s[j] = s[j - 1];
					}
					s[i + 1] = '.';
					length++;
				}
			}
			i++;
		}
		string suffix;				//����ĺ�׺�ַ���
		stack<char> oper_stack;		//�����ջ
		for (int i = 0; i < s.size(); i++)
		{
			if ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z'))	//������ֱ�����
			{
				suffix += s[i];
			}
			else
			{
				if (s[i] == '(')			//����������ѹ��ջ��
				{
					oper_stack.push(s[i]);
				}
				else if (s[i] == ')')		//����������ʱ
				{
					char ch = oper_stack.top();
					while (ch != '(')		//��ջ��Ԫ�س�ջ��ֱ��ջ��Ϊ������
					{
						suffix += ch;
						oper_stack.pop();
						ch = oper_stack.top();
					}
					oper_stack.pop();		//��������ų�ջ
				}
				else						//��������������ʱ
				{
					if (oper_stack.empty())	//����ջΪ�գ�ֱ�ӽ��������ջ
					{
						oper_stack.push(s[i]);
					}
					else				//����ջ��Ϊ��
					{
						char ch = oper_stack.top();
						while (Level(ch) >= Level(s[i]))//����ջ�����ȼ����ڵ��ڵ�ǰ������������
						{
							suffix += ch;
							oper_stack.pop();
							if (oper_stack.empty())	//����ջΪ�������ѭ��
							{
								break;
							}
							else ch = oper_stack.top();
						}
						oper_stack.push(s[i]);		//����ǰ�������ջ
					}
				}
			}
		}
		//������ջ��Ϊ�գ����ջ��������ַ���
		while (!oper_stack.empty())
		{
			char ch = oper_stack.top();
			oper_stack.pop();
			suffix += ch;
		}
		out << '\n' ;
		out << "��׺���ʽΪ��" << s << '\n' << '\n';
		out << "��׺���ʽΪ��" << suffix << '\n' << '\n';
		cout << '\n' << "��׺���ʽΪ��" << suffix << '\n' << '\n';
		return suffix;
	}

	//��״̬n1ͨ�� value תΪ״̬n2
	void addEdge(NfaState *n1, NfaState *n2, char value)
	{
		n1->input = value;
		n1->transTo = n2->index;
	}

	//��״̬n1ͨ��ֵΪ �� תΪ״̬n2
	void E_addEdge(NfaState *n1, NfaState *n2)
	{
		int transTo = n2->index;
		n1->E_trans_to.insert(transTo);
	}

	//��׺���ʽתNFA
	void ReToNFA(string &str)
	{
		str = InfixtoSuffix(str);//����׺���ʽת��Ϊ��׺���ʽ
		stack<NFA> NfaStack;		//NFAջ
		NFA T_symbol;
		for (int i = 0; i < str.size(); i++)//Thompson��������NFA
		{
			if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))
			{
				NFA n = NFA(NFAStateNum,NfaStates);//�½�һ��NFA��Ϊ״̬(NFAStateNum)
				NFAStateNum += 2;
				addEdge(n.head, n.tail, str[i]);//ת������Ϊs[i]
				NfaStack.push(n);
				T_symbol.End_symbol.insert(str[i]);//�ս����
			}
			else if (str[i] == '*')		 //�հ������
			{
				NFA n = NFA(NFAStateNum, NfaStates);	//�½�һ��NFA
				NFAStateNum += 2;

				NFA n1 = NfaStack.top();	//��ջ�е���һ��NFA
				NfaStack.pop();

				E_addEdge(n.head, n1.head);//ͨ������ת��
				E_addEdge(n.head, n.tail); //ͨ������ת��
				E_addEdge(n1.tail, n.head);//ͨ������ת��
				E_addEdge(n1.tail, n.tail);//ͨ������ת��
				NfaStack.push(n);
			}
			else if (str[i] == '.')		//�������������
			{
				NFA n2 = NfaStack.top();//ջ��Ϊn2	
				NfaStack.pop();
				NFA n1 = NfaStack.top();//��ջ��Ϊn1
				NfaStack.pop();
				E_addEdge(n1.tail, n2.head);//n1��βͨ���ŵ�n2��ͷ
				NFA n;				//����һ���µ�NFA n
				n.head = n1.head;	//n��ͷΪn1��ͷ
				n.tail = n2.tail;	//n��βΪn2��β
				NfaStack.push(n);
			}
			else if (str[i] == '|')		//�����������
			{
				NFA n2 = NfaStack.top();//ջ��Ϊn2
				NfaStack.pop();
				NFA n1 = NfaStack.top();//��ջ��Ϊn1
				NfaStack.pop();
				NFA n = NFA(NFAStateNum, NfaStates);//�½�һ��NFA��Ϊ״̬(NFAStateNum)
				NFAStateNum += 2;

				E_addEdge(n.head, n1.head);	//ͨ������ת��
				E_addEdge(n.head, n2.head);	//ͨ������ת��
				E_addEdge(n1.tail, n.tail);	//ͨ������ת��
				E_addEdge(n2.tail, n.tail);	//ͨ������ת��
				NfaStack.push(n);
			}
		}
		resultNFA = NfaStack.top();//����ջ��Ԫ�ؼ�Ϊ���ɺõ�NFA
		resultNFA.End_symbol = T_symbol.End_symbol;//�ս��
		saveNFA();
	}

	void saveNFA()
	{
		out << "***************     NFA     ***************" << '\n' << '\n';
		out << "NFA��״̬����:" << NFAStateNum << '\n';
		out << "��̬:" << resultNFA.head->index << '\n';
		out << "��̬:" << resultNFA.tail->index << '\n';
		out << '\n' << "ת�ƺ���Ϊ��" << '\n';
		for (int i = 0; i < NFAStateNum; i++)	//����NFA״̬����
		{
			set<int>::iterator it;			//�����״̬�����ŵ����״̬
			for (it = NfaStates[i].E_trans_to.begin(); it != NfaStates[i].E_trans_to.end(); it++)
			{
				if(NfaStates[i].index== resultNFA.head->index)
					out<< "("<<NfaStates[i].index << ")-->'" << ' ' << "'-->";
				else
					out << NfaStates[i].index << "-->'" << ' ' << "'-->";
				if (*it == resultNFA.tail->index)
					out << "<" << *it << ">\t";
				else
					out << *it << '\t';
			}
			if (NfaStates[i].input != ' ')	//�����״̬����ת�����������״̬
			{
				out<< NfaStates[i].index << "-->'" << NfaStates[i].input << "'-->" ;
				if (NfaStates[i].transTo == resultNFA.tail->index)
					out << "<" << NfaStates[i].transTo << ">\t";
				else
					out << NfaStates[i].transTo << '\t';
			}
			out << '\n';
		}
		out << '\n';
	}
};


