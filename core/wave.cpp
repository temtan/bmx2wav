// core/wave.cpp

#include <time.h>

#include <algorithm>
#include <map>

#include "vorbis/vorbisenc.h"

#include "ttl_define.h"
#include "tt_time.h"

#include "exception.h"
#include "utility.h"

#include "core/wave.h"

#define OPPOSITE( i ) ((-1 * (i)) - 1)

using namespace BMX2WAV;


// -- Wave::Tick ---------------------------------------------------------
Core::Wave::Tick::Tick( void ) :
Tick( 0 )
{
}

Core::Wave::Tick::Tick( int64_t center ) :
Tick( center, center )
{
}

Core::Wave::Tick::Tick( int64_t left, int64_t right ) :
left_( left ),
right_( right )
{
}

void
Core::Wave::Tick::Change( double ratio )
{
  auto change = [&] ( int64_t& target ) {
    double changed = static_cast<double>( target );
    changed *= ratio;
    if ( changed > static_cast<double>( std::numeric_limits<int64_t>::max() ) ) {
      target = std::numeric_limits<int64_t>::max();
    }
    else if ( changed < static_cast<double>( std::numeric_limits<int64_t>::min() ) ) {
      target = std::numeric_limits<int64_t>::min();
    }
    else {
      target = static_cast<int64_t>( changed );
    }
  };
  change( left_ );
  change( right_ );
}

void
Core::Wave::Tick::Mixin( const Tick& other )
{
  /* 性能向上の為、チェック廃止
  auto positive_overflow = [] ( const int64_t& x, const int64_t& y ) -> bool {
    return ( x > 0 ) ? (y > std::numeric_limits<int64_t>::max() - x) : false;
  };
  auto negative_overflow = [] ( const int64_t& x, const int64_t& y ) -> bool {
    return ( x < 0 ) ? (std::numeric_limits<int64_t>::min() - x > y ) : false;
  };
  auto mixin = [&] ( int64_t& x, const int64_t& y ) -> void {
    x = (positive_overflow( x, y )) ? std::numeric_limits<int64_t>::max() : (negative_overflow( x, y )) ? std::numeric_limits<int64_t>::min() : x + y;
  };
  mixin( left_, other.left_ );
  mixin( right_, other.right_ );
   */
  left_ += other.left_;
  right_ += other.right_;
}

void
Core::Wave::Tick::Deduct( const Tick& other )
{
  /* 性能向上の為、チェック廃止
  auto positive_overflow = [] ( const int64_t& x, const int64_t& y ) -> bool {
    return ( y < 0 ) ? (x > std::numeric_limits<int64_t>::max() + y ) : false;
  };
  auto negative_overflow = [] ( const int64_t& x, const int64_t& y ) -> bool {
    return ( y > 0 ) ? (std::numeric_limits<int64_t>::min() + y > x) : false;
  };
  auto deduct = [&] ( int64_t& x, const int64_t& y ) -> void {
    x = (positive_overflow( x, y )) ? std::numeric_limits<int64_t>::max() : (negative_overflow( x, y )) ? std::numeric_limits<int64_t>::min() : x - y;
  };
  deduct( left_, other.left_ );
  deduct( right_, other.right_ );
   */
  left_ -= other.left_;
  right_ -= other.right_;
}


// -- Wave ---------------------------------------------------------------
Core::Wave::Wave( void ) :
data_(),
opposite_data_()
{
}


unsigned int
Core::Wave::GetLength( void ) const
{
  return this->GetPositiveLength() + this->GetNegativeLength();
}

unsigned int
Core::Wave::GetPositiveLength( void ) const
{
  return static_cast<unsigned int>( data_.size() );
}

unsigned int
Core::Wave::GetNegativeLength( void ) const
{
  return static_cast<unsigned int>( opposite_data_.size() );
}


Core::Wave::Tick&
Core::Wave::At( int index )
{
  return index < 0 ? opposite_data_[OPPOSITE( index )] : data_[index];
}

const Core::Wave::Tick&
Core::Wave::At( int index ) const
{
  return index < 0 ? opposite_data_[OPPOSITE( index )] : data_[index];
}

bool
Core::Wave::HasValue( int index ) const
{
  if ( index < 0 ) {
    return OPPOSITE( index ) < this->GetNegativeLength();
  }
  else {
    return index < this->GetPositiveLength();
  }
}

