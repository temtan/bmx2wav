// base/channel.cpp

#include <numeric>

#include "ttl_define.h"

#include "exception.h"
#include "base/base_exception.h"

#include "base/channel.h"

using namespace BMX2WAV;


// -- Buffer -------------------------------------------------------------
BL::Buffer::Buffer( void ) :
Buffer( 1 )
{
}

BL::Buffer::Buffer( unsigned int length ) :
array_( length )
{
}


unsigned int
BL::Buffer::GetLength( void ) const
{
  return static_cast<unsigned int>( array_.size() );
}

std::vector<BL::Word>&
BL::Buffer::GetArray( void )
{
  return array_;
}


BL::Word&
BL::Buffer::operator []( unsigned int position )
{
  this->IfPositionIsOverTheLengthThenRaiseError( position );
  return array_[position];
}

const BL::Word&
BL::Buffer::operator []( unsigned int position ) const
{
  this->IfPositionIsOverTheLengthThenRaiseError( position );
  return array_[position];
}


void
BL::Buffer::ZeroClearAt( unsigned int position )
{
  this->IfPositionIsOverTheLengthThenRaiseError( position );
  this->operator []( position ) = BL::Word::MIN;
}

void
BL::Buffer::ZeroClearAll( void )
{
  for ( auto& one : array_ ) {
    one = BL::Word::MIN;
  }
}


void
BL::Buffer::Merge( const BL::Buffer& buffer )
{
  this->Merge( 0, buffer );
}

void
BL::Buffer::Merge( unsigned int start, const BL::Buffer& buffer )
{
  if ( start + buffer.GetLength() > this->GetLength() ) {
    throw BufferOutOfRangeAccessException( start + buffer.GetLength() );
  }
  for ( unsigned int i = 0; ; ++i ) {
    if ( i > start + array_.size() || i > buffer.GetLength() ) {
      break;
    }
    if ( buffer[i] != BL::Word::MIN ) {
      array_[start + i] = buffer[i];
    }
  }
}


bool
BL::Buffer::Contains( BL::Word word ) const
{
  for ( auto& one : array_ ) {
    if ( one == word ) {
      return true;
    }
  }
  return false;
}

bool
BL::Buffer::ContainsAnyObject( void ) const
{
  for ( auto& one : array_ ) {
    if ( one != BL::Word::MIN ) {
      return true;
    }
  }
  return false;
}


bool
BL::Buffer::Empty( void ) const
{
  return NOT( this->ContainsAnyObject() );
}


unsigned int
BL::Buffer::GetObjectCount( void ) const
{
  unsigned int sum = 0;
  for ( auto& one : array_ ) {
    if ( one != BL::Word::MIN ) {
      sum += 1;
    }
  }
  return sum;
}


void
BL::Buffer::Resize( size_t size )
{
  array_.resize( size );
}

