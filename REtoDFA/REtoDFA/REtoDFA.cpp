// REtoDFA.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include"NFA.h"
#include"DFA.h"
#include"MinDFA.h"
#include"windows.h"

using namespace std;


void main()
{
	cout << "输入正确的正则式:" << endl;
	string str;
	//str = "a(b*a)*|b(a*b)*";
	//str = "(a|b)*abb";
	//str="(a|b)*(aa|bb)(a|b)*";
	cin >> str;

	NFA_class nc;
	nc.ReToNFA(str);

	DFA_class dc(nc);
	dc.NFAtoDFA(str);

	MinDFA md(dc);
	md.Minimum_DFA();
	WinExec("notepad.exe 结果输出.txt", SW_SHOW);
	
}
