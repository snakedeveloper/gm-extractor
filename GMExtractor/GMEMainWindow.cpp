/*
    Copyright 2008, 2009 (C) Snake [http://sgames.ovh.org/] 

    GM Extractor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GM Extractor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GM Extractor.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PreHead.h"
#include "GMEMainWindow.h"

const int ID_BTN_OPEN = 100;
const int ID_BTN_DIR = 101;
const int ID_BTN_EXTRACT = 102;
const int ID_EDIT_FILE = 103;
const int ID_EDIT_DIR = 104;
const int ID_LOG = 104;

const char* CMainWindow::m_className = "GME_mainFrame";
const char* CMainWindow::m_windowName = "GM Extractor v2.0 | by Snake (http://sgames.ovh.org/)";

const char* CButtonOpen::m_text = "Open";
const char* CButtonOpen::m_filterText = "Game Maker 5.3a/6.0/6.1/7.0 project file (*.gmd, *.gm6, *.gmk)\0*.gmd; *.gm6; *.gmk;\0\0";
const char* CButtonDir::m_text = "Select";
const char* CButtonDir::m_dirDialogText = "Please select a directory where You want to extract the resources.";
const char* CButtonExtract::m_text = "Extract!";
const char* CEditFile::m_text = "GM project from which extract the resources";
const char* CEditDir::m_text = "Directory where the resources will be saved";

const char* CStaticStatus::m_texts[] = {
  "Please open Game Maker project file to extract its resources.",
  "Press \"Extract\" button to begin.",
  "Invalid directory.",
  "File does not exists.",
  "Cannot read the file.",
  "File is corrupted - extracting cancelled.",
  "Incompatible game project version.",
  "Extracting...",
  "Task finished successfully."
};

bool CMainWindow::Create( CWindow* aParent ) {
  if ( !m_hwnd )
    if ( RegisterWindowClass() ) {
      m_hwnd = CreateWindowEx( 0, m_className, m_windowName, 
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                               0, 0, 0, 0, NULL, NULL, GetModuleHandle( NULL ), this );

      buttonFile.Create( this );
      buttonDir.Create( this );
      buttonExtract.Create( this );
      editFile.Create( this );
      editDir.Create( this );
      staticStatus.Create( this );
      listboxLog.Create( this );

      staticStatus.SetStatus( AS_WAIT_FOR_FILE );
      return true;
    }

  return false;
}

void CMainWindow::Destroy() {
  buttonFile.Destroy();
  buttonDir.Destroy();
  buttonExtract.Destroy();
  editFile.Destroy();
  editDir.Destroy();
  staticStatus.Destroy();
  listboxLog.Destroy();

  UnregisterClass( m_className, GetModuleHandle( NULL ) );
  CWindow::Destroy();
}

bool CMainWindow::RegisterWindowClass() {
  WNDCLASSEX wndClass = {0};
  wndClass.cbSize = sizeof( WNDCLASSEX );
  wndClass.hInstance = GetModuleHandle( NULL );
  wndClass.hbrBackground = (HBRUSH) GetSysColorBrush( COLOR_BTNFACE );
  wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
  wndClass.hIcon = LoadIcon( GetModuleHandle( NULL ), "MAINICON" );
  wndClass.lpszClassName = m_className;
  wndClass.lpfnWndProc = WndProc;
  
  return ( RegisterClassEx( &wndClass ) != 0 );
}

LRESULT CALLBACK CMainWindow::WndProc( HWND aHwnd, UINT aMsg, WPARAM aWparam, LPARAM aLparam ) {
  CMainWindow* self = (CMainWindow*) GetWindowLong( aHwnd, GWL_USERDATA );

  switch ( aMsg ) {
    case WM_CREATE: {
      LPCREATESTRUCT createStruct = (LPCREATESTRUCT) aLparam;
      SetWindowLong( aHwnd, GWL_USERDATA, (LONG) createStruct->lpCreateParams );
      break;
    }

    case WM_COMMAND:
      switch ( aWparam ) {
        case ID_BTN_OPEN:
          self->buttonFile.OnClick();
          break;

        case ID_BTN_DIR:
          self->buttonDir.OnClick();
          break;

        case ID_BTN_EXTRACT:
          self->buttonExtract.OnClick();
      }
      break;

    case WM_MEASUREITEM:
      if ( aWparam == ID_LOG )
        self->listboxLog.OnMeasureItem( (LPMEASUREITEMSTRUCT) aLparam );
      break;

    case WM_DRAWITEM:
      if ( aWparam == ID_LOG )
        self->listboxLog.OnDrawItem( (LPDRAWITEMSTRUCT) aLparam );
      break;

    case WM_CTLCOLORSTATIC:
      if ( (HWND) aLparam == self->editDir.hwnd() || (HWND) aLparam == self->editFile.hwnd() ) {
        COLORREF bgColor = SetBkColor( (HDC) aWparam, 0 );
        SetBkColor( (HDC) aWparam, bgColor );

        SetTextColor( (HDC) aWparam, GetSysColor( COLOR_GRAYTEXT ) );
        return bgColor;
      } else
        return DefWindowProc( aHwnd, aMsg, aWparam, aLparam );

    case WM_SIZE: {
      RECT wndRect;
      self->GetRect( &wndRect );

      self->editDir.Rearrange( &wndRect );
      self->editFile.Rearrange( &wndRect );
      self->staticStatus.Rearrange( &wndRect );
      self->listboxLog.Rearrange( &wndRect );
      break;
    }

    case WM_SIZING: {
      LPRECT wndRect = (LPRECT) aLparam;

      if ( wndRect->bottom - wndRect->top < 400 )
        wndRect->bottom = wndRect->top + 400;

      if ( wndRect->right - wndRect->left < 400 )
        wndRect->right = wndRect->left + 400;

      self->editDir.Rearrange( wndRect );
      self->editFile.Rearrange( wndRect );
      self->staticStatus.Rearrange( wndRect );
      self->listboxLog.Rearrange( wndRect );

      return TRUE;
    }

    default: return DefWindowProc( aHwnd, aMsg, aWparam, aLparam );
  }

  return 0;
}

// ==============================================

bool CButtonOpen::Create( CWindow *aParent ) {
  if ( CButton::Create( aParent ) ) {
    SetID( ID_BTN_OPEN );
    SetText( m_text );
    SetWindow( 10, 10, 80, 22 );
    Visible( true );

    return true;
  }

  return false;
}

void CButtonOpen::OnClick() {
  CMainWindow* mainWnd = (CMainWindow*) GetParentWindow();

  char fileName[MAX_PATH] = {0};
  OPENFILENAME ofn = {0};

  ofn.lStructSize = sizeof( OPENFILENAME );
  ofn.hInstance = GetModuleHandle( NULL );
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFilter = m_filterText;
  ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
              OFN_PATHMUSTEXIST;

  if ( GetOpenFileName( &ofn ) ) {
    std::string dir( fileName );
    dir.erase( dir.find_last_of( '.' ) );

    mainWnd->editDir.SetText( dir.c_str() );
    mainWnd->editFile.SetText( fileName );
    mainWnd->buttonExtract.Enabled( true );
    mainWnd->staticStatus.SetStatus( AS_READY );
  }
}

bool CButtonDir::Create( CWindow *aParent ) {
  if ( CButton::Create( aParent ) ) {
    SetID( ID_BTN_DIR );
    SetText( m_text );
    SetWindow( 10, 35, 80, 22 );
    Visible( true );

    return true;
  }

  return false;
}

void CButtonDir::OnClick() {
  CMainWindow* mainWnd = (CMainWindow*) GetParentWindow();

  BROWSEINFO bi = {0};
  bi.hwndOwner = mainWnd->hwnd();
  bi.lpszTitle = m_dirDialogText;
  bi.ulFlags = BIF_RETURNONLYFSDIRS;

  LPITEMIDLIST iil = SHBrowseForFolder( &bi );
  if ( iil ) {
    char dirPath[MAX_PATH];
    SHGetPathFromIDList( iil, dirPath );
    mainWnd->editDir.SetText( dirPath );

    IMalloc* im = NULL;
    if ( SHGetMalloc( &im ) >= 0 ) {
      im->Free( iil );
      im->Release();
    }
  }
}

bool CButtonExtract::Create( CWindow *aParent ) {
  if ( CButton::Create( aParent ) ) {
    SetID( ID_BTN_EXTRACT );
    SetText( m_text );
    SetWindow( 10, 63, 80, 22 );
    Enabled( false );
    Visible( true );

    return true;
  }

  return false;
}

void CButtonExtract::OnClick() {
  CMainWindow* mainWnd = (CMainWindow*) GetParentWindow();

  char filePath[MAX_PATH] = {0};
  char dirPath[MAX_PATH] = {0};

  mainWnd->editFile.GetText( filePath, MAX_PATH );
  mainWnd->editDir.GetText( dirPath, MAX_PATH );

  CreateDirectory( dirPath, NULL );

  if ( !PathIsDirectory( dirPath ) ) {
    mainWnd->staticStatus.SetStatus( AS_INVALID_DIRECTORY );
    return;
  }

  if ( GetFileAttributes( filePath ) == INVALID_FILE_ATTRIBUTES ) {
    mainWnd->staticStatus.SetStatus( AS_FILE_NOT_EXISTS );
    return;
  }

  mainWnd->buttonExtract.Enabled( false );
  mainWnd->buttonDir.Enabled( false );
  mainWnd->buttonFile.Enabled( false );

  mainWnd->listboxLog.Clear();
  mainWnd->staticStatus.SetStatus( AS_EXTRACTING );

  mainWnd->extractTask.Reset( filePath, dirPath );
  mainWnd->extractTask.Execute();
}

bool CEditFile::Create( CWindow *aParent ) {
  if ( CEdit::Create( aParent, ES_READONLY | ES_AUTOHSCROLL | WS_CHILD ) ) {
    AddExStyle( WS_EX_CLIENTEDGE );
    SetWindow( 95, 11, 290, 20 );
    SetText( m_text );
    Visible( true );

    return true;
  }

  return false;
}

void CEditFile::Rearrange( LPRECT aNewRect ) {
  SetWidth( aNewRect->right - aNewRect->left - 10 - GetX() - (GetSystemMetrics( SM_CXFRAME ) * 2) );
}

bool CEditDir::Create( CWindow *aParent ) {
  if ( CEdit::Create( aParent, ES_READONLY | ES_AUTOHSCROLL | WS_CHILD ) ) {
    AddExStyle( WS_EX_CLIENTEDGE );
    SetWindow( 95, 36, 290, 20 );
    SetText( m_text );
    Visible( true );

    return true;
  }

  return false;
}

void CEditDir::Rearrange( LPRECT aNewRect ) {
  SetWidth( aNewRect->right - aNewRect->left - 10 - GetX() - (GetSystemMetrics( SM_CXFRAME ) * 2) );
}

bool CStaticStatus::Create( CWindow* aParent ) {
  if ( CStaticCtrl::Create( aParent ) ) {
    SetWindow( 95, 60, 290, 25 );
    Visible( true );

    return true;
  }

  return false;
}

void CStaticStatus::Rearrange( LPRECT aNewRect ) {
  SetWidth( aNewRect->right - aNewRect->left - 10 - GetX() - (GetSystemMetrics( SM_CXFRAME ) * 2) );
  Invalidate();
}

bool CListBoxLog::Create( CWindow* aParent ) {
  if ( CODListBox::Create( aParent, WS_VSCROLL | WS_CHILD | LBS_NOINTEGRALHEIGHT ) ) {
    AddExStyle( WS_EX_CLIENTEDGE );
    SetWindow( 10, 90, 375, 280 );
    SetID( ID_LOG );

    HDC hdc = GetDC( m_hwnd );
    
    LOGFONT lf = {0};
    strcpy_s( lf.lfFaceName, 32, "Terminal" );
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -MulDiv( 6, GetDeviceCaps( hdc, LOGPIXELSY ), 72 );
    lf.lfWeight = FW_NORMAL;

    m_font = CreateFontIndirect( &lf );

    SendMessage( m_hwnd, WM_SETFONT, (WPARAM) m_font, true );
    ReleaseDC( m_hwnd, hdc );

    Visible( true );
    return true;
  }

  return false;
}

void CListBoxLog::Destroy() {
  DeleteObject( m_font );
}

void CListBoxLog::Rearrange( LPRECT aNewRect ) {
  SetSize( (aNewRect->right - aNewRect->left - GetX()) - 10 - (GetSystemMetrics( SM_CXFRAME ) * 2),
           ((aNewRect->bottom - aNewRect->top - GetY()) - 10 - GetSystemMetrics( SM_CYCAPTION ) -
           (GetSystemMetrics( SM_CYFRAME ) * 2)) );
}

void CListBoxLog::AddItem( const char* aItemText, bool aIsLabel ) {
  LOGDATA data = { aItemText, aIsLabel };
  m_items.push_back( data );

  SendMessage( m_hwnd, WM_SETREDRAW, FALSE, 0 );

  SendMessage( m_hwnd, LB_ADDSTRING, NULL, m_items.size() - 1 );
  SendMessage( m_hwnd, LB_SETCURSEL, m_items.size() - 1, 0 );

  SendMessage( m_hwnd, WM_SETREDRAW, TRUE, 0 );
}

void CListBoxLog::Clear() {
  SendMessage( m_hwnd, LB_RESETCONTENT, 0, 0 );
  m_items.clear();
}

void CListBoxLog::OnMeasureItem( LPMEASUREITEMSTRUCT aMeasureItemStruct ) {
  aMeasureItemStruct->itemHeight = 10;
}

void CListBoxLog::OnDrawItem( LPDRAWITEMSTRUCT aDrawItemStruct ) {
  int item = aDrawItemStruct->itemID;
  HDC hdc = aDrawItemStruct->hDC;

  if ( item >= 0 && item < (int) m_items.size() ) {
    SetTextColor( hdc, 0 );
    SetBkColor( hdc, ( m_items[item].isLabel ? 0xDDDDDD : 0xFFFFFF ) );
    SetDCBrushColor( hdc, ( m_items[item].isLabel ? 0xDDDDDD : 0xFFFFFF ) );

    FillRect( hdc, &aDrawItemStruct->rcItem, (HBRUSH) GetStockObject( DC_BRUSH ) );

    aDrawItemStruct->rcItem.left += ( m_items[item].isLabel ? 2 : 12 );

    DrawText( hdc, m_items[item].logText.c_str(), -1, &aDrawItemStruct->rcItem,
              DT_SINGLELINE | DT_VCENTER );
  }
}