int
Core::Wave::GetFrontPosition( void ) const
{
  return (-1) * this->GetNegativeLength();
}


void
Core::Wave::ChangeVolume( double ratio )
{
  for ( Tick& tick : *this ) {
    tick.Change( ratio );
  }
}

void
Core::Wave::MixinAt( int start, const Wave& other, int other_start )
{
  this->SynthesizeAt( start, other, other_start, &Tick::Mixin );
}

void
Core::Wave::DeductAt( int start, const Wave& other, int other_start )
{
  this->SynthesizeAt( start, other, other_start, &Tick::Deduct );
}

void
Core::Wave::SynthesizeAt( int start, const Wave& other, int other_start, TickOperation operation )
{
  if ( other_start >= other.GetPositiveLength() ) {
    return;
  }
  if ( start + other.GetPositiveLength() - other_start > this->GetPositiveLength() ) {
    data_.resize( start + other.GetPositiveLength() );
  }
  if ( start < 0 && std::abs( start ) >= static_cast<int>( this->GetNegativeLength() ) ) {
    opposite_data_.resize( std::abs( start ) );
  }

  for ( int i = 0;; ++i ) {
    if ( NOT( other.HasValue( other_start + i ) ) ) {
      if ( other_start + i < 0 ) {
        continue;
      }
      break;
    }
    (this->At( start + i ).*operation)( other.At( other_start + i ) );
  }
}


double
Core::Wave::PeakNormalize( void )
{
  int64_t max = 0;
  for ( Tick& tick : *this ) {
    max = std::max( {std::abs( tick.left_ ), std::abs( tick.right_ ), max} );
  }

  if ( max == 0 ) {
    return 1.0;
  }

  double ratio = static_cast<double>( Tick::MaxValue ) / static_cast<double>( max );
  this->ChangeVolume( ratio );
  return ratio;
}

double
Core::Wave::AverageNormalize( void )
{
  double sum = 0.0;
  unsigned int count = 0;
  for ( Tick& tick : *this ) {
    sum += static_cast<double>( std::abs( tick.left_ ) );
    sum += static_cast<double>( std::abs( tick.right_ ) );
    ++count;
  }

  if ( sum == 0 || count == 0 ) {
    return 1.0;
  }

  double ratio = (
    (static_cast<double>( Tick::MaxValue ) / 10.0) /
    (sum / static_cast<double>( count * 2 )) );
  this->ChangeVolume( ratio );
  return ratio;
}

double
Core::Wave::GetOverNormalizeRatio( double over_ratio )
{
  auto length = static_cast<unsigned int>( std::ceil( static_cast<double>( this->GetLength() ) * 2.0 * over_ratio ) );
  std::list<uint64_t> top_list( length, 0 );

  auto tmp = [&] ( uint64_t value ) {
    if ( value > top_list.back() ) {
      for ( auto it = top_list.begin(); it != top_list.end(); ++it ) {
        if ( *it <= value ) {
          top_list.insert( it, value );
          top_list.pop_back();
          break;
        }
      }
    }
  };
  for ( Tick& tick : *this ) {
    tmp( std::abs( tick.left_ ) );
    tmp( std::abs( tick.right_ ) );
  }

  return static_cast<double>( Tick::MaxValue ) / static_cast<double>( top_list.back() );
}


double
Core::Wave::OverNormalize( double over_ratio )
{
  double ratio = this->GetOverNormalizeRatio( over_ratio );
  this->ChangeVolume( ratio );
  return ratio;
}

double
Core::Wave::OverNormalizeOld2( double over_ratio )
{
  std::unordered_map<uint64_t, unsigned int> table;
  unsigned int all_count = 0;
  for ( Tick& tick : *this ) {
    table[std::abs( tick.left_ )] += 1;
    table[std::abs( tick.right_ )] += 1;
    all_count += 2;
  }
  if ( all_count == 0 ) {
    return 1.0;
  }

  uint64_t new_max = 0;
  unsigned int over_count = 0;
  std::vector<std::pair<uint64_t, unsigned int>> v;
  v.reserve( table.size() );
  for ( auto& one : table ) {
    v.push_back( one );
  }
  std::sort( v.begin(), v.end(), [] ( auto& x, auto& y ) { return x.first < y.first; } );

  for ( auto it = v.rbegin(); it != v.rend(); ++it ) {
    new_max = it->first;
    over_count += it->second;
    if ( over_count > all_count * over_ratio ) {
      break;
    }
  }
  if ( new_max == 0 ) {
    return 1.0;
  }

  double ratio = static_cast<double>( Tick::MaxValue ) / static_cast<double>( new_max );
  this->ChangeVolume( ratio );
  return ratio;

}

