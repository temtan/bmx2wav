// base/base_exception.cpp

#include <sstream>

#include "string_table.h"
#include "utility.h"

#include "base/base_exception.h"

using namespace BMX2WAV;


// -- BaseLibraryException -----------------------------------------------
BL::BaseLibraryException::BaseLibraryException( void ) :
Exception()
{
}


// -- InvalidCharUsedAsWordException -------------------------------------
BL::InvalidCharUsedAsWordException::InvalidCharUsedAsWordException( char msb, char lsb ) :
msb_( msb ),
lsb_( lsb )
{
}

char
BL::InvalidCharUsedAsWordException::GetMsb( void ) const
{
  return msb_;
}

char
BL::InvalidCharUsedAsWordException::GetLsb( void ) const
{
  return lsb_;
}

std::string
BL::InvalidCharUsedAsWordException::GetAsString( void ) const
{
  std::string tmp;
  tmp.append( msb_, 1 );
  tmp.append( lsb_, 1 );
  return tmp;
}


std::string
BL::InvalidCharUsedAsWordException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InvalidCharUsedAsWord.Get(), this->GetAsString() );
}

std::string
BL::InvalidCharUsedAsWordException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << this->GetAsString();
  return os.str();
}


// -- InvalidWordValueUsedException ------------------------------------
BL::InvalidWordValueUsedException::InvalidWordValueUsedException( int value ) :
value_( value )
{
}

int
BL::InvalidWordValueUsedException::GetValue( void ) const
{
  return value_;
}


std::string
BL::InvalidWordValueUsedException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InvalidWordValueUsed.Get(), this->GetValue() );
}


std::string
BL::InvalidWordValueUsedException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << this->GetValue();
  return os.str();
}


// -- BufferOutOfRangeAccessException ------------------------------------
BL::BufferOutOfRangeAccessException::BufferOutOfRangeAccessException( unsigned int position ) :
position_( position )
{
}


unsigned int
BL::BufferOutOfRangeAccessException::GetPosition( void ) const
{
  return position_;
}


std::string
BL::BufferOutOfRangeAccessException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BufferOutOfRangeAccess.Get(), this->GetPosition() );
}

std::string
BL::BufferOutOfRangeAccessException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << this->GetPosition();
  return os.str();
}


// -- RequiredResolutionIsTooBigException --------------------------------
BL::RequiredResolutionIsTooBigException::RequiredResolutionIsTooBigException( unsigned int required_resolution ) :
required_resolution_( required_resolution )
{
}

unsigned int
BL::RequiredResolutionIsTooBigException::GetRequiredResolution( void ) const
{
  return required_resolution_;
}

std::string
BL::RequiredResolutionIsTooBigException::GetMessage( void )
{
  return Utility::Format( StrT::Message::RequiredResolutionIsTooBig.Get(), this->GetRequiredResolution() );
}

std::string
BL::RequiredResolutionIsTooBigException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << this->GetRequiredResolution();
  return os.str();
}
