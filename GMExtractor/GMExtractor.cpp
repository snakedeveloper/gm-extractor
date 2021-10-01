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
#include "GMExtractor.h"

#define EVENT_VERIFY( err ) if(!CallEvent(GMEE_EXTRACTION_VERIFY,(void*) err ,NULL)){return;}

GMEError CGMExtractor::Open( const char* aProjectFile ) {
  m_filePath = aProjectFile;
  m_input.open( m_filePath.c_str(), std::ios::binary );

  if ( !m_input.is_open() )
    return GMEERR_NO_ACCESS;

  DWORD sig = 0;
  m_input.read( (char*) &sig, 4 );

  if ( sig != 1234321 ) {
    m_input.close();
    return GMEERR_FILE_CORRUPTED;
  }

  m_input.read( (char*) &m_version, 4 );

  if ( !(m_version == 530 || m_version == 600 || m_version == 701) ) {
    m_version = 0;
    return GMEERR_INCOMPATIBLE_VERSION;
  }

  m_input.seekg( 0, std::ios::beg );

  m_buffer << m_input.rdbuf();

  m_buffer.seekg( 0, std::ios::end );
  m_fileSize = m_buffer.tellg();
  m_buffer.seekg( 8, std::ios::beg );

  m_input.close();

  switch ( m_version ) {
    case 701:
      if ( !DecryptProject() )
        return GMEERR_FILE_CORRUPTED;
      m_buffer.seekg( 0x5C, std::ios::cur );
      break;

    case 600:
      m_buffer.seekg( 0x68, std::ios::cur );
      break;

    case 530:
      m_buffer.seekg( 0x74, std::ios::cur );
  }

  return ( m_buffer.fail() ? GMEERR_FILE_CORRUPTED : GMEERR_SUCCESS );
}

BYTE* CGMExtractor::CreateSwapTable( int aSeed ) {
  int a, b, j, i, t;

  BYTE* swapTable = new BYTE[0x200];
  ZeroMemory( swapTable, 0x200 );

  a = 6 + ( aSeed % 250 );
  b = aSeed / 250;

  for ( i = 0; i < 256; i++ )
    swapTable[i] = i;

  for ( int i = 1; i < 10001; i++ ) {
    j = 1 + ((i * a + b) % 254);
    t = swapTable[j];
    swapTable[j] = swapTable[j + 1];
    swapTable[j + 1] = t;
  }

  swapTable[256] = 0;

  for ( i = 1; i < 256; i++ )
    swapTable[ 256 + swapTable[i] ] = i;

  return swapTable;
}

void CGMExtractor::DestroySwapTable( BYTE* aSwapTable ) {
  delete [] aSwapTable;
}

bool CGMExtractor::DecryptProject() {
  DWORD seedPos, dataPos, dataSize, offset;
  int seed;

  m_buffer.seekg( 8, std::ios::beg );

  m_buffer.read( (char*) &seedPos, 4 );
  m_buffer.read( (char*) &dataPos, 4 );

  m_buffer.seekg( seedPos * 4, std::ios::cur );
  m_buffer.read( (char*) &seed, 4 );
  m_buffer.seekg( (dataPos * 4) + 1, std::ios::cur );

  if ( m_buffer.fail() )
    return false;

  BYTE* swapTable = CreateSwapTable( seed );
  BYTE* data = (BYTE*) m_buffer.str();

  offset = (int) m_buffer.tellg();
  dataSize = m_fileSize - offset;

  for ( DWORD i = 0; i < dataSize; i++ )
    data[i] = swapTable[ 256 + data[i] ] - (BYTE) (offset + i);

  DestroySwapTable( swapTable );

  m_buffer.seekg( 0x13, std::ios::cur );
  return ( *(int*) m_buffer.str() == 702 );
}

GMEError CGMExtractor::ProcessResource( bool aIsCompressed, const char* aOutputFileName ) {
  DWORD resourceSize;
  m_buffer.read( (char*) &resourceSize, 4 );

  GMEError result = SaveResource( (BYTE*) m_buffer.str(), resourceSize, aIsCompressed,
                                  aOutputFileName );

  m_buffer.seekg( resourceSize, std::ios::cur );
  return ( m_buffer.fail() ? GMEERR_SUCCESS : result );
}

GMEError CGMExtractor::ProcessConditionalResource( bool aIsCompressed, const char* aOutputFileName ) {
  int condition;
  m_buffer.read( (char*) &condition, 4 );

  if ( condition == 10 )
    return ProcessResource( aIsCompressed, aOutputFileName );

  return GMEERR_SUCCESS;
}