double
Core::Wave::OverNormalizeOld( double over_ratio )
{
  // 単純なアルゴリズムだけど遅い
  std::map<uint64_t, unsigned int> table;
  unsigned int all_count = 0;
  for ( Tick& tick : *this ) {
    table[std::abs( tick.left_ )] += 1;
    table[std::abs( tick.right_ )] += 1;
    all_count += 2;
  }
  if ( all_count == 0 ) {
    return 1.0;
  }

  uint64_t new_max = 0;
  unsigned int over_count = 0;
  for ( auto it = table.rbegin(); it != table.rend(); ++it ) {
    new_max = it->first;
    over_count += it->second;
    if ( over_count > all_count * over_ratio ) {
      break;
    }
  }
  if ( new_max == 0 ) {
    return 1.0;
  }

  double ratio = static_cast<double>( Tick::MaxValue ) / static_cast<double>( new_max );
  this->ChangeVolume( ratio );
  return ratio;
}


void
Core::Wave::Trim( int start, int end )
{
  if ( start > end ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }

  std::vector<Tick> new_data;
  std::vector<Tick> new_opposite_reverse_data;
  for ( int i = start; i <= end; ++i ) {
    if ( i < 0 ) {
      if ( OPPOSITE( i ) < static_cast<int>( this->GetNegativeLength() ) ) {
        new_opposite_reverse_data.push_back( opposite_data_[OPPOSITE( i )] );
      }
      else {
        new_opposite_reverse_data.push_back( Tick( 0, 0 ) );
      }
    }
    else {
      if ( i < static_cast<int>( this->GetPositiveLength() ) ) {
        new_data.push_back( data_[i] );
      }
      else {
        new_data.push_back( Tick( 0, 0 ) );
      }
    }
  }
  data_ = new_data;
  opposite_data_.clear();
  for ( auto it = new_opposite_reverse_data.rbegin(); it != new_opposite_reverse_data.rend(); ++it ) {
    opposite_data_.push_back( *it );
  }
}

void
Core::Wave::TrimFrontSilence( void )
{
  int start = this->GetFrontPosition();
  while ( start < this->GetPositiveLength() ) {
    if ( this->At( start ).left_ != 0 || this->At( start ).right_ != 0 ) {
      break;
    }
    start += 1;
  }
  if ( start < static_cast<int>( this->GetPositiveLength() ) - 1 ) {
    this->Trim( start, this->GetPositiveLength() - 1 );
  }
}


void
Core::Wave::InsertFrontSilence( unsigned int length )
{
  this->ExtendTo( -1 * (length + this->GetNegativeLength() ) );
}

void
Core::Wave::InsertFrontSilenceBySecond( double second )
{
  this->InsertFrontSilence( static_cast<unsigned int>( second * Wave::FREQUENCY ) );
}


void
Core::Wave::FadeIn( int start, int end, double ratio )
{
  if ( start > end ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }

  unsigned int count = 0;
  for ( int i = start; i <= end; ++i ) {
    double tmp = ratio + ((1.0 - ratio) * static_cast<double>( count ) / static_cast<double>( end - start ));
    if ( i < 0 && OPPOSITE( i ) < static_cast<int>( this->GetNegativeLength() ) ) {
      opposite_data_[OPPOSITE( i )].Change( tmp );
    }
    if ( i >= 0 && i < static_cast<int>( this->GetPositiveLength() ) ) {
      data_[i].Change( tmp );
    }
    count++;
  }
}

void
Core::Wave::FadeOut( int start, int end, double ratio )
{
  if ( start > end ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }

  unsigned int count = 0;
  for ( int i = start; i <= end; ++i ) {
    double tmp = 1.0 - ((1.0 - ratio) * static_cast<double>( count ) / static_cast<double>( end - start ) );
    if ( i < 0 && OPPOSITE( i ) < static_cast<int>( this->GetNegativeLength() ) ) {
      opposite_data_[OPPOSITE( i )].Change( tmp );
    }
    if ( i >= 0 && i < static_cast<int>( this->GetPositiveLength() ) ) {
      data_[i].Change( tmp );
    }
    count++;
  }
}


