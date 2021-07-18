
// vstoolkitsDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "vstoolkits.h"
#include "vstoolkitsDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GetProcessList();

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CvstoolkitsDlg dialog



CvstoolkitsDlg::CvstoolkitsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VSTOOLKITS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvstoolkitsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CvstoolkitsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(btnSelectProcess, &CvstoolkitsDlg::OnBnClickedbtnselectprocess)
	ON_BN_CLICKED(btnSelectDll, &CvstoolkitsDlg::OnBnClickedbtnselectdll)
	ON_BN_CLICKED(btnInject, &CvstoolkitsDlg::OnBnClickedbtninject)
	ON_BN_CLICKED(btnDispatch, &CvstoolkitsDlg::OnBnClickedbtndispatch)
//	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CvstoolkitsDlg message handlers

BOOL CvstoolkitsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// �̶����ڴ�С
	SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
	// �����ļ��Ϸ�
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);       // 0x0049 == WM_COPYGLOBALDATA

	GetAppList((CComboBox*)GetDlgItem(cbxSelectProcess));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CvstoolkitsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CvstoolkitsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CvstoolkitsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CvstoolkitsDlg::OnBnClickedbtnselectprocess()
{
	GetAppList((CComboBox*)GetDlgItem(cbxSelectProcess));
	// TODO: Add your control notification handler code here
}


void CvstoolkitsDlg::OnBnClickedbtnselectdll()
{
	WCHAR text[MAX_PATH];                     // �����ں���ִ����Ϻ���Զ����� 

	OPENFILENAME opfn;  // 
	ZeroMemory(&opfn, sizeof(OPENFILENAME));  // �ṹ���ʼ��
	opfn.lStructSize = sizeof(OPENFILENAME);
	opfn.lpstrFilter = L"��̬���ӿ�\0*.dll";   // �ļ�������
	opfn.nFilterIndex = 0;                    // Ĭ�Ϲ���������  
	opfn.lpstrFile = text;                    // �ļ����ĵ�һ���ַ���������Ϊ0
	opfn.lpstrFile[0] = '\0';                 // �²�ԭ����Ϊ���ظ�ʹ��ʱ����bug
	opfn.nMaxFile = sizeof(text);             // �ļ����������ĳ���
	opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;  // �ļ�����·���������
	if (GetOpenFileName(&opfn)) {
		CEdit* editor = (CEdit*)GetDlgItem(txtSelectDll);
		editor->SetWindowText(text);
	}

	// TODO: Add your control notification handler code here
}


void CvstoolkitsDlg::OnBnClickedbtninject()
{
	CComboBox* cmx = (CComboBox*)GetDlgItem(cbxSelectProcess);
	CEdit* editor = (CEdit*)GetDlgItem(txtSelectDll);
	LoadLibraryOrNot(cmx,editor, FALSE);

	/// TODO: Add your control notification handler code here
}


void CvstoolkitsDlg::OnBnClickedbtndispatch()
{
	CComboBox* cmx = (CComboBox*)GetDlgItem(cbxSelectProcess);
	CEdit* editor = (CEdit*)GetDlgItem(txtSelectDll);
	LoadLibraryOrNot(cmx, editor, TRUE);
	// TODO: Add your control notification handler code here
}

