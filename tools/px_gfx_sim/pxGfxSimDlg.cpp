
// pxGfxSimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pxGfxSim.h"
#include "pxGfxSimDlg.h"
#include "afxdialogex.h"
#include "px_gfx_test.h"
#include "px_gfx_lcd_sim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CpxGfxSimDlg dialog
CpxGfxSimDlg * dlg;

extern "C" void px_gfx_lcd_sim_update(const px_gfx_area_t * area);

void px_gfx_lcd_sim_update(const px_gfx_area_t * area)
{
    TRACE("Updating area [%d, %d, %d, %d]\n",
          area->x1, area->y1, area->x2, area->y2);

    for(int y = area->y1; y <= area->y2; y++)
    {
        for(int x = area->x1; x <= area->x2; x++)
        {
            COLORREF color;
            if(px_gfx_lcd_frame_buf[y][x] == 0)
            {
                color = RGB(224,224,224);
            }
            else
            {
                color = RGB(0,0,0);
            }
            dlg->m_dcFrame.FillSolidRect(x * dlg->m_iZoom,
                                         y * dlg->m_iZoom,
                                         dlg->m_iZoom,
                                         dlg->m_iZoom,
                                         color);
        }
    }
	// Invalidate window so that it will be redrawn
	dlg->Invalidate();
}

CpxGfxSimDlg::CpxGfxSimDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PXGFXSIM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    dlg     = this;
}

void CpxGfxSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CpxGfxSimDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_MOUSEWHEEL()	
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CpxGfxSimDlg message handlers

BOOL CpxGfxSimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // Resize window to current zoom factor
	SetZoom(4);

    // Start graphics
	px_gfx_test_start();    
    
	// Start 100ms timer
	SetTimer(0, 100, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CpxGfxSimDlg::OnPaint()
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
        // Draw display
        CPaintDC dc(this);
		dc.BitBlt(0, 0, PX_GFX_DISP_SIZE_X * m_iZoom, PX_GFX_DISP_SIZE_Y * m_iZoom, &m_dcFrame, 0, 0, SRCCOPY);

		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CpxGfxSimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CpxGfxSimDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
	{
		if (m_iZoom > 2)
		{
			m_iZoom--;
			SetZoom(m_iZoom);
		}
	}
	if (zDelta < 0)
	{
		if (m_iZoom < 8)
		{
			m_iZoom++;
			SetZoom(m_iZoom);
		}
	}
	
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CpxGfxSimDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Update graphics
	px_gfx_test_update(); 

	CDialogEx::OnTimer(nIDEvent);
}

void CpxGfxSimDlg::SetZoom(int zoom)
{
    CRect    rect;
    CPaintDC dc(this);
	px_gfx_area_t area;

    if( (zoom < 1) || (zoom > 8) )
    {
        return;
    }

    m_iZoom = zoom;

	GetClientRect(&rect);
	rect.right = rect.left + PX_GFX_DISP_SIZE_X * m_iZoom;
	rect.bottom = rect.top + PX_GFX_DISP_SIZE_Y * m_iZoom;
	AdjustWindowRect(rect, GetStyle(), false);
	rect.MoveToXY(50, 50);
	MoveWindow(rect);

    if(m_bmpFrame.m_hObject != NULL)
    {
        m_bmpFrame.DeleteObject();
    }
    if(m_dcFrame.m_hDC != NULL)
    {
        m_dcFrame.DeleteDC();
    }
    m_bmpFrame.CreateCompatibleBitmap(&dc, PX_GFX_DISP_SIZE_X * m_iZoom, PX_GFX_DISP_SIZE_Y * m_iZoom);
    m_dcFrame.CreateCompatibleDC(&dc);
	m_dcFrame.SelectObject(&m_bmpFrame);

	area.x1 = PX_GFX_X_MIN;
	area.x2 = PX_GFX_X_MAX;
    area.y1 = PX_GFX_Y_MIN;
    area.y2 = PX_GFX_Y_MAX;
    px_gfx_lcd_sim_update(&area);
}