void
Core::Wave::Reserve( unsigned int size )
{
  data_.reserve( size );
}

void
Core::Wave::ExtendTo( int size )
{
  if ( size < 0 && std::abs( size ) > this->GetNegativeLength() ) {
    opposite_data_.resize( std::abs( size ) );
  }
  else if ( size > this->GetPositiveLength() ) {
    data_.resize( size );
  }
}


void
Core::Wave::Clear( void )
{
  data_.clear();
  opposite_data_.clear();
}

void
Core::Wave::WriteToFile( const std::string& path )
{
  if ( NOT( Utility::MakeDirectoryOfFileIfNotExist( path ) ) ) {
    throw AudioFileOpenException( path, ENOENT );
  }

  FILE* file;
  if ( int error_number = ::fopen_s( &file, path.c_str(), "wb" ); error_number != 0 ) {
    throw AudioFileOpenException( path, error_number );
  }
  TtUtility::DestructorCall closer( [&] ( void ) { std::fclose( file ); } );

  auto write_2byte = [&] ( short data ) {
    if ( std::fwrite( reinterpret_cast<char*>( &data ), 1, sizeof( short ), file ) != sizeof( short ) ) {
      throw WavFileWriteException( path, errno );
    }
  };
  auto write_integer = [&] ( int data ) {
    if ( std::fwrite( reinterpret_cast<char*>( &data ), 1, sizeof( int ), file ) != sizeof( int ) ) {
      throw WavFileWriteException( path, errno );
    }
  };

  write_integer( Wave::RIFF_HEADER );
  // file size
  // 36byte はヘッダ情報など
  write_integer( this->GetLength() * 2 * 2 + 36 );
  write_integer( Wave::WAVE_HEADER );
  write_integer( Wave::FMT_CHUNK );
  write_integer( Wave::FMT_BYTE_SIZE );
  write_2byte( Wave::FMT_ID );
  // channel count
  write_2byte( 2 );
  // frequency
  write_integer( Wave::FREQUENCY );
  // byte per second
  write_integer( Wave::FREQUENCY * 2 * 2 );
  // block size
  write_2byte( 4 );
  // bit rate
  write_2byte( 16 );
  write_integer( Wave::DATA_CHUNK );
  write_integer( this->GetLength() * 2 * 2 );
  for ( Tick& tick : *this ) {
    // 内部的には「32bit範囲」扱いなので16bit化する
    write_2byte( static_cast<short>( std::clamp( tick.left_,  Tick::MinValue, Tick::MaxValue ) >> 16 ) );
    write_2byte( static_cast<short>( std::clamp( tick.right_, Tick::MinValue, Tick::MaxValue ) >> 16 ) );
  }
}


