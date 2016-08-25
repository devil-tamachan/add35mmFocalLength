/*
licenced by New BSD License

Copyright (c) 2014, devil.tamachan@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#define ADD35MMFOCAL_NOMAIN

extern "C" {
#include "../add35mmFocalLength.c"
}


class CMainDlg : public CDialogImpl<CMainDlg>, public CDialogResize<CMainDlg>, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

  CListViewCtrl m_list_files;
    sqlite3 *pDb;
    char *errMsg;
    sqlite3_stmt *pSearchDiagonalStmt;

  CMainDlg()
  {
    pDb = NULL;
    errMsg = NULL;
    pSearchDiagonalStmt = NULL;
  }
  virtual ~CMainDlg()
  {
    FreeSqlite();
  }

  void SetStatusText(int idx, LPCTSTR status)
  {
    m_list_files.SetItemText(idx, 1, status);
    m_list_files.UpdateWindow();
  }
  void SetRunning(int idx)
  {
    SetStatusText(idx, _T("-"));
  }
  void SetSuccess(int idx)
  {
    SetStatusText(idx, _T("OK"));
  }
  void SetFail(int idx)
  {
    SetStatusText(idx, _T("!"));
  }


  virtual BOOL OnIdle()
  {
    
    //GetExitCodeProcess
    return FALSE;
  }

  BEGIN_DLGRESIZE_MAP(CMainDlg)
    DLGRESIZE_CONTROL(IDL_FILES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    DLGRESIZE_CONTROL(IDB_CLEAR, DLSZ_MOVE_X)
  END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    MSG_WM_SIZE(OnSize)
    MSG_WM_CLOSE(OnClose)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_DROPFILES(OnDropFiles)
		//COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		//COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    CHAIN_MSG_MAP(CDialogResize<CMainDlg>)
	END_MSG_MAP()

  void OnDestroy()
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveIdleHandler(this);
    SetMsgHandled(FALSE);
  }

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

    m_list_files = GetDlgItem(IDL_FILES);
    m_list_files.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_list_files.InsertColumn(0, _T("Path"), LVCFMT_LEFT, 250);
    m_list_files.InsertColumn(1, _T("Result"), LVCFMT_LEFT, 50);

    DragAcceptFiles();

    DlgResize_Init(true, true, WS_THICKFRAME | WS_MAXIMIZEBOX | WS_CLIPCHILDREN);

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->AddIdleHandler(this);

    
    int numArgs = 0;
    LPWSTR *lpArgv = CommandLineToArgvW(GetCommandLineW(), &numArgs);
    for(int i=1; i<numArgs; i++)AddPath(lpArgv[i]);

		return TRUE;
	}

  void OnSize(UINT nType, CSize size)
  {
    int newWidth = size.cx - 80;
    m_list_files.SetColumnWidth(0, newWidth);
    SetMsgHandled(FALSE);
  }

  void AddDir(LPCTSTR _dirpath)
  {
    CFindFile finder;

    CString dirpath(_dirpath);
    PathAddBackslash(dirpath.GetBuffer(dirpath.GetLength()+2));
    dirpath.ReleaseBuffer();
    dirpath += _T("*");

    BOOL bWorking = finder.FindFile(dirpath);

    while (bWorking)
    {
      bWorking = finder.FindNextFile();
      CString file = finder.GetFilePath();
      if (finder.IsDots())continue;
      else if (finder.IsDirectory())AddDir(file);
      else if(IsJpgExt(file))AddJpg(file);
    }
   finder.Close();
  }
  

  
HRESULT LoadFromFile(LPCTSTR path)
{
  int rc = procImage(CT2A(path), pDb, pSearchDiagonalStmt);

  return (rc==1)?S_OK:E_FAIL;
}



  void AddJpg(LPCTSTR path)
  {
    m_list_files.InsertItem(0, path);
    if(SUCCEEDED(LoadFromFile(path)))SetSuccess(0);
    else SetFail(0);
  }

  BOOL IsJpgExt(LPCTSTR path)
  {
    CString ext = PathFindExtension(path);
    if(ext.CompareNoCase(_T(".jpg"))==0)return TRUE;
    return FALSE;
  }
  
  void AddPath(LPCTSTR path)
  {
    if(PathIsDirectory(path))AddDir(path);
    else if(PathFileExists(path))
    {
      if(IsJpgExt(path))AddJpg(path);
    }
  }
  
  void InitSqlite()
  {
    static const char *zFilename = "exif.sqlite";
    int rc = 0;
    if(pDb==NULL)
    {
      rc = sqlite3_open(zFilename, &pDb);
      if(rc != SQLITE_OK)
      {
        printf("%s\n", errMsg);
        sqlite3_free(errMsg);errMsg = NULL;
        return;
      }
    }
    
    if(pSearchDiagonalStmt==NULL)
    {
      rc = sqlite3_prepare_v2(pDb, "SELECT models.diagonal, models.sensorwidth, models.id from models INNER JOIN makers ON models.makerid = makers.id WHERE makers.name = ?1 AND models.name = ?2 LIMIT 1;", -1, &pSearchDiagonalStmt, NULL);
      if(rc != SQLITE_OK)
      {
        sqlite3_finalize(pSearchDiagonalStmt);
        return;
      }
    }
  }
  
  void FreeSqlite()
  {
    if(pSearchDiagonalStmt)sqlite3_finalize(pSearchDiagonalStmt);
    if(pDb)sqlite3_close(pDb);
  }

  void OnDropFiles(HDROP hDropInfo)
  {
    InitSqlite();
    unsigned int numFile = ::DragQueryFile(hDropInfo, -1, NULL, 0);
    for(unsigned int i=0; i<numFile; i++)
    {
      CString path;
      ::DragQueryFile(hDropInfo, i, path.GetBuffer(1024+2), 1024);
      path.ReleaseBuffer();
      AddPath(path);
    }
    ::DragFinish(hDropInfo);
  }

  
	//LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//{
//		CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
//		dlg.DoModal();
//		return 0;
//	}
//
  void OnClose()
  {
    DestroyWindow();
    ::PostQuitMessage(0);
  }
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
    DestroyWindow();
    ::PostQuitMessage(wID);
		return 0;
	}

//	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	{
//		EndDialog(wID);
//		return 0;
//	}
};
