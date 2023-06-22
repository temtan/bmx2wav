// base/bar.cpp

#include "ttl_define.h"

#include "common.h"
#include "exception.h"
#include "utility.h"

#include "base/base_exception.h"

#include "base/bar.h"

using namespace BMX2WAV;


// -- Bar ----------------------------------------------------------------
BL::Bar::Bar( void ) :
resolution_( 1 ),
ratio_( 1.0 ),
channels_()
{
}


unsigned int
BL::Bar::GetResolution( void ) const
{
  return resolution_;
}

double
BL::Bar::GetRatio( void ) const
{
  return ratio_;
}

void
BL::Bar::SetRatio( double ratio )
{
  ratio_ = ratio;
}

void
BL::Bar::MultiplyResolution( unsigned int multiplier )
{
  if ( multiplier == 0 ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  if ( multiplier == 1 ) {
    return;
  }
  // if ( resolution_ * multiplier > Const::BAR_RESOLUTION_MAX ) {
  //  throw RequiredResolutionIsTooBigException( resolution_ * multiplier );
  // }
  resolution_ *= multiplier;
  for ( auto& bgm_channel : bgm_channels_ ) {
    bgm_channel->MagnifyBy( multiplier );
  }
  for ( auto& number_channel : channels_ ) {
    number_channel.second->MagnifyBy( multiplier );
  }
}


bool
BL::Bar::ContainsAnyObject( void ) const
{
  for ( auto& bgm_channel : bgm_channels_ ) {
    if ( bgm_channel->ContainsAnyObject() ) {
      return true;
    }
  }
  for ( auto& number_channel : channels_ ) {
    if ( number_channel.second->ContainsAnyObject() ) {
      return true;
    }
  }
  return false;
}

bool
BL::Bar::Empty( void ) const
{
  return NOT( this->ContainsAnyObject() );
}


unsigned int
BL::Bar::GetObjectCountOf( std::function<bool ( BL::Word channel_number )> should_count_channel ) const
{
  unsigned int sum = 0;
  for ( auto& bgm_channel : bgm_channels_ ) {
    if ( should_count_channel( bgm_channel->GetChannelNumber() ) ) {
      sum += bgm_channel->GetObjectCount();
    }
  }
  for ( auto& number_channel : channels_ ) {
    if ( should_count_channel( number_channel.second->GetChannelNumber() ) ) {
      sum += number_channel.second->GetObjectCount();
    }
  }
  return sum;
}


BL::Channel&
BL::Bar::GetChannelBy( BL::Word channel_number )
{
  if ( channel_number == "01"_hex36 ) {
    if ( bgm_channels_.empty() ) {
      return this->MakeNewBgmChannel();
    }
    return *bgm_channels_.front();
  }
  auto it = channels_.find( channel_number );
  if ( it != channels_.end() ) {
    return *it->second;
  }
  auto tmp = std::shared_ptr<Channel>( new Channel( channel_number, resolution_ ) );
  channels_.emplace( channel_number, tmp );
  return *tmp;
}

std::vector<std::shared_ptr<BL::Channel>>&
BL::Bar::GetBgmChannels( void )
{
  return bgm_channels_;
}


BL::Channel&
BL::Bar::MakeNewBgmChannel( void )
{
  auto tmp = std::shared_ptr<Channel>( new Channel( "01"_hex36, resolution_ ) );
  bgm_channels_.push_back( tmp );
  return *tmp;
}


// -- SimpleIterator -----------------------------------------------------
BL::Bar::SimpleIterator::SimpleIterator( Bar& bar, bool is_end ) :
bar_( bar ),
bgm_it_( is_end ? bar_.bgm_channels_.end() : bar_.bgm_channels_.begin() ),
channels_it_( is_end ? bar_.channels_.end() : bar_.channels_.begin() )
{
}

BL::Channel&
BL::Bar::SimpleIterator::operator *( void )
{
  if ( bgm_it_ != bar_.bgm_channels_.end() ) {
    return **bgm_it_;
  }
  else {
    return *(*channels_it_).second;
  }
}

void
BL::Bar::SimpleIterator::operator ++( void )
{
  if ( bgm_it_ != bar_.bgm_channels_.end() ) {
    ++bgm_it_;
  }
  else {
    ++channels_it_;
  }
}

bool
BL::Bar::SimpleIterator::operator !=( SimpleIterator& other )
{
  return (this->bgm_it_ != other.bgm_it_) || (this->channels_it_ != other.channels_it_);
}


BL::Bar::SimpleIterator
BL::Bar::begin( void )
{
  return SimpleIterator( *this, false );
}

BL::Bar::SimpleIterator
BL::Bar::end( void )
{
  return SimpleIterator( *this, true );
}
