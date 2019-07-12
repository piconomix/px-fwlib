
// pxGfxSimDlg.h : header file
//

#pragma once


// CpxGfxSimDlg dialog
class CpxGfxSimDlg : public CDialogEx
{
// Construction
public:
	CpxGfxSimDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PXGFXSIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    void SetZoom(int zoom);    

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int     m_iZoom;
    CDC     m_dcFrame;
    CBitmap m_bmpFrame;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
