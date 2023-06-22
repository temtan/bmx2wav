// utility.cpp

#include "tt_windows_h_include.h"
#include <Psapi.h>

#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "winmm.lib")

#include "ttl_define.h"
#include "tt_string.h"
#include "tt_path.h"

#include "common.h"
#include "tt_exception.h"

#include "utility.h"


using namespace BMX2WAV;


// -- Utility ------------------------------------------------------------
int
Utility::Hex36ToInteger( char c )
{
  switch ( c ) {
  case '0':           return 0;
  case '1':           return 1;
  case '2':           return 2;
  case '3':           return 3;
  case '4':           return 4;
  case '5':           return 5;
  case '6':           return 6;
  case '7':           return 7;
  case '8':           return 8;
  case '9':           return 9;
  case 'a': case 'A': return 10;
  case 'b': case 'B': return 11;
  case 'c': case 'C': return 12;
  case 'd': case 'D': return 13;
  case 'e': case 'E': return 14;
  case 'f': case 'F': return 15;
  case 'g': case 'G': return 16;
  case 'h': case 'H': return 17;
  case 'i': case 'I': return 18;
  case 'j': case 'J': return 19;
  case 'k': case 'K': return 20;
  case 'l': case 'L': return 21;
  case 'm': case 'M': return 22;
  case 'n': case 'N': return 23;
  case 'o': case 'O': return 24;
  case 'p': case 'P': return 25;
  case 'q': case 'Q': return 26;
  case 'r': case 'R': return 27;
  case 's': case 'S': return 28;
  case 't': case 'T': return 29;
  case 'u': case 'U': return 30;
  case 'v': case 'V': return 31;
  case 'w': case 'W': return 32;
  case 'x': case 'X': return 33;
  case 'y': case 'Y': return 34;
  case 'z': case 'Z': return 35;
  default:            return -1;
  }
}

char
Utility::IntegerToHex36( int x )
{
  const char* hex36_table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  return hex36_table[x % Const::HEX36_MAX_COUNT];
}


bool
Utility::IsDigit( char c )
{
  int tmp = Utility::Hex36ToInteger( c );
  return tmp >= 0 && tmp <= 9;
}

bool
Utility::IsNotDigit( char c )
{
  return NOT( Utility::IsDigit( c ) );
}

bool
Utility::IsAlphabet( char c )
{
  int tmp = Utility::Hex36ToInteger( c );
  return tmp >= 10 && tmp <= 35;
}

bool
Utility::IsNotAlphabet( char c )
{
  return NOT( Utility::IsAlphabet( c ) );
}

bool
Utility::IsHex36( char c )
{
  return Utility::Hex36ToInteger( c ) >= 0;
}

bool
Utility::IsNotHex36( char c )
{
  return NOT( Utility::IsHex36( c ) );
}


double
Utility::Round( double x )
{
  return floor( x + 0.5 );
}


unsigned int
Utility::GetDenominator( double x, unsigned int max )
{
  const double epsilon = 0.0000000001;
  if ( x < 0.0 ) {
    x = std::fabs( x );
  }
  if ( x > 1.0 ) {
    x -= std::floor( x );
  }
  if ( x < epsilon ) {
    return 1;
  }

  for ( unsigned int i = 1; i < max; ++i ) {
    if ( std::fabs( x * i - std::floor( x * i + 0.5 ) ) < epsilon ) {
      return i;
    }
  }
  return max;
}


bool
Utility::KeyEqual( const std::string& s1, const std::string& s2 )
{
  return TtString::ToUpper( s1 ) == TtString::ToUpper( s2 );
}


bool
Utility::MakeDirectoryOfFileIfNotExist( const std::string& path )
{
  std::string dir_path = TtPath::DirName( path );
  if ( NOT( TtPath::IsDirectory( dir_path ) ) ) {
    return ::SHCreateDirectoryEx( nullptr, dir_path.c_str(), nullptr ) == ERROR_SUCCESS;
  }
  return true;
}


int64_t
Utility::GetCurrentProcessWorkingSetSize( void )
{
  PROCESS_MEMORY_COUNTERS info;
  ::GetProcessMemoryInfo( ::GetCurrentProcess(), &info, sizeof( info ) );
  return info.WorkingSetSize;
}


bool
Utility::UserDefaultLocaleIsJapanese( void )
{
  return ::GetUserDefaultLCID() == 0x0411;
}

std::string
Utility::GetLanguageDirectoryPath( void )
{
  return TtPath::GetExecutingDirectoryPath() + "\\lang";
}


std::string
Utility::RemoveCanNotUseCharacterAsFilePathFrom( const std::string& path )
{
  auto buffer = std::make_unique<char[]>( path.size() + 1 );
  char* c = buffer.get();
  const char* s = path.c_str();
  if ( NOT( TtPath::IsRelative( path ) ) ) {
    *c = *s;
    ++c;
    ++s;
    *c = *s;
    ++c;
    ++s;
  }
  while ( *s != '\0' ) {
    *c = *s;
    switch ( *s ) {
    case '/':
    case ':':
    case '*':
    case '?':
    case '"':
    case '<':
    case '>':
      break;

    case '|':
      if ( s != path.c_str() && (static_cast<unsigned char>( s[-1] ) >= 0x81 && static_cast<unsigned char>( s[-1] ) <= 0x9F) ) {
        ++c;
      }
      break;

    default:
      ++c;
      break;
    }
    ++s;
  }
  *c = *s;
  return buffer.get();

  /*
  std::string tmp = path;
  std::string::iterator it = tmp.begin();
  if ( NOT( TtPath::IsRelative( tmp ) ) ) {
    ++it;
    ++it;
  }
  while ( it != tmp.end() ) {
    switch ( *it ) {
    case '/':
    case ':':
    case '*':
    case '?':
    case '"':
    case '<':
    case '>':
    case '|':
      it = tmp.erase( it );
      continue;
    }
    ++it;
  }
  return tmp;
   */
}


// -- TextFileReader -----------------------------------------------------
Utility::TextFileReader::TextFileReader( const std::string& path ) :
TtFileReader( path, false ),
read_as_utf8_( this->HasUTF8BOM() )
{
}

std::optional<std::string>
Utility::TextFileReader::ReadLineAutoEncode( void )
{
  auto ret = this->ReadLine();
  if ( ret && read_as_utf8_ ) {
    ret = TtString::UTF8ToCP932( ret.value() );
  }
  return ret;
}

std::string
Utility::TextFileReader::ReadAllAutoEncode( void )
{
  auto ret = this->ReadAll();
  if ( read_as_utf8_ ) {
    ret = TtString::UTF8ToCP932( ret );
  }
  return ret;
}
