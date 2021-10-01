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
#include "GMEApplication.h"

CApplication application;

bool CApplication::Create() {
  INITCOMMONCONTROLSEX iccex;
  iccex.dwSize = sizeof( INITCOMMONCONTROLSEX );
  iccex.dwICC = ICC_STANDARD_CLASSES;

  if ( !InitCommonControlsEx( &iccex ) )
    InitCommonControls();

  mainWindow.Create( NULL );
  mainWindow.SetSize( 440, 400 );

  int centerX = GetSystemMetrics( SM_CXSCREEN ) / 2 - mainWindow.GetWidth() / 2,
      centerY = GetSystemMetrics( SM_CYSCREEN ) / 2 - mainWindow.GetHeight() / 2;

  mainWindow.SetPosition( centerX, centerY );
  mainWindow.Visible( true );

  return true;
}

void CApplication::Destroy() {
  mainWindow.Destroy();
}

int CApplication::Run() {
  MSG msg = {0};

  while ( GetMessage( &msg, mainWindow.hwnd(), 0, 0 ) > 0 ) {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }

  return (int) msg.wParam;
}