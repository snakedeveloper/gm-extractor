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
#include "GMExtractorHandler.h"
#include "GMEApplication.h"

const char* CExtractTask::m_tasks[] = {
  "Various resources",
  "Sprites",
  "Backgrounds",
  "Sounds"
};

void CExtractTask::Execute() {
  m_hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) ThreadProc, this, 0, NULL );
}

void CExtractTask::Finish() {
  application.mainWindow.buttonExtract.Enabled( true );
  application.mainWindow.buttonDir.Enabled( true );
  application.mainWindow.buttonFile.Enabled( true );
  FlashWindow( application.mainWindow.hwnd(), true );

  CloseHandle( m_hThread );
}

DWORD CExtractTask::ThreadProc( CExtractTask* aSelf ) {
  CStaticStatus& status = application.mainWindow.staticStatus;

  CGMExtractor extractor;
  extractor.SetDirectory( aSelf->m_dirPath.c_str() );

  switch ( extractor.Open( aSelf->m_filePath.c_str() ) ) {
    case GMEERR_SUCCESS:
      extractor.SetEventProcedure( EventProc );
      extractor.Run();
      break;

    case GMEERR_INCOMPATIBLE_VERSION:
      status.SetStatus( AS_INCOMPATIBLE_VERSION );
      break;

    case GMEERR_FILE_CORRUPTED:
      status.SetStatus( AS_FILE_CORRUPTED );
      break;
  }

  extractor.Close();
  aSelf->Finish();
  return 0;
}

bool CExtractTask::EventProc( GMEEvent aEvent, void* aParam1, void* aParam2 ) {
  CListBoxLog& log = application.mainWindow.listboxLog;
  CStaticStatus& status = application.mainWindow.staticStatus;

  switch ( aEvent ) {
    case GMEE_NEW_TASK: {
      GMEExtractionTask task = (GMEExtractionTask) (int) aParam1; 
      log.AddItem( m_tasks[task], 1 );
      break;
    }

    case GMEE_EXTRACTION_VERIFY: {
      GMEError error = (GMEError) (int) aParam1;
      if ( error == GMEERR_FILE_CORRUPTED )
        return false;
      break;
    }

    case GMEE_RESOURCE_EXTRACTED:
      log.AddItem( (const char*) aParam1, 0 );
      break;

    case GMEE_END: {
      GMEError error = (GMEError) (int) aParam1;

      if ( error == GMEERR_SUCCESS ) {
        status.SetStatus( AS_FINISHED );

        char buffer[0x100] = {0};
        status.GetText( buffer, 0x100 );
        std::stringstream newText;
        newText << buffer << std::endl << "Extracted " << (DWORD) aParam2 << " files.";
        status.SetText( newText.str().c_str() );
      } else
        status.SetStatus( AS_FILE_CORRUPTED );
      break;
    }
  }

  return true;
}

