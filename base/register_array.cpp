// base/register_array.cpp

#include "ttl_define.h"

#include "utility.h"

#include "base/register_array.h"

using namespace BMX2WAV;


// -- RegisterArray ------------------------------------------------------
BL::RegisterArray::RegisterArray( const std::string& name ) :
name_( name )
{
}


const std::string&
BL::RegisterArray::GetName( void ) const
{
  return name_;
}

BL::RegisterArray::Array&
BL::RegisterArray::GetArray( void )
{
  return array_;
}


std::string
BL::RegisterArray::At( BL::Word position ) const
{
  return array_[position].value();
}

std::string
BL::RegisterArray::operator []( BL::Word position ) const
{
  return this->At( position );
}


bool
BL::RegisterArray::CanApply( const std::string& str )
{
  if ( str.size() != name_.size() + 2 ) {
    return false;
  }
  std::string tmp = str.substr( 0, name_.size() );
  if ( NOT( Utility::KeyEqual( str.substr( 0, name_.size() ), name_ ) ) ) {
    return false;
  }
  if ( NOT( BL::Word::CanConstructAsWord( str.c_str() + name_.size() ) ) ) {
    return false;
  }
  return true;
}


unsigned int
BL::RegisterArray::GetExistCount( void ) const
{
  unsigned int count = 0;
  for ( auto& one : array_ ) {
    if ( one ) {
      count += 1;
    }
  }
  return count;
}

bool
BL::RegisterArray::IsExists( Word position ) const
{
  return array_[position].has_value();
}

bool
BL::RegisterArray::IsNotExists( Word position ) const
{
  return NOT( this->IsExists( position ) );
}
