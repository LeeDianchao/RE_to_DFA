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

ofstream out("结果输出.txt");

struct NfaState				//NFA状态
{
	bool valid;             //状态是否有效
	int index;				//状态号
	char input;				//转移条件
	int transTo;			//通过转移条件转移到的状态号
	set<int> E_trans_to;	//当前状态通过ε转移到的状态号集合
	NfaState()
	{
		valid = false;
		input = ' ';
		transTo = -1;
	}
};

struct NFA
{
	NfaState *head;			//NFA的头指针
	NfaState *tail;			//NFA的尾指针
	set<char> End_symbol;	//终结符集
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

int getoffset(char c)//计算终结符c在终结符集中的位置
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
	vector<NfaState> NfaStates;	//NFA状态数组
	int NFAStateNum;			//NFA状态总数
	NFA resultNFA;
	vector < vector<int>> transMatrix;//状态转移表
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
	int Level(char ch)//运算符优先级
	{
		if (ch == '(')
		{
			return 0;
		}
		else if (ch == '|')//或
		{
			return 1;
		}
		else if (ch == '.')//连接
		{
			return 2;
		}
		else if (ch == '*')//闭包
		{
			return 3;
		}
	}
	
	string InfixtoSuffix(string s)//中缀表达式转后缀表达式
	{
		//对字符串s进行预处理，插入隐含连接符 '.'
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
		string suffix;				//输出的后缀字符串
		stack<char> oper_stack;		//运算符栈
		for (int i = 0; i < s.size(); i++)
		{
			if ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z'))	//操作数直接输出
			{
				suffix += s[i];
			}
			else
			{
				if (s[i] == '(')			//遇到左括号压入栈中
				{
					oper_stack.push(s[i]);
				}
				else if (s[i] == ')')		//遇到右括号时
				{
					char ch = oper_stack.top();
					while (ch != '(')		//将栈中元素出栈，直到栈顶为左括号
					{
						suffix += ch;
						oper_stack.pop();
						ch = oper_stack.top();
					}
					oper_stack.pop();		//最后将左括号出栈
				}
				else						//遇到其他操作符时
				{
					if (oper_stack.empty())	//符号栈为空，直接将运算符入栈
					{
						oper_stack.push(s[i]);
					}
					else				//符号栈不为空
					{
						char ch = oper_stack.top();
						while (Level(ch) >= Level(s[i]))//弹出栈中优先级大于等于当前运算符的运算符
						{
							suffix += ch;
							oper_stack.pop();
							if (oper_stack.empty())	//符号栈为空则结束循环
							{
								break;
							}
							else ch = oper_stack.top();
						}
						oper_stack.push(s[i]);		//将当前运算符入栈
					}
				}
			}
		}
		//最后如果栈不为空，则出栈并输出到字符串
		while (!oper_stack.empty())
		{
			char ch = oper_stack.top();
			oper_stack.pop();
			suffix += ch;
		}
		out << '\n' ;
		out << "中缀表达式为：" << s << '\n' << '\n';
		out << "后缀表达式为：" << suffix << '\n' << '\n';
		cout << '\n' << "后缀表达式为：" << suffix << '\n' << '\n';
		return suffix;
	}

	//从状态n1通过 value 转为状态n2
	void addEdge(NfaState *n1, NfaState *n2, char value)
	{
		n1->input = value;
		n1->transTo = n2->index;
	}

	//从状态n1通过值为 ε 转为状态n2
	void E_addEdge(NfaState *n1, NfaState *n2)
	{
		int transTo = n2->index;
		n1->E_trans_to.insert(transTo);
	}

	//后缀表达式转NFA
	void ReToNFA(string &str)
	{
		str = InfixtoSuffix(str);//将中缀表达式转换为后缀表达式
		stack<NFA> NfaStack;		//NFA栈
		NFA T_symbol;
		for (int i = 0; i < str.size(); i++)//Thompson方法构造NFA
		{
			if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))
			{
				NFA n = NFA(NFAStateNum,NfaStates);//新建一个NFA，为状态(NFAStateNum)
				NFAStateNum += 2;
				addEdge(n.head, n.tail, str[i]);//转换条件为s[i]
				NfaStack.push(n);
				T_symbol.End_symbol.insert(str[i]);//终结符集
			}
			else if (str[i] == '*')		 //闭包运算符
			{
				NFA n = NFA(NFAStateNum, NfaStates);	//新建一个NFA
				NFAStateNum += 2;

				NFA n1 = NfaStack.top();	//从栈中弹出一个NFA
				NfaStack.pop();

				E_addEdge(n.head, n1.head);//通过空字转换
				E_addEdge(n.head, n.tail); //通过空字转换
				E_addEdge(n1.tail, n.head);//通过空字转换
				E_addEdge(n1.tail, n.tail);//通过空字转换
				NfaStack.push(n);
			}
			else if (str[i] == '.')		//遇到连接运算符
			{
				NFA n2 = NfaStack.top();//栈顶为n2	
				NfaStack.pop();
				NFA n1 = NfaStack.top();//次栈顶为n1
				NfaStack.pop();
				E_addEdge(n1.tail, n2.head);//n1的尾通过ε到n2的头
				NFA n;				//定义一个新的NFA n
				n.head = n1.head;	//n的头为n1的头
				n.tail = n2.tail;	//n的尾为n2的尾
				NfaStack.push(n);
			}
			else if (str[i] == '|')		//遇到或运算符
			{
				NFA n2 = NfaStack.top();//栈顶为n2
				NfaStack.pop();
				NFA n1 = NfaStack.top();//次栈顶为n1
				NfaStack.pop();
				NFA n = NFA(NFAStateNum, NfaStates);//新建一个NFA，为状态(NFAStateNum)
				NFAStateNum += 2;

				E_addEdge(n.head, n1.head);	//通过空字转换
				E_addEdge(n.head, n2.head);	//通过空字转换
				E_addEdge(n1.tail, n.tail);	//通过空字转换
				E_addEdge(n2.tail, n.tail);	//通过空字转换
				NfaStack.push(n);
			}
		}
		resultNFA = NfaStack.top();//最后的栈顶元素即为生成好的NFA
		resultNFA.End_symbol = T_symbol.End_symbol;//终结符
		saveNFA();
	}

	void saveNFA()
	{
		out << "***************     NFA     ***************" << '\n' << '\n';
		out << "NFA总状态个数:" << NFAStateNum << '\n';
		out << "初态:" << resultNFA.head->index << '\n';
		out << "终态:" << resultNFA.tail->index << '\n';
		out << '\n' << "转移函数为：" << '\n';
		for (int i = 0; i < NFAStateNum; i++)	//遍历NFA状态数组
		{
			set<int>::iterator it;			//输出该状态经过ε到达的状态
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
			if (NfaStates[i].input != ' ')	//输出该状态经过转移条件到达的状态
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


