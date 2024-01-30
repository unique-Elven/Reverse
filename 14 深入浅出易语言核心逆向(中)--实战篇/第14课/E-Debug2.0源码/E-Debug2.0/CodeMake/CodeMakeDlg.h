
// CodeMakeDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CCodeMakeDlg 对话框
class CCodeMakeDlg : public CDialogEx
{
// 构造
public:
	CCodeMakeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CODEMAKE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void AddMessage(char *str);
	afx_msg void EnumCommand(char *szFile);
	afx_msg void MakeCommand(char *szFile);
	HWND hEdit;
	HWND hStc2;
	HWND hStc3;
	BOOL m_check;
	CProgressCtrl m_Progress;
};
