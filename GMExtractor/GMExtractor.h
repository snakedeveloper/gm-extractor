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

enum GMEEvent {
  GMEE_START,
  GMEE_END,
  GMEE_NEW_TASK,
  GMEE_EXTRACTION_VERIFY,
  GMEE_RESOURCE_EXTRACTED
};

enum GMEError {
  GMEERR_SUCCESS,
  GMEERR_INCOMPATIBLE_VERSION,
  GMEERR_NO_ACCESS,
  GMEERR_FILE_CORRUPTED
};

enum GMEExtractionTask {
  GMET_VARIOUS,
  GMET_SPRITES,
  GMET_BACKGROUNDS,
  GMET_SOUNDS
};

typedef bool (*GME_EVENT)( GMEEvent, void*, void* );

class CGMExtractor {
  public:
    CGMExtractor(): m_event( NULL ), m_fileCount( 0 ) {}

    GMEError Open( const char* aProjectFile );
    void SetDirectory( const char* aDirectoryPath ) {
      m_dirPath = aDirectoryPath;
    }

    void Run();
    void Close();

    void SetEventProcedure( GME_EVENT aEventProc ) {
      m_event = aEventProc;
    }

  private:
    bool CallEvent( GMEEvent aEventType, void* aParam1, void* aParam2 ) {
      if ( m_event )
        return m_event( aEventType, aParam1, aParam2 );

      return false;
    }

    BYTE* CreateSwapTable( int aSeed );
    void DestroySwapTable( BYTE* aSwapTable );
    bool DecryptProject();

    GMEError ProcessResource( bool aIsCompressed, const char* aOutputFileName );
    GMEError ProcessConditionalResource( bool aIsCompressed, const char* aOutputFileName );
    GMEError SaveResource( BYTE* aResourceData, DWORD aResourceSize, bool aIsCompressed, 
                           const char* aFileName );

    std::string GetVariableLengthString();
    void SkipVariableLengthData();

    std::ifstream m_input;
    std::strstream m_buffer;
    std::string m_filePath, m_dirPath;

    int m_version;
    DWORD m_fileSize, m_fileCount;
    GME_EVENT m_event;
};