GMEError CGMExtractor::SaveResource( BYTE* aResourceData, DWORD aResourceSize,
                                     bool aIsCompressed, const char* aFileName ) {
  if ( aResourceSize > m_fileSize || aResourceSize == 0 )
    return GMEERR_FILE_CORRUPTED;

  GMEError result = GMEERR_SUCCESS;

  DWORD bufferSize = aResourceSize;
  BYTE* buffer = aResourceData; 

  if ( aIsCompressed ) {
    for ( int i = 0x100; i < 0x1000; i += 0x100 ) {
      bufferSize = aResourceSize * i;
      buffer = new BYTE[bufferSize];

      int state = zlib::uncompress( buffer, &bufferSize, aResourceData, aResourceSize );
        
      if ( state == Z_DATA_ERROR ) {
        result = GMEERR_FILE_CORRUPTED;
        break;
      } else if ( state == Z_BUF_ERROR )
        delete [] buffer;
      else
        break;
    }
  }

  if ( result == GMEERR_SUCCESS ) {
    std::string filePath( m_dirPath );
    filePath += '\\'; filePath += aFileName;

    std::ofstream output( filePath.c_str(), std::ios::binary );

    if ( !output.is_open() )
      result = GMEERR_NO_ACCESS;
    else {
      output.write( (char*) buffer, bufferSize );
      output.close();

      ++m_fileCount;
    }
  }

  if ( aIsCompressed )
    delete [] buffer;

  return result;
}

void CGMExtractor::SkipVariableLengthData() {
  DWORD length;
  m_buffer.read( (char*) &length, 4 );
  m_buffer.seekg( length, std::ios::cur );
}

std::string CGMExtractor::GetVariableLengthString() {
  std::string buffer;
  DWORD length;
  m_buffer.read( (char*) &length, 4 );

  if ( length > m_fileSize )
    return buffer;

  char* temp = new char[length + 1];
  ZeroMemory( temp, length + 1 );
  m_buffer.read( temp, length );

  buffer = temp;
  delete [] temp;

  return buffer;
}

