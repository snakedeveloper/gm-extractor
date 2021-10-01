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

#pragma once

class CWindow {
  public:
    CWindow();
    virtual ~CWindow();

    virtual bool Create( CWindow* aParent ) { 
      return true;
    }

    void Destroy() { 
      CWindow::~CWindow(); 
    }

    HWND hwnd() const {
      return m_hwnd;
    }

    void Enabled( bool aEnabled ) const { 
      EnableWindow( m_hwnd, aEnabled );
    }

    void Visible( bool aVisible ) const {
      ShowWindow( m_hwnd, aVisible ? SW_SHOW : SW_HIDE ); 
    }

    bool IsVisible() const { 
      return IsWindowVisible( m_hwnd ) & 1;
    }
    
    void SetX( int aX ) const {
      SetWindowPos( m_hwnd, NULL, aX, GetY(), 0, 0, SWP_NOSIZE | SWP_NOZORDER );
    }

    void SetY( int aY ) const {
      SetWindowPos( m_hwnd, NULL, GetX(), aY, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
    }

    void SetWidth( int aWidth ) const {
      SetWindowPos( m_hwnd, NULL, 0, 0, aWidth, GetHeight(), SWP_NOMOVE | SWP_NOZORDER );
    }

    void SetHeight( int aHeight ) const { 
      SetWindowPos( m_hwnd, NULL, 0, 0, GetWidth(), aHeight, SWP_NOMOVE | SWP_NOZORDER );
    }

    void SetPosition( int aX, int aY ) const { 
      SetWindowPos( m_hwnd, NULL, aX, aY, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
    }

    void SetSize( int aWidth, int aHeight ) const {
      SetWindowPos( m_hwnd, NULL, 0, 0, aWidth, aHeight, SWP_NOMOVE | SWP_NOZORDER ); 
    }

    void SetWindow( int aX, int aY, int aWidth, int aHeight ) const {
      SetWindowPos( m_hwnd, NULL, aX, aY, aWidth, aHeight, SWP_NOZORDER );
    }

    int GetX() const;
    int GetY() const;

    int GetClientWidth() const;
    int GetClientHeight() const;

    int GetWidth() const;
    int GetHeight() const;

    void GetRect( LPRECT aRectPtr ) const;
    void GetClient( LPRECT aRectPtr ) const; 

    void AddStyle( DWORD aStyle ) const { 
      SetWindowLong( m_hwnd, GWL_STYLE, GetStyle() | aStyle );
      SetWindowPos( m_hwnd, NULL, 0, 0, 0, 0, 
                    SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE );
    }

    void AddExStyle( DWORD aExStyle ) const {
      SetWindowLong( m_hwnd, GWL_EXSTYLE, GetExStyle() | aExStyle ); RedrawFrame();
    }

    void RemoveStyle( DWORD aStyle ) const {
      SetWindowLong( m_hwnd, GWL_STYLE, GetStyle() & ~aStyle ); RedrawFrame(); 
    }

    void RemoveExStyle( DWORD aExStyle ) const {
      SetWindowLong( m_hwnd, GWL_EXSTYLE, GetExStyle() & ~aExStyle ); RedrawFrame(); 
    }

    void SetStyle( DWORD aStyle ) const {
      SetWindowLong( m_hwnd, GWL_STYLE, aStyle ); RedrawFrame(); 
    }

    void SetExStyle( DWORD aExStyle ) const {
      SetWindowLong( m_hwnd, GWL_EXSTYLE, aExStyle ); RedrawFrame();
    }

    DWORD GetStyle() const {
      return GetWindowLong( m_hwnd, GWL_STYLE );
    }

    DWORD GetExStyle() const {
      return GetWindowLong( m_hwnd, GWL_EXSTYLE );
    }

    bool IsStyleSet( DWORD aStyle ) const {
      return ( (GetStyle() & aStyle) != 0 );
    }

    bool IsExStyleSet( DWORD aExStyle ) const {
      return ( (GetExStyle() & aExStyle) != 0 );
    };

    void SetText( const char* aText ) const { 
      SetWindowText( m_hwnd, aText ); 
    }

    void GetText( char* aBuffer, DWORD aBufferSize ) const {
      GetWindowText( m_hwnd, aBuffer, aBufferSize );
    }

    void SetID( int aId ) { 
      SetWindowLong( m_hwnd, GWL_ID, aId );
    }

    int GetID() { 
      return GetWindowLong( m_hwnd, GWL_ID );
    }

    CWindow* GetParentWindow() const;

    void RedrawFrame() const {
      SetWindowPos( m_hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE ); 
    }

    void Invalidate() const { 
      InvalidateRect( m_hwnd, NULL, true );
    }

  protected:
    static HFONT m_systemFont;
    static HFONT m_captionFont;

    HWND m_hwnd;
    CWindow* m_parent;

  private:
    void GetDefaultFonts();
};

class CStaticCtrl: public CWindow {
  public:
    bool Create( CWindow* aParent );
};

class CButton: public CWindow {
  public:
    virtual bool Create( CWindow* aParent );
    virtual void OnClick() {};
};

class CCheckBox: public CWindow {
  public:
    virtual bool Create( CWindow* aParent );
    virtual void OnClick() {};

    void Check( bool aCheck ) {
      CheckDlgButton( m_parent->hwnd(), GetID(), (int) aCheck );
    }

    bool Checked() {
      return ( IsDlgButtonChecked( m_parent->hwnd(), GetID() ) == BST_CHECKED ); 
    }
};

class CRadioButton: public CCheckBox {
  public:
    virtual bool Create( CWindow* aParent );
};

class CGroupBox: public CWindow {
  public:
    virtual bool Create( CWindow* aParent );
    static LRESULT CALLBACK CommandForwarding( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
};

class CEdit: public CWindow {
  public:
    virtual bool Create( CWindow* aParent, DWORD aStyle );
    void ReadOnly( bool aEnable ) {
      SendMessage( m_hwnd, EM_SETREADONLY, aEnable, 0 );
    }
};

class CODListBox: public CWindow {
  public:
    virtual bool Create( CWindow* aParent, DWORD aStyle );
    virtual void OnMeasureItem( LPMEASUREITEMSTRUCT aMeasureItemStruct ) {}
    virtual void OnDrawItem( LPDRAWITEMSTRUCT aDrawItemStruct ) {}
};
