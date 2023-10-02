// base/bms_data.cpp

#include "exception.h"

#include "utility.h"

#include "base/bms_data.h"

using namespace BMX2WAV;

static int kazu = 0;

// -- BmsData ------------------------------------------------------------
BL::BmsData::BmsData( void ) :
headers_(),
wav_array_( "WAV" ),
bmp_array_( "BMP" ),
extended_bpm_array_( "BPM" ),
stop_sequence_array_( "STOP" ),
bars_(),

path_(),
most_serious_error_level_( ErrorLevel::None ),
has_random_statement_( false )
{
}


unsigned int
BL::BmsData::GetBarNumberLastObjectExists( void )
{
  for ( unsigned int i = Const::BAR_MAX_VALUE; i > 0; --i ) {
    if ( bars_[i].ContainsAnyObject() ) {
      return i;
    }
  }
  return 0;
}


unsigned int
BL::BmsData::GetObjectCount( void )
{
  return this->GetObjectCountOf( [] ( BL::Word ) { return true; } );
}

unsigned int
BL::BmsData::GetObjectCountOf( const std::vector<BL::Word>& should_count_channel )
{
  return this->GetObjectCountOf( [&] ( BL::Word channel_number ) -> bool {
    return TtUtility::Find( should_count_channel, channel_number ) != should_count_channel.end();
  } );
}

unsigned int
BL::BmsData::GetObjectCountOf( std::function<bool ( BL::Word channel_number )> should_count_channel )
{
  unsigned int sum = 0;
  for ( unsigned int i = Const::BAR_MAX_VALUE; i > 0; --i ) {
    sum += bars_[i].GetObjectCountOf( should_count_channel );
  }
  return sum;
}


unsigned int
BL::BmsData::GetObjectCountOf1P( void )
{
  return this->GetObjectCountOf( [] ( BL::Word channel_number ) -> bool {
    return BL::Channel::NumberIsFirstPlayerNoteChannel( channel_number );
  } );
}

unsigned int
BL::BmsData::GetObjectCountOf2P( void )
{
  return this->GetObjectCountOf( [] ( BL::Word channel_number ) -> bool {
    return BL::Channel::NumberIsSecondPlayerNoteChannel( channel_number );
  } );
}


unsigned int
BL::BmsData::GetMaxResolution( void )
{
  return bars_[this->GetBarNumberOfMaxResolution()].GetResolution();
}

unsigned int
BL::BmsData::GetBarNumberOfMaxResolution( void )
{
  Bar* max = &bars_[0];
  unsigned int max_number = 0;
  for ( unsigned int i = 0; auto& bar : bars_ ) {
    if ( bar.GetResolution() > max->GetResolution() ) {
      max = &bar;
      max_number = i;
    }
    i += 1;
  }
  return max_number;
}


double
BL::BmsData::CalculatePlayingTime( void )
{
  double sum = 0.0;

  // BPM ÇÃì«Ç›çûÇ›
  double current_bpm = 120.0;
  if ( auto it = headers_.find( "BPM" ); it != headers_.end() ) {
    if ( NOT( TtUtility::StringToDouble( it->second, &current_bpm ) ) || current_bpm == 0.0 ) {
      return std::numeric_limits<double>::min();
    }
  }

  std::unordered_map<BL::Word, double> extended_bpm_table;
  std::unordered_map<BL::Word, int>    stop_sequence_table;
  for ( BL::Word i( 0 ); i < Const::WORD_MAX_COUNT; i.Increase() ) {
    if ( extended_bpm_array_.IsExists( i ) ) {
      if ( double tmp; NOT( TtUtility::StringToDouble( extended_bpm_array_[i], &tmp ) ) || tmp == 0.0 ) {
        return std::numeric_limits<double>::min();
      }
      else {
        extended_bpm_table[i] = tmp;
      }
    }
    if ( stop_sequence_array_.IsExists( i ) ) {
      if ( int tmp; NOT( TtUtility::StringToInteger( stop_sequence_array_[i], & tmp ) ) ) {
        return std::numeric_limits<double>::min();
      }
      else {
        stop_sequence_table[i] = tmp;
      }
    }
  }

  unsigned int end_bar = this->GetBarNumberLastObjectExists();
  for ( unsigned int current_bar_number = 0; current_bar_number <= end_bar; ++current_bar_number ) {
    BL::Bar& current_bar = bars_[current_bar_number];

    for ( unsigned int position_of_bar = 0; position_of_bar < current_bar.GetResolution(); ++position_of_bar ) {
      int current_stop_sequence = 0;

      for ( BL::Channel& current_channel : current_bar ) {
        BL::Word current_word = current_channel[position_of_bar];
        if ( current_word == BL::Word::MIN ) {
          continue;
        }

        // BPMïœçX
        else if ( current_channel.IsBpmChangeChannel() ) {
          if ( int new_bpm; NOT( TtUtility::StringToInteger( current_word.ToCharPointer(), &new_bpm, 16 ) ) ) {
            return std::numeric_limits<double>::min();
          }
          else {
            current_bpm = static_cast<double>( new_bpm );
          }
        }
        // ägí£BPMïœçX
        else if ( current_channel.IsExtendedBpmChangeChannel() ) {
          if ( auto it = extended_bpm_table.find( current_word ); it == extended_bpm_table.end() ) {
            return std::numeric_limits<double>::min();
          }
          else {
            current_bpm = it->second;
          }
        }
        // STOP sequence
        else if ( current_channel.IsStopSequenceChannel() ) {
          if ( auto it = stop_sequence_table.find( current_word ); it == stop_sequence_table.end() ) {
            return std::numeric_limits<double>::min();
          }
          else {
            current_stop_sequence = stop_sequence_table[current_word];
          }
        }
      }
      sum += (60.0 / current_bpm) / (static_cast<double>( current_bar.GetResolution() ) / 4.0) * current_bar.GetRatio();
      if ( current_stop_sequence != 0 ) {
        sum += (60.0 / current_bpm ) * (static_cast<double>( current_stop_sequence ) / 192.0 * 4.0);
      }
    }
  }
  return sum;
}


