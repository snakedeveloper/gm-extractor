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

#include "Window.h"
#include "GMExtractorHandler.h"

struct LOGDATA {
  std::string logText;
  bool isLabel;
};

enum APP_STATUS {
  AS_WAIT_FOR_FILE,
  AS_READY,
  AS_INVALID_DIRECTORY,
  AS_FILE_NOT_EXISTS,
  AS_READ_ERROR,
  AS_FILE_CORRUPTED,
  AS_INCOMPATIBLE_VERSION,
  AS_EXTRACTING,
  AS_FINISHED
};

class CButtonOpen: public CButton {
  public:
    bool Create( CWindow* aParent );
    void OnClick();

  private:
    static const char* m_text;
    static const char* m_filterText;
};

class CButtonDir: public CButton {
  public:
    bool Create( CWindow* aParent );
    void OnClick();

  private:
    static const char* m_text;
    static const char* m_dirDialogText;
};

class CButtonExtract: public CButton {
  public:
    bool Create( CWindow* aParent );
    void OnClick();

  private:
    static const char* m_text;
};

class CEditFile: public CEdit {
  public:
    bool Create( CWindow* aParent );

    void Rearrange( LPRECT aNewRect );

  private:
    static const char* m_text;
};

class CEditDir: public CEdit {
  public:
    bool Create( CWindow* aParent );

    void Rearrange( LPRECT aNewRect );

  private:
    static const char* m_text;
};

class CStaticStatus: public CStaticCtrl {
  public:
    bool Create( CWindow* aParent );

    void SetStatus( APP_STATUS aStatus ) {
      SetText( m_texts[aStatus] );
    }

    void Rearrange( LPRECT aNewRect );

  private:
    static const char* m_texts[];
};

class CListBoxLog: public CODListBox {
  public:
    bool Create( CWindow* aParent );
    void Destroy();

    void AddItem( const char* aItemText, bool aIsLabel );
    void Clear();

    void OnMeasureItem( LPMEASUREITEMSTRUCT aMeasureItemStruct );
    void OnDrawItem( LPDRAWITEMSTRUCT aDrawItemStruct );
    void Rearrange( LPRECT aNewRect );

  private:
    HFONT m_font;
    std::vector<LOGDATA> m_items;
};

class CMainWindow: public CWindow {
  public:
    bool Create( CWindow* aParent );
    void Destroy();

    static LRESULT CALLBACK WndProc( HWND aHwnd, UINT aMsg, WPARAM aWparam, LPARAM aLparam );

    CButtonOpen buttonFile;
    CButtonDir buttonDir;
    CButtonExtract buttonExtract;
    CEditFile editFile;
    CEditDir editDir;
    CStaticStatus staticStatus;
    CListBoxLog listboxLog;

    CExtractTask extractTask;

  private:
    bool RegisterWindowClass();

    static const char* m_className;
    static const char* m_windowName;
};