void
BL::Buffer::MagnifyBy( unsigned int multiplier )
{
  if ( multiplier == 0 ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  if ( multiplier == 1 ) {
    return;
  }
  size_t old_length = this->GetLength();
  this->Resize( this->GetLength() * multiplier );
  for ( size_t i = old_length - 1; i > 0; --i ) {
    array_[i * multiplier] = array_[i];
    array_[i] = BL::Word::MIN;
  }
}

void
BL::Buffer::SafetyShrink( void )
{
  unsigned int divisor = static_cast<unsigned int>( array_.size() );
  for ( unsigned int i = 0; i < array_.size(); ++i ) {
    if ( array_[i] != BL::Word::MIN ) {
      divisor = std::gcd( divisor, i );
    }
  }
  if ( divisor == 1 ) {
    return;
  }
  for ( unsigned int i = 0; i < array_.size(); ++i ) {
    if ( i % divisor == 0 ) {
      array_[i / divisor] = array_[i];
    }
  }
  array_.resize( array_.size() / divisor );
}


std::string
BL::Buffer::ToString( void )
{
  std::string tmp;
  for ( auto word : *this ) {
    tmp.append( word.ToCharPointer() );
  }
  return tmp;
}



BL::Buffer::Iterator
BL::Buffer::begin( void )
{
  return array_.begin();
}

BL::Buffer::Iterator
BL::Buffer::end( void )
{
  return array_.end();
}


void
BL::Buffer::IfPositionIsOverTheLengthThenRaiseError( unsigned int pos ) const
{
  if ( pos >= array_.size() ) {
    throw BufferOutOfRangeAccessException( pos );
  }
}


// -- Channel ------------------------------------------------------------
bool
BL::Channel::NumberIsWavChannel( BL::Word number )
{
  return ( number == "01"_hex36 ||
           "11"_hex36 <= number && number <= "1Z"_hex36 ||
           "21"_hex36 <= number && number <= "2Z"_hex36 ||
           "31"_hex36 <= number && number <= "3Z"_hex36 ||
           "41"_hex36 <= number && number <= "4Z"_hex36 ||
           "51"_hex36 <= number && number <= "5Z"_hex36 ||
           "61"_hex36 <= number && number <= "6Z"_hex36 );
}

bool
BL::Channel::NumberIsShoudPlayChannel( BL::Word number )
{
  return ( number == "01"_hex36 ||
           "11"_hex36 <= number && number <= "1Z"_hex36 ||
           "21"_hex36 <= number && number <= "2Z"_hex36 ||
           "51"_hex36 <= number && number <= "5Z"_hex36 ||
           "61"_hex36 <= number && number <= "6Z"_hex36 );
}

bool
BL::Channel::NumberIsBmpChannel( BL::Word number )
{
  switch ( number.ToInteger() ) {
    // 04 06 07 0A A5
  case 4:
  case 6:
  case 7:
  case 10:
  case 365:
    return true;

  default:
    return false;
  }
}

bool
BL::Channel::NumberIsBpmChangeChannel( BL::Word number )
{
  return number == "03"_hex36;
}

bool
BL::Channel::NumberIsExtendedBpmChangeChannel( BL::Word number )
{
  return number == "08"_hex36;
}

bool
BL::Channel::NumberIsInvisibilityObjectChannel( BL::Word number )
{
  return ( "31"_hex36 <= number && number <= "3Z"_hex36 ||
           "41"_hex36 <= number && number <= "4Z"_hex36 );
}

bool
BL::Channel::NumberIsLongNoteChannel( BL::Word number )
{
  return ( "51"_hex36 <= number && number <= "5Z"_hex36 ||
           "61"_hex36 <= number && number <= "6Z"_hex36 );
}

bool
BL::Channel::NumberIsStopSequenceChannel( BL::Word number )
{
  return number == "09"_hex36;
}


bool
BL::Channel::NumberIsPlayerNoteChannel( BL::Word number )
{
  return ( Channel::NumberIsFirstPlayerNoteChannel( number ) ||
           Channel::NumberIsSecondPlayerNoteChannel( number ) );
}

bool
BL::Channel::NumberIsFirstPlayerNoteChannel( BL::Word number )
{
  return ("11"_hex36 <= number && number <= "1Z"_hex36 ||
          "51"_hex36 <= number && number <= "5Z"_hex36 );
}


bool
BL::Channel::NumberIsSecondPlayerNoteChannel( BL::Word number )
{
  return ("21"_hex36 <= number && number <= "2Z"_hex36 ||
          "61"_hex36 <= number && number <= "6Z"_hex36 );
}

bool
BL::Channel::NumberIsLandmineObjectChannel( BL::Word number )
{
  return ("D1"_hex36 <= number && number <= "DZ"_hex36 ||
          "E1"_hex36 <= number && number <= "EZ"_hex36 );
}

// -----------------------------------------
BL::Channel::Channel( BL::Word number ) :
Channel( number, 1 )
{
}

BL::Channel::Channel( BL::Word number, unsigned int length ) :
Buffer( length ),
number_( number )
{
}


BL::Word
BL::Channel::GetChannelNumber( void ) const
{
  return number_;
}

bool
BL::Channel::IsWavChannel( void ) const
{
  return BL::Channel::NumberIsWavChannel( number_ );
}

bool
BL::Channel::IsShoudPlayChannel( void ) const
{
  return BL::Channel::NumberIsShoudPlayChannel( number_ );
}

bool
BL::Channel::IsBmpChannel( void ) const
{
  return BL::Channel::NumberIsBmpChannel( number_ );
}

bool
BL::Channel::IsBpmChangeChannel( void ) const
{
  return BL::Channel::NumberIsBpmChangeChannel( number_ );
}

bool
BL::Channel::IsInvisibilityObjectChannel( void ) const
{
  return BL::Channel::NumberIsInvisibilityObjectChannel( number_ );
}

bool
BL::Channel::IsExtendedBpmChangeChannel( void ) const
{
  return BL::Channel::NumberIsExtendedBpmChangeChannel( number_ );
}

bool
BL::Channel::IsLongNoteChannel( void ) const
{
  return BL::Channel::NumberIsLongNoteChannel( number_ );
}

bool 
BL::Channel::IsStopSequenceChannel( void ) const
{
  return BL::Channel::NumberIsStopSequenceChannel( number_ );
}

bool
BL::Channel::IsPlayerNoteChannel( void ) const
{
  return BL::Channel::NumberIsPlayerNoteChannel( number_ );
}

bool
BL::Channel::IsFirstPlayerNoteChannel( void ) const
{
  return BL::Channel::NumberIsFirstPlayerNoteChannel( number_ );
}

bool
BL::Channel::IsSecondPlayerNoteChannel( void ) const
{
  return BL::Channel::NumberIsSecondPlayerNoteChannel( number_ );
}

bool
BL::Channel::IsLandmineObjectChannel( void ) const
{
  return BL::Channel::NumberIsLandmineObjectChannel( number_ );
}
