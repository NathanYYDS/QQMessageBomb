// Message2.0版本包含的头文件文件
#include "pch.h"
#include "framework.h"
#include "Message.h"
#include "MessageDlg.h"
#include "afxdialogex.h"
#include "Windows.h"
#include "stdio.h"
#include "stdlib.h"
#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//定义所需要的变量
CString user, words, str, Twait;
char name[100];
int times, num = 0, state = BST_UNCHECKED, result;
float wait;
HWND hwnd;
HGLOBAL clipbuffer;

// CMessage20Dlg 对话框
CMessage20Dlg::CMessage20Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MESSAGE20_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMessage20Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMessage20Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CMessage20Dlg::OnBnClickedSend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PICTURE, &CMessage20Dlg::OnBnClickedPicture)
END_MESSAGE_MAP()

// CMessage20Dlg 消息处理程序
BOOL CMessage20Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CMessage20Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMessage20Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//发送函数
void CMessage20Dlg::OnBnClickedSend()
{
	// 初始化数据
	num = 0;
	times = GetDlgItemInt(IDC_TIMES, 0, 1);
	GetDlgItemText(IDC_WAIT, Twait);
	wait = atof(Twait);
	GetDlgItemText(IDC_USER, user);
	if (state != BST_CHECKED)
		GetDlgItemText(IDC_WORDS, words);
	//判断数据是否正确
	if (user == "")
	{
		MessageBox("发送目的地不可为空！", "警告", MB_OK | MB_ICONERROR);
		return;
	}
	if (state != BST_CHECKED)
		if (words == "")
		{
			MessageBox("发送内容不可为空！", "警告", MB_OK | MB_ICONERROR);
			return;
		}

	if (state == BST_CHECKED)
	{
		result = MessageBox("即将开始发送\n请确认是否已复制图片文件", "警告", MB_YESNO | MB_ICONWARNING);
		if (result == IDNO)
			return;
	}

	if (times < 1 || times > 10000000000)
	{
		MessageBox("发送次数不可低于 1 次！\n发送次数不可高于 10000000000 次！", "警告", MB_OK | MB_ICONERROR);
		return;
	}

	if (wait < 0 || wait > 10000)
	{
		MessageBox("发送间隔时间不可低于 0 秒每次！\n发送次数不可高于 10000 秒每次！", "警告", MB_OK | MB_ICONERROR);
		return;
	}
	//判断窗口是否存在
	hwnd = ::FindWindow(NULL, user);
	if (hwnd == NULL)
	{
		sprintf_s(name, "找不到以“ %s ”命名的窗口", user);
		MessageBox(name, "警告", MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		GetDlgItem(IDC_SEND)->GetWindowText(str);

		if (str == "开始发送")
		{
			//设置定时器
			SetTimer(1, wait * 1000, NULL);
			GetDlgItem(IDC_SEND)->SetWindowText("停止发送");
			GetDlgItem(IDC_USER)->EnableWindow(FALSE);
			GetDlgItem(IDC_WORDS)->EnableWindow(FALSE);
			GetDlgItem(IDC_TIMES)->EnableWindow(FALSE);
			GetDlgItem(IDC_WAIT)->EnableWindow(FALSE);
		}
		else
		{
			//设置按键
			KillTimer(1);
			GetDlgItem(IDC_SEND)->SetWindowText("开始发送");
			GetDlgItem(IDC_USER)->EnableWindow(TRUE);
			GetDlgItem(IDC_WORDS)->EnableWindow(TRUE);
			GetDlgItem(IDC_TIMES)->EnableWindow(TRUE);
			GetDlgItem(IDC_WAIT)->EnableWindow(TRUE);
		}
	}
	//将发送内容复制到剪贴板
	if (state != BST_CHECKED)
		if (OpenClipboard())
		{
			char * buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, words.GetLength() + 1);
			buffer = (char*)GlobalLock(clipbuffer);
			strcpy(buffer, LPCSTR(words));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
}

// 调用时间批处理函数
void CMessage20Dlg::OnTimer(UINT_PTR nIDEvent)
{
	//查找需要的QQ/微信窗口
	hwnd = ::FindWindow(NULL, user);
	if (num == times)
	{
		KillTimer(1);
		GetDlgItem(IDC_SEND)->SetWindowText("开始发送");
		GetDlgItem(IDC_USER)->EnableWindow(TRUE);
		if (state != BST_CHECKED)
			GetDlgItem(IDC_WORDS)->EnableWindow(TRUE);
		GetDlgItem(IDC_TIMES)->EnableWindow(TRUE);
		GetDlgItem(IDC_WAIT)->EnableWindow(TRUE);
		return;
	}
	if (hwnd == NULL)
	{
		sprintf_s(name, "找不到以“ %s ”命名的窗口/n已经停止轰炸", user);
		KillTimer(1);
		MessageBox(name, "失败", MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		::PostMessage(hwnd, WM_PASTE, 0x56, 0);
		::PostMessage(hwnd, WM_KEYDOWN, 13, 0);
		num++;
	}
	CDialogEx::OnTimer(nIDEvent);
}

//图片复选框
void CMessage20Dlg::OnBnClickedPicture()
{
	state = ((CButton *)GetDlgItem(IDC_PICTURE))->GetCheck();
	if(state == BST_CHECKED)
		GetDlgItem(IDC_WORDS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_WORDS)->EnableWindow(TRUE);
}
