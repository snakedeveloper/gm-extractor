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
#include "GMExtractor.h"

class CExtractTask {
  public:
    void Reset( const char* aProjectFile, const char* aDirectoryPath ) {
      m_fileCount = 0;
      m_filePath = aProjectFile;
      m_dirPath = aDirectoryPath;
    }

    void Execute();
    void Finish();

  private:
    static DWORD CALLBACK ThreadProc( CExtractTask* aSelf );
    static bool EventProc( GMEEvent aEvent, void* aParam1, void* aParam2 );

    std::string m_filePath, m_dirPath;

    HANDLE m_hThread;
    int m_fileCount;

    static const char* m_tasks[];
};
