
// vstoolkitsDlg.h : header file
//

#pragma once


// CvstoolkitsDlg dialog
class CvstoolkitsDlg : public CDialogEx
{
// Construction
public:
	CvstoolkitsDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VSTOOLKITS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedbtnselectprocess();
	afx_msg void OnBnClickedbtnselectdll();
	afx_msg void OnBnClickedbtninject();
	afx_msg void OnBnClickedbtndispatch();
//	afx_msg void OnDropFiles(HDROP hDropInfo);
};