void
Core::Wave::WriteToFileAsOgg( const std::string path, float base_quality )
{
  int ret;

  FILE* file;
  if ( int error_number = ::fopen_s( &file, path.c_str(), "wb" ); error_number != 0 ) {
    throw AudioFileOpenException( path, error_number );
  }
  TtUtility::DestructorCall closer( [&] ( void ) { std::fclose( file ); } );

  {
    ::vorbis_info info;
    ::vorbis_info_init( &info );
    ret = ::vorbis_encode_init_vbr( &info, 2, Wave::FREQUENCY, base_quality );
    if ( ret ) {
      throw TT_SYSTEM_CALL_EXCEPTION( FUNC_NAME_OF( vorbis_encode_init_vbr ), 0 );
    }

    ::vorbis_dsp_state dsp_state;
    ::vorbis_analysis_init( &dsp_state, &info );

    ::ogg_stream_state stream_state;
    ::srand( static_cast<unsigned int>( ::time( NULL ) ) );
    ::ogg_stream_init( &stream_state, rand() );

    ::vorbis_comment comment;
    ::vorbis_comment_init( &comment );
    ::vorbis_comment_add_tag( &comment, "ENCODER", __FILE__ );

    {
      ::ogg_packet header;
      ::ogg_packet header_comm;
      ::ogg_packet header_code;

      ::vorbis_analysis_headerout( &dsp_state, &comment, &header, &header_comm, &header_code );
      ::ogg_stream_packetin( &stream_state, &header );
      ::ogg_stream_packetin( &stream_state, &header_comm );
      ::ogg_stream_packetin( &stream_state, &header_code );

      for (;;) {
        ::ogg_page page;
        int result = ::ogg_stream_flush( &stream_state, &page );
        if ( result == 0 ) {
          break;
        }
        std::fwrite( page.header, 1, page.header_len, file );
        std::fwrite( page.body,   1, page.body_len,   file );
      }
    }

    bool end_of_stream = false;
    int wav_pos = this->GetFrontPosition();
    ::vorbis_block block;
    ::vorbis_block_init( &dsp_state, &block );
    while ( !end_of_stream ) {
      int buffer_size = std::min<unsigned int>( 1024 * 4 + 44, this->GetPositiveLength() - wav_pos );
      float **buffer = ::vorbis_analysis_buffer( &dsp_state, buffer_size );
      int i = 0;
      while ( i < buffer_size ) {
        const int left = 0;
        const int right = 1;
        buffer[left][i]  = static_cast<float>( this->At( wav_pos ).left_ )  / static_cast<float>( Wave::Tick::MaxValue );
        buffer[right][i] = static_cast<float>( this->At( wav_pos ).right_ ) / static_cast<float>( Wave::Tick::MaxValue );
        ++wav_pos;
        ++i;
      }
      ::vorbis_analysis_wrote( &dsp_state, i );

      while ( ::vorbis_analysis_blockout( &dsp_state, &block ) == 1 ) {
        ::vorbis_analysis( &block, NULL );
        ::vorbis_bitrate_addblock( &block );

        ::ogg_packet packet;
        while ( ::vorbis_bitrate_flushpacket( &dsp_state, &packet ) ) {
          ::ogg_stream_packetin( &stream_state, &packet );

          while( !end_of_stream ) {
            ::ogg_page page;
            int result = ::ogg_stream_flush( &stream_state, &page );
            if ( result == 0 ) {
              break;
            }
            std::fwrite( page.header, 1, page.header_len, file );
            std::fwrite( page.body,   1, page.body_len,   file );
            if ( ::ogg_page_eos( &page ) ) {
              end_of_stream = true;
            }
          }
        }
      }
    }
    ::vorbis_block_clear( &block );
    ::vorbis_comment_clear( &comment );
    ::ogg_stream_clear( &stream_state );
    ::vorbis_dsp_clear( &dsp_state );
    ::vorbis_info_clear( &info );
  }
}


// -- SimpleIterator -----------------------------------------------------
Core::Wave::SimpleIterator::SimpleIterator( Wave& wave ) :
SimpleIterator( wave, OPPOSITE( wave.GetNegativeLength() - 1 ) )
{
}

Core::Wave::SimpleIterator::SimpleIterator( Wave& wave, int start ) :
wave_( wave ),
index_( start )
{
}


Core::Wave::Tick&
Core::Wave::SimpleIterator::operator *( void )
{
  return wave_.At( index_ );
}

void
Core::Wave::SimpleIterator::operator ++( void )
{
  index_++;
}

bool
Core::Wave::SimpleIterator::operator !=( SimpleIterator& other )
{
  return index_ != other.index_;
}


Core::Wave::SimpleIterator
Core::Wave::begin( void )
{
  return SimpleIterator( *this );
}

Core::Wave::SimpleIterator
Core::Wave::end( void )
{
  return SimpleIterator( *this, this->GetPositiveLength() );
}


// -- DirectSoundStream::WaveReader --------------------------------------
Core::DirectSoundStream::WaveReader::WaveReader( Wave& wave ) :
wave_( wave ),
position_( wave.GetFrontPosition() )
{
}


unsigned int
Core::DirectSoundStream::WaveReader::GetSegment( char* buffer, unsigned int size )
{
  unsigned int write_byte = 0;
  for (;;) {
    if ( this->IsEnd() ) {
      break;
    }
    if ( size - write_byte < 4 ) {
      break;
    }
    *(reinterpret_cast<short*>( buffer + write_byte )) = static_cast<short>( std::clamp( wave_.At( position_ ).left_, Wave::Tick::MinValue, Wave::Tick::MaxValue ) >> 16 );
    write_byte += 2;
    *(reinterpret_cast<short*>( buffer + write_byte )) = static_cast<short>( std::clamp( wave_.At( position_ ).right_, Wave::Tick::MinValue, Wave::Tick::MaxValue ) >> 16 );
    write_byte += 2;
    position_ += 1;
  }
  return write_byte;
}

bool
Core::DirectSoundStream::WaveReader::IsEnd( void )
{
  return position_ >= static_cast<int>( wave_.GetPositiveLength() );
}