std::string
BL::BmsData::ConvertToFileFormat( void )
{
  std::string result;
  auto add_result = [&] ( const std::string& str ) {
    result.append( str );
    result.append( "\n" );
  };
  auto add_as_header = [&] ( const std::string& key, const std::string& value ) {
    add_result( "#" + key + " " + value );
  };

  std::unordered_map<std::string, std::string> headers_clone = headers_;

  auto if_has_header_add_result = [&] ( const std::string& key ) {
    if ( headers_clone.contains( key ) ) {
      add_as_header( key, headers_clone[key] );
      headers_clone.erase( key );
    }
  };
  for ( unsigned int i = 0; i < Const::WORD_MAX_COUNT; ++i ) {
    headers_clone.erase( wav_array_.GetName()           + BL::Word( i ).ToString() );
    headers_clone.erase( bmp_array_.GetName()           + BL::Word( i ).ToString() );
    headers_clone.erase( extended_bpm_array_.GetName()  + BL::Word( i ).ToString() );
    headers_clone.erase( stop_sequence_array_.GetName() + BL::Word( i ).ToString() );
  }

  add_result( "\n*--- HEADER" );
  if_has_header_add_result( "PLAYER" );
  if_has_header_add_result( "GENRE" );
  if_has_header_add_result( "TITLE" );
  if_has_header_add_result( "ARTIST" );
  if_has_header_add_result( "BPM" );
  if_has_header_add_result( "PLAYLEVEL" );
  if_has_header_add_result( "RANK" );
  if_has_header_add_result( "TOTAL" );
  if_has_header_add_result( "STAGEFILE" );
  add_result( "" );
  for ( auto one : headers_clone ) {
    add_as_header( one.first, one.second );
  }
  add_result( "" );

  auto add_array = [&] ( BL::RegisterArray& array ) {
    for ( unsigned int i = 0; i < Const::WORD_MAX_COUNT; ++i ) {
      if ( array.IsExists( BL::Word( i ) ) ) {
        add_as_header( array.GetName() + BL::Word( i ).ToString(), array[BL::Word( i )] );
      }
    }
    add_result( "" );
  };
  add_array( wav_array_ );
  add_array( bmp_array_ );
  add_array( extended_bpm_array_ );
  add_array( stop_sequence_array_ );

  auto add_bar = [&] ( unsigned int bar_number, BL::Bar& bar ) {
    auto make_number_channel = [bar_number] ( BL::Word channel_number ) -> std::string {
      return Utility::Format( "#%03d%s:", bar_number, channel_number.ToCharPointer() );
    };

    if ( bar.Empty() ) {
      return;
    }
    if ( bar.GetRatio() != 1.0 ) {
      add_result( make_number_channel( "02"_hex36 ) + TtUtility::ToStringFrom( bar.GetRatio() ) );
    }
    for ( BL::Channel& channel : bar ) {
      BL::Channel channel_clone = channel;
      channel_clone.SafetyShrink();
      add_result( make_number_channel( channel_clone.GetChannelNumber() ) + channel_clone.ToString() );
    }
    add_result( "" );
  };
  for ( unsigned int i = 0; auto& bar : bars_ ) {
    add_bar( i, bar );
    ++i;
  }
  return result;
}
