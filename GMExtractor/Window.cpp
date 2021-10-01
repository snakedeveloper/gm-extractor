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
#include "Window.h"

HFONT CWindow::m_systemFont = NULL;
HFONT CWindow::m_captionFont = NULL;

CWindow::CWindow(): m_hwnd( NULL ), m_parent( NULL ) {
  GetDefaultFonts();
}

CWindow::~CWindow() {
  if ( m_hwnd ) {
    DestroyWindow( m_hwnd );
    m_hwnd = NULL;
  }

  m_parent = NULL;
}

void CWindow::GetDefaultFonts() {
  if ( !m_systemFont && !m_captionFont ) {
    NONCLIENTMETRICS NCMetrics = {0};
    NCMetrics.cbSize = sizeof( NONCLIENTMETRICS ) - 4;
    SystemParametersInfo( SPI_GETNONCLIENTMETRICS, 0, &NCMetrics, 0 );

    NCMetrics.lfMessageFont.lfCharSet = EASTEUROPE_CHARSET;

    m_systemFont = CreateFontIndirect( &NCMetrics.lfMessageFont );
    m_captionFont = CreateFontIndirect( &NCMetrics.lfCaptionFont );
  }
}

int CWindow::GetX() const {
  RECT winRect;
  GetRect( &winRect );
  ScreenToClient( GetParent( m_hwnd ), (LPPOINT) &winRect );

  return winRect.left;
}

int CWindow::GetY() const {
  RECT winRect;
  GetRect( &winRect );
  ScreenToClient( GetParent( m_hwnd ), (LPPOINT) &winRect );

  return winRect.top;
}

int CWindow::GetWidth() const {
  RECT winRect;
  GetRect( &winRect );

  return winRect.right - winRect.left;
}

int CWindow::GetHeight() const {
  RECT winRect;
  GetRect( &winRect );

  return winRect.bottom - winRect.top;
}

int CWindow::GetClientWidth() const {
  RECT cRect;
  GetClient( &cRect );

  return cRect.right - cRect.left;
}

int CWindow::GetClientHeight() const {
  RECT cRect;
  GetClient( &cRect );

  return cRect.bottom - cRect.top;
}

void CWindow::GetRect( LPRECT aRectPtr ) const {
  GetWindowRect( m_hwnd, aRectPtr );
}

void CWindow::GetClient( LPRECT aRectPtr ) const {
  GetClientRect( m_hwnd, aRectPtr );
}

CWindow* CWindow::GetParentWindow() const {
  return m_parent;
}

// ================================================

bool CButton::Create( CWindow* aParent ) {
  if ( !m_hwnd || aParent ) {

    m_parent = aParent;
    m_hwnd = CreateWindowEx( 0, WC_BUTTON, NULL, WS_CHILD, 0, 0,
                             0, 0, aParent->hwnd(), NULL, GetModuleHandle( NULL ), NULL );

    SendMessage( m_hwnd, WM_SETFONT, (WPARAM) CWindow::m_systemFont, true );
    return true;
  }

  return false;
}

// ================================================

bool CStaticCtrl::Create( CWindow* aParent ) {
  if ( !m_hwnd || aParent ) {

    m_parent = aParent;
    m_hwnd = CreateWindowEx( 0, WC_STATIC, NULL, WS_CHILD, 0, 0,
                             0, 0, aParent->hwnd(), NULL, GetModuleHandle( NULL ), NULL );

    SendMessage( m_hwnd, WM_SETFONT, (WPARAM) CWindow::m_systemFont, true );
    return true;
  }

  return false;
}

// ================================================

bool CCheckBox::Create( CWindow* aParent ) {
  if ( !m_hwnd || aParent ) {

    m_parent = aParent;
    m_hwnd = CreateWindowEx( WS_EX_TRANSPARENT, WC_BUTTON, NULL, 
                             WS_CHILD | BS_CHECKBOX | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX, 0, 0,
                             0, 0, aParent->hwnd(), NULL, GetModuleHandle( NULL ), NULL );

    SendMessage( m_hwnd, WM_SETFONT, (WPARAM) CWindow::m_systemFont, true );
    return true;
  }

  return false;
}

// ================================================

bool CRadioButton::Create( CWindow* aParent ) {
  if ( !m_hwnd || aParent ) {
    m_parent = aParent;
    m_hwnd = CreateWindowEx( WS_EX_TRANSPARENT, WC_BUTTON, NULL,
                            WS_CHILD | WS_TABSTOP | BS_AUTORADIOBUTTON,
                            0, 0, 0, 0, aParent->hwnd(), NULL, GetModuleHandle( NULL ), NULL );

    SendMessage( m_hwnd, WM_SETFONT, (WPARAM) CWindow::m_systemFont, true );
    return true;
  }

  return false;
}

// ================================================

bool CGroupBox::Create( CWindow* aParent ) {
  if ( !m_hwnd || aParent ) {

    m_parent = aParent;
    m_hwnd = CreateWindowEx( WS_EX_TRANSPARENT, WC_BUTTON, NULL,
                            WS_CHILD | BS_GROUPBOX, 0, 0,
                            0, 0, aParent->hwnd(), NULL, GetModuleHandle( NULL ), NULL );

    SetWindowLong( m_hwnd, GWL_USERDATA, 
                   SetWindowLong( m_hwnd, GWL_WNDPROC, (LONG) CommandForwarding ) );

    SendMessage( m_hwnd, WM_SETFONT, (WPARAM) CWindow::m_systemFont, true );
    return true;
  }

  return false;
}

LRESULT CALLBACK CGroupBox::CommandForwarding( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
  switch ( msg ) {
    case WM_SETFOCUS:
    case WM_CTLCOLORSTATIC:
    case WM_COMMAND:
      return SendMessage( GetParent( hwnd ), msg, wParam, lParam );
  }

  return CallWindowProc( (WNDPROC) GetWindowLong( hwnd, GWL_USERDATA ), 
                         hwnd, msg, wParam, lParam );
}

// ================================================

bool CEdit::Create( CWindow* aParent, DWORD aStyle ) {
  if ( !m_hwnd || aParent ) {

    m_parent = aParent;
    m_hwnd = CreateWindowEx( 0, WC_EDIT, NULL, aStyle, 0, 0, 0,
                            0, aParent->hwnd(), NULL, GetModuleHandle( NULL ), NULL );

    SendMessage( m_hwnd, WM_SETFONT, (WPARAM) CWindow::m_systemFont, true );
    return true;
  }

  return false;
}

// ================================================

bool CODListBox::Create( CWindow* aParent, DWORD aStyle ) {
  if ( !m_hwnd || aParent ) {

    m_parent = aParent;
    m_hwnd = CreateWindowEx( 0, WC_LISTBOX, NULL, aStyle | LBS_OWNERDRAWFIXED, 0, 0, 0,
                             0, aParent->hwnd(), NULL, GetModuleHandle( NULL ), NULL );

    SendMessage( m_hwnd, WM_SETFONT, (WPARAM) CWindow::m_systemFont, true );
    return true;
  }

  return false;
}