void CGMExtractor::Run() {
  int buffer, count;

  CallEvent( GMEE_NEW_TASK, (void*) GMET_VARIOUS, NULL );

  m_buffer.read( (char*) &buffer, 4 );

  // Custom progress bar
  if ( buffer == 2 ) {
    EVENT_VERIFY( ProcessConditionalResource( true, "various_progressbar_front.bmp" ) );
    CallEvent( GMEE_RESOURCE_EXTRACTED, "Front loading progressbar", NULL );
    EVENT_VERIFY( ProcessConditionalResource( true, "various_progressbar_back.bmp" ) );
    CallEvent( GMEE_RESOURCE_EXTRACTED, "Back loading progressbar", NULL );  
  }

  m_buffer.read( (char*) &buffer, 4 );

  // Custom loading image
  if ( buffer == 1 ) {
    EVENT_VERIFY( ProcessConditionalResource( true, "various_loading_image.bmp" ) );
    CallEvent( GMEE_RESOURCE_EXTRACTED, "Loading image", NULL );
  }

  m_buffer.seekg( 0xC, std::ios::cur );

  // Game icon
  EVENT_VERIFY( ProcessResource( false, "various_gameicon.ico" ) );
  CallEvent( GMEE_RESOURCE_EXTRACTED, "Game icon", NULL );

  m_buffer.seekg( 0x10, std::ios::cur );
  SkipVariableLengthData();

  if ( m_version == 701 ) {
    SkipVariableLengthData();
    m_buffer.seekg( 0x8, std::ios::cur );
  } else
    m_buffer.seekg( 0xC, std::ios::cur );

  SkipVariableLengthData();

  m_buffer.read( (char*) &buffer, 4 );
  for ( int i = buffer; i > 0; i-- ) {
    SkipVariableLengthData();
    SkipVariableLengthData();
  }

  if ( m_version == 701 ) {
    m_buffer.seekg( 0x10, std::ios::cur );
    SkipVariableLengthData();
    SkipVariableLengthData();
    SkipVariableLengthData();
    SkipVariableLengthData();
  } else if ( m_version == 600 ) {
    m_buffer.read( (char*) &buffer, 4 );
    for ( int i = buffer; i > 0; i-- )
      SkipVariableLengthData();

    m_buffer.seekg( 0xC, std::ios::cur );
  }

  m_buffer.seekg( 0x4, std::ios::cur );

  CallEvent( GMEE_NEW_TASK, (void*) GMET_SOUNDS, NULL );

  // Sounds
  m_buffer.read( (char*) &count, 4 );
  for ( int i = 0; i < count; i++ ) {
    m_buffer.read( (char*) &buffer, 4 );
    if ( buffer == 1 ) {
      bool skipData = false;
      std::stringstream fileName;
      std::string resName( GetVariableLengthString() );

      if ( !resName.size() )
        if ( !CallEvent( GMEE_END, (void*) GMEERR_FILE_CORRUPTED, NULL ) )
          return;

      m_buffer.seekg( 0x4, std::ios::cur );

      if ( m_version == 530 ) {
        m_buffer.read( (char*) &buffer, 4 );
        skipData = ( buffer < 0 );
      } else
        m_buffer.seekg( 0x4, std::ios::cur );

      std::string fileExt( GetVariableLengthString() );

      if ( !fileExt.size() ) {
        CallEvent( GMEE_END, (void*) GMEERR_FILE_CORRUPTED, NULL );
        return;
      }

      if ( m_version > 530 ) {
        SkipVariableLengthData();
        m_buffer.read( (char*) &buffer, 4 );
        skipData = !(buffer & 1);
      }

      fileName << "sound_" << resName << '[' << i << ']' << fileExt;

      if ( !skipData ) {
        EVENT_VERIFY( ProcessResource( true, fileName.str().c_str() ) );
        CallEvent( GMEE_RESOURCE_EXTRACTED, (void*) resName.c_str(), NULL );
      }

      m_buffer.seekg( 0xC, std::ios::cur );

      if ( m_version > 530 )
        m_buffer.seekg( 0xC, std::ios::cur );
    }
  }

  m_buffer.seekg( 0x4, std::ios::cur );

  CallEvent( GMEE_NEW_TASK, (void*) GMET_SPRITES, NULL );

  // Sprites
  m_buffer.read( (char*) &count, 4 );
  for ( int i = 0; i < count; i++ ) {
    m_buffer.read( (char*) &buffer, 4 );
    if ( buffer == 1 ) {
      std::stringstream fileName;
      std::string resName( GetVariableLengthString() );

      if ( !resName.size() )
        if ( !CallEvent( GMEE_END, (void*) GMEERR_FILE_CORRUPTED, NULL ) )
          return;

      m_buffer.seekg( 0x38, std::ios::cur );

      m_buffer.read( (char*) &buffer, 4 );
      for ( int j = 0; j < buffer; j++ ) {
        fileName << "sprite_" << resName << '(' << j << ')' << '[' << i << "].bmp";
        EVENT_VERIFY( ProcessConditionalResource( true, fileName.str().c_str() ) );
        fileName.str( "" );
      }

      CallEvent( GMEE_RESOURCE_EXTRACTED, (void*) resName.c_str(), NULL );
    }
  }

  m_buffer.seekg( 0x4, std::ios::cur );

  CallEvent( GMEE_NEW_TASK, (void*) GMET_BACKGROUNDS, NULL );

  // Backgrounds
  m_buffer.read( (char*) &count, 4 );
  for ( int i = 0; i < count; i++ ) {
    m_buffer.read( (char*) &buffer, 4 );
    if ( buffer == 1 ) {
      std::stringstream fileName;
      std::string resName( GetVariableLengthString() );

      if ( !resName.size() )
        if ( !CallEvent( GMEE_END, (void*) GMEERR_FILE_CORRUPTED, NULL ) )
          return;

      m_buffer.seekg( 0x18, std::ios::cur );

      if ( m_version > 530 )
        m_buffer.seekg( 0x1C, std::ios::cur );
 
      m_buffer.read( (char*) &buffer, 4 );
      if ( buffer ) {
        fileName << "background_" << resName << '[' << i << "].bmp";
        EVENT_VERIFY( ProcessConditionalResource( true, fileName.str().c_str() ) );
        fileName.str( "" );
      }

      CallEvent( GMEE_RESOURCE_EXTRACTED, (void*) resName.c_str(), NULL );
    }
  }

  CallEvent( GMEE_END, (void*) GMEERR_SUCCESS, (void*) m_fileCount );
}

void CGMExtractor::Close() {
  m_fileCount = 0;
  m_buffer.freeze( false );
  m_buffer.clear();
}

