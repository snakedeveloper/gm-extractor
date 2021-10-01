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

int WINAPI WinMain( HINSTANCE aThisInstance, HINSTANCE aPrevInstance,
                    LPSTR aCmdLine, int aCmdShow ) {
  int exitCode = 0;

  application.Create();
  exitCode = application.Run();
  application.Destroy();

  return exitCode;
}