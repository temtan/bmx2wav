// core/wave_maker.cpp

#define _USE_MATH_DEFINES
#include <math.h>

#include <map>

#include "ttl_define.h"
#include "tt_exception.h"
#include "tt_string.h"
#include "tt_path.h"

#include "utility.h"
#include "exception.h"

#include "core/wave_maker.h"

using namespace BMX2WAV;


namespace {
  // -- WavFileReader ----------------------------------------------------
  class WavFileReader {
  public:
    explicit WavFileReader( const std::string& path );
    ~WavFileReader();

    int ReadByte( void );
    int Read2Byte( void );
    int ReadInteger( void );
    void Read( unsigned char* buffer, size_t buffer_size );

    void Skip( unsigned int offset );

  private:
    const std::string path_;
    FILE* file_;
  };
}

WavFileReader::WavFileReader( const std::string& path ) :
path_( path ),
file_( NULL )
{
  if ( int error_number = ::fopen_s( &file_, path.c_str(), "rb" ); error_number != 0 ) {
    throw AudioFileOpenException( path, error_number );
  }
}


WavFileReader::~WavFileReader()
{
  std::fclose( file_ );
}

int
WavFileReader::ReadByte( void )
{
  unsigned char buf = 0x00;
  if ( std::fread( reinterpret_cast<char*>( &buf ), sizeof( char ), 1, file_ ) != 1 ) {
    throw WavFileReadException( path_, errno );
  }
  return static_cast<int>( buf );
}

int
WavFileReader::Read2Byte( void )
{
  signed short int buf = 0x0000;
  if ( std::fread( reinterpret_cast<char*>( &buf ), sizeof( char ), 2, file_ ) != 2  ) {
    throw WavFileReadException( path_, errno );
  }
  return static_cast<int>( buf );
}

int
WavFileReader::ReadInteger( void )
{
  signed int buf = 0x00000000;
  if ( std::fread( reinterpret_cast<char*>( &buf ), sizeof( char ), 4, file_ ) != 4 ) {
    throw WavFileReadException( path_, errno );
  }
  return static_cast<int>( buf );
}

void
WavFileReader::Read( unsigned char* buffer, size_t buffer_size )
{
  if ( std::fread( buffer, sizeof( unsigned char ), buffer_size, file_ ) != buffer_size ) {
    throw WavFileReadException( path_, errno );
  }
}


void
WavFileReader::Skip( unsigned int offset )
{
  if ( std::fseek( file_, offset, SEEK_CUR ) != 0 ) {
    throw WavFileReadException( path_, errno );
  }
}


// -- DataBuffer ---------------------------------------------------------
template<class TYPE>
Core::DataBuffer<TYPE>::DataBuffer( unsigned int size ) :
size_( size ),
data_( std::make_unique<TYPE[]>( size ) )
{
}

template<class TYPE>
void
Core::DataBuffer<TYPE>::Extend( unsigned int new_size )
{
  if ( new_size > size_ ) {
    data_ = std::make_unique<TYPE[]>( new_size );
    size_ = new_size;
  }
}


// -- OggFileHolder ------------------------------------------------------
Core::OggFileHolder::OggFileHolder( const std::string& path ) :
path_( path ),
file_( NULL )
{
  if ( int error_number = fopen_s( &file_, path.c_str(), "rb" ); error_number != 0 ) {
    throw AudioFileOpenException( path, error_number );
  }
  if ( int ret = ::ov_open( file_, &vorbis_file_, NULL, 0 ); ret < 0 ) {
    std::fclose( file_ );
    throw OggFileOpenException( path, ret );
  }
}

Core::OggFileHolder::~OggFileHolder()
{
  ::ov_clear( &vorbis_file_ );
  if ( file_ != NULL ) {
    std::fclose( file_ );
  }
}


// -- WavDataHolder ------------------------------------------------------
Core::WavDataHolder::WavDataHolder( DataBuffer<unsigned char>& buffer ) :
buffer_( buffer ),
current_( 0 )
{
}

bool
Core::WavDataHolder::DataRemains( void )
{
  return current_ < buffer_.size_;
}

// -- EightBitWavDataHoler -----------------------------------------------
int64_t
Core::EightBitWavDataHoler::ReadOneData( void )
{
  if ( current_ > buffer_.size_ - 1 ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  int64_t tmp = static_cast<int64_t>( buffer_.data_[current_] );
  current_ += 1;
  return (tmp - 128) << 24;
}

// -- SixteenBitWavDataHolder --------------------------------------------
int64_t
Core::SixteenBitWavDataHolder::ReadOneData( void )
{
  if ( current_ + 1 > buffer_.size_ - 1 ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  signed char tmp[2];
  tmp[0] = buffer_.data_[current_];
  tmp[1] = buffer_.data_[current_ + 1];
  current_ += 2;
  return static_cast<int64_t>( *reinterpret_cast<short*>( tmp ) ) << 16;
}

// -- TwentyFourBitWavDataHolder -----------------------------------------
int64_t
Core::TwentyFourBitWavDataHolder::ReadOneData( void )
{
  if ( current_ + 2 > buffer_.size_ - 1 ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  signed char tmp[4];
  tmp[0] = buffer_.data_[current_];
  tmp[1] = buffer_.data_[current_ + 1];
  tmp[2] = buffer_.data_[current_ + 2];
  tmp[3] = static_cast<signed char>( tmp[2] & 0x80 ? 0xFF : 0x00 );
  current_ += 3;
  return static_cast<int64_t>( *reinterpret_cast<int*>( tmp ) ) << 8;
}

// -- SixteenBitOggDataHolder --------------------------------------------
Core::SixteenBitOggDataHolder::SixteenBitOggDataHolder( OggFileHolder& ogg_file_holder ) :
ogg_file_holder_( ogg_file_holder ),
buffer_( 1024 * 16 ),
current_( 0 ),
last_read_size_( 0 )
{
  this->PreRead();
}

bool
Core::SixteenBitOggDataHolder::DataRemains( void )
{
  return last_read_size_ > 0;
}

int64_t
Core::SixteenBitOggDataHolder::ReadOneData( void )
{
  char tmp[2];
  if ( current_ >= last_read_size_ ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  if ( current_ + 1 != last_read_size_ ) {
    tmp[0] = buffer_.data_[current_];
    tmp[1] = buffer_.data_[current_ + 1];
    current_ += 2;
  }
  else {
    tmp[0] = buffer_.data_[current_];
    this->PreRead();
    if ( NOT( this->DataRemains() ) ) {
      throw OggFileReadException( ogg_file_holder_.path_ );
    }
    tmp[1] = buffer_.data_[current_];
    current_ += 1;
  }
  if ( current_ >= last_read_size_ ) {
    this->PreRead();
  }
  return static_cast<int64_t>( *reinterpret_cast<short*>( tmp ) ) << 16;
}

void
Core::SixteenBitOggDataHolder::PreRead( void )
{
  int bitstream;
  last_read_size_ = ::ov_read( &ogg_file_holder_.vorbis_file_,
                               buffer_.data_.get(),
                               buffer_.size_,
                               0, 2, 1, &bitstream );
  current_ = 0;
}


// -- ChannelReader ------------------------------------------------------
Core::ChannelReader::ChannelReader( std::shared_ptr<DataHolder> data_holder ) :
data_holder_( data_holder )
{
}

bool
Core::ChannelReader::DataRemains( void )
{
  return data_holder_->DataRemains();
}

// -- OneChannelReader ---------------------------------------------------
Core::Wave::Tick
Core::OneChannelReader::ReadTick( void )
{
  return Core::Wave::Tick( data_holder_->ReadOneData() );
}

// -- TwoChannelReader ---------------------------------------------------
Core::Wave::Tick
Core::TwoChannelReader::ReadTick( void )
{
  int64_t left  = data_holder_->ReadOneData();
  int64_t right = data_holder_->ReadOneData();
  return Core::Wave::Tick( left, right );
}



// -- ButterworthTwoOrderFilter ------------------------------------------
Core::ButterworthTwoOrderFilter::ButterworthTwoOrderFilter( unsigned int cutoff_frequency )
{
  const double Q = M_SQRT2;
  const double T = 1.0 / static_cast<double>( Core::Wave::FREQUENCY );
  const double omega = 2.0 * M_PI * cutoff_frequency;

  left_  = {0.0, 0.0, 0.0};
  right_ = {0.0, 0.0, 0.0};

  const double denominator = 4 * Q + 2 * T * omega + Q * T * T * omega * omega;
  a1_ = (8 * Q - 2 * Q * T * T * omega * omega) / denominator;
  a2_ = (-(4 * Q - 2 * T * omega + Q * T * T * omega * omega))/ denominator;
  b0_ = (Q * T * T * omega * omega) / denominator;
}

Core::Wave::Tick
Core::ButterworthTwoOrderFilter::Input( int64_t left, int64_t right )
{
  return this->Input( Core::Wave::Tick( left, right ) );
}

Core::Wave::Tick
Core::ButterworthTwoOrderFilter::Input( const Core::Wave::Tick& tick )
{
  left_.z0_  = (tick.left_)  + (a1_ * left_.z1_)  + (a2_ * left_.z2_);
  right_.z0_ = (tick.right_) + (a1_ * right_.z1_) + (a2_ * right_.z2_);

  Core::Wave::Tick tmp = Core::Wave::Tick(
    static_cast<int64_t>( (b0_ * left_.z0_) +  (b0_ * 2.0 * left_.z1_)  + (b0_ * left_.z2_)  ),
    static_cast<int64_t>( (b0_ * right_.z0_) + (b0_ * 2.0 * right_.z1_) + (b0_ * right_.z2_) ) );

  left_.z2_  = left_.z1_;
  right_.z2_ = right_.z1_;
  left_.z1_  = left_.z0_;
  right_.z1_ = right_.z0_;

  return tmp;
}


// -- WaveMaker ----------------------------------------------------------
Core::WaveMaker::WaveMaker( bool overlook_error ) :
overlook_error_( overlook_error )
{
}

std::shared_ptr<Core::Wave>
Core::WaveMaker::MakeNewWaveFromPathAutoExtension( const std::string& path )
{
  std::string target = path;
  if ( NOT( TtString::EndWith( TtString::ToLower( target ), ".ogg" ) ) ) {
    if ( TtPath::FileExists( target ) ) {
      return this->MakeNewWaveFromWavFile( target );
    }
    target = TtPath::ChangeExtension( target, "ogg" );
  }
  return this->MakeNewWaveFromOggFile( target );
}

std::shared_ptr<Core::Wave>
Core::WaveMaker::MakeNewWaveFromWavFile( const std::string& path )
{
  WavFileReader reader( path );

  // RIFF header
  if ( reader.ReadInteger() != Wave::RIFF_HEADER && NOT( overlook_error_ ) ) {
    throw InvalidWavFileFormatException( path,  "RIFF header" );
  }

  // file size
  reader.ReadInteger();

  // WAVE header
  if ( reader.ReadInteger() != Wave::WAVE_HEADER && NOT( overlook_error_ ) ) {
    throw InvalidWavFileFormatException( path, "WAVE header" );
  }

  // fmt チャンクになるまで読み飛ばす
  for (;;) {
    if ( reader.ReadInteger() == Wave::FMT_CHUNK ) {
      break;
    }
    reader.Skip( reader.ReadInteger() );
  }

  // fmt byte size
  int fmt_byte_size = reader.ReadInteger();

  // fmt id
  if ( reader.Read2Byte() != Wave::FMT_ID ) {
    throw InvalidWavFileFormatException( path, "fmt id" );
  }

  // channel count
  int channel_count = reader.Read2Byte();
  if ( channel_count != 1 && channel_count != 2 ) {
    throw UnsupportedWavFileFormatException( path, "channel count is " + TtUtility::ToStringFrom( channel_count ) );
  }

  // frequency
  int frequency = reader.ReadInteger();

  // byte per second
  int byte_per_second = reader.ReadInteger();

  // block size
  int block_size = reader.Read2Byte();

  // bit rate
  int bit_rate = reader.Read2Byte();
  if ( bit_rate != 8 && bit_rate != 16 && bit_rate != 24 ) {
    throw UnsupportedWavFileFormatException( path, "bit rate is " + TtUtility::ToStringFrom( bit_rate ) );
  }

  // 細かいチェック
  if ( block_size != (bit_rate / 8) * channel_count && NOT( overlook_error_ ) ) {
    throw InvalidWavFileFormatException( path, "block size, bit rate, and channel count" );
  }
  if ( byte_per_second != frequency * block_size && NOT( overlook_error_ ) ) {
    throw InvalidWavFileFormatException( path, "byte per second, frequency, and block size" );
  }

  // 拡張部分の対応
  if ( fmt_byte_size - Wave::FMT_BYTE_SIZE > 0 ) {
    reader.Skip( fmt_byte_size - Wave::FMT_BYTE_SIZE );
  }

  // data チャンクになるまで読み飛ばす
  for (;;) {
    if ( reader.ReadInteger() == Wave::DATA_CHUNK ) {
      break;
    }
    reader.Skip( reader.ReadInteger() );
  }

  // data size
  int data_size = reader.ReadInteger();

  // data size のチェック
  if ( data_size % ((bit_rate / 8) * channel_count) != 0  ) {
    if ( NOT( overlook_error_ ) ) {
      throw InvalidWavFileFormatException( path, "data size can not be divided by block size" );
    }
    data_size -= data_size % ((bit_rate / 8) * channel_count );
  }

  std::shared_ptr<Wave> wave = std::make_shared<Wave>();
  wave->Reserve( static_cast<unsigned int>(
    static_cast<double>( data_size / channel_count / ( bit_rate / 8 ) ) *
    ( static_cast<double>( Wave::FREQUENCY ) / static_cast<double>( frequency ) ) ) );

  DataBuffer<unsigned char> buffer( data_size );
  reader.Read( buffer.data_.get(), buffer.size_ );

  auto channel_reader = this->CreateChannelReader( buffer, channel_count, bit_rate );

  this->ReadFromChannelReader( *wave, *channel_reader, frequency );
  return wave;
}

std::shared_ptr<Core::Wave>
Core::WaveMaker::MakeNewWaveFromOggFile( const std::string& path )
{
  OggFileHolder ogg_file_holder( path );
  vorbis_info* vorbis_info = ::ov_info( &ogg_file_holder.vorbis_file_, -1 );

  if ( vorbis_info == NULL ) {
    throw OggFileReadException( path );
  }
  if ( vorbis_info->channels != 1 && vorbis_info->channels != 2 ) {
    throw OggFileReadException( path );
  }

  std::shared_ptr<Wave> wave = std::make_shared<Wave>();

  auto data_holder = std::make_shared<SixteenBitOggDataHolder>( ogg_file_holder );

  auto channel_reader = this->SelectByChannel( data_holder, vorbis_info->channels );

  this->ReadFromChannelReader( *wave, *channel_reader, vorbis_info->rate );
  return wave;
}


std::shared_ptr<Core::ChannelReader>
Core::WaveMaker::CreateChannelReader( DataBuffer<unsigned char>& buffer, int channel_count, int bit_rate )
{
  switch ( bit_rate ) {
  case  8:
    return this->SelectByChannel( std::make_shared<EightBitWavDataHoler>( buffer ), channel_count );

  case 16:
    return this->SelectByChannel( std::make_shared<SixteenBitWavDataHolder>( buffer ), channel_count );

  case 24:
    return this->SelectByChannel( std::make_shared<TwentyFourBitWavDataHolder>( buffer ), channel_count );

  default:
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
}

std::shared_ptr<Core::ChannelReader>
Core::WaveMaker::SelectByChannel( std::shared_ptr<DataHolder> data_holder, int channel_count )
{
  switch ( channel_count ) {
  case 1:
    return std::shared_ptr<ChannelReader>( new OneChannelReader( data_holder ) );

  case 2:
    return std::shared_ptr<ChannelReader>( new TwoChannelReader( data_holder ) );

  default:
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
}


void
Core::WaveMaker::ReadFromChannelReader( Wave& wave, ChannelReader& reader, int frequency )
{
  auto filter = ButterworthTwoOrderFilter( frequency / 2 );
  auto median_of = [] ( const Core::Wave::Tick& x, const Core::Wave::Tick& y ) {
    return Core::Wave::Tick( ( x.left_ + y.left_ ) / 2, ( x.right_ + y.right_ ) / 2 );
  };

  switch ( frequency ) {
    // 44kHz 読み込み
  case Core::Wave::FREQUENCY: {
    while ( reader.DataRemains() ) {
      wave.data_.push_back( reader.ReadTick() );
    }
    break;
  }

    // 22kHz 読み込み
  case 22050: {
    Core::Wave::Tick prev1 = reader.ReadTick();
    Core::Wave::Tick prev2;
    wave.data_.push_back( filter.Input( prev1 ) );
    while ( reader.DataRemains() ) {
      Core::Wave::Tick current( reader.ReadTick() );

      wave.data_.push_back( filter.Input( median_of( prev1, current ) ) );
      wave.data_.push_back( filter.Input( current ) );

      prev2 = prev1;
      prev1 = current;
    }
    wave.data_.push_back( filter.Input( (prev1.left_  * 3 - prev2.left_ ) / 2,
                                        (prev1.right_ * 3 - prev2.right_) / 2 ) );
    break;
  }

    // 11kHz 読み込み
  case 11025: {
    Core::Wave::Tick prev1 = reader.ReadTick();
    Core::Wave::Tick prev2;
    wave.data_.push_back( filter.Input( prev1 ) );
    while ( reader.DataRemains() ) {
      Core::Wave::Tick current( reader.ReadTick() );
      Core::Wave::Tick tmp = median_of( prev1, current );

      wave.data_.push_back( filter.Input( median_of( prev1, tmp ) ) );
      wave.data_.push_back( filter.Input( tmp ) );
      wave.data_.push_back( filter.Input( median_of( tmp, current ) ) );
      wave.data_.push_back( filter.Input( current ) );

      prev2 = prev1;
      prev1 = current;
    }
    wave.data_.push_back( filter.Input( (prev1.left_  * 5 - prev2.left_ ) / 4,
                                        (prev1.right_ * 5 - prev2.right_) / 4 ) );
    wave.data_.push_back( filter.Input( (prev1.left_  * 3 - prev2.left_ ) / 2,
                                        (prev1.right_ * 3 - prev2.right_) / 2 ) );
    wave.data_.push_back( filter.Input( (prev1.left_  * 7 - prev2.left_  * 3) / 4,
                                        (prev1.right_ * 7 - prev2.right_ * 3) / 4 ) );
    break;
  }

    // 他周波数 読み込み
  default: {
    std::map<double, Core::Wave::Tick> origin;
    double origin_tick_step = 1.0 / static_cast<double>( frequency );

    for ( int i = 0; reader.DataRemains(); ++i ) {
      origin[origin_tick_step * static_cast<double>( i )] = reader.ReadTick();
    }

    auto it = origin.begin();
    filter.Input( origin[0.0] );
    std::pair<double, Core::Wave::Tick> prev = *it;
    ++it;
    std::pair<double, Core::Wave::Tick> next = *it;
    for ( unsigned int i = 1; ; ++i ) {
      double current = (1.0 / static_cast<double>( Core::Wave::FREQUENCY )) * static_cast<double>( i );
      if ( prev.first <= current && current <= next.first ) {
        wave.data_.push_back( filter.Input(
          static_cast<int64_t>(
            ( ( static_cast<double>( prev.second.left_ ) * ( next.first - current ) ) +
              ( static_cast<double>( next.second.left_ ) * ( current - prev.first ) ) )
            / origin_tick_step ),
          static_cast<int64_t>(
            ( ( static_cast<double>( prev.second.right_ ) * ( next.first - current ) ) +
              ( static_cast<double>( next.second.right_ ) * ( current - prev.first ) ) )
            / origin_tick_step ) ) );
      }
      else {
        prev = next;
        ++it;
        if ( it == origin.end() ) {
          break;
        }
        next = *it;
        // ループをやり直し
        --i;
        continue;
      }
    }
    break;
  }

  } // end switch
}



// -- DirectSoundStream::OggFileReader --------------------------------------
Core::DirectSoundStream::OggFileReader::OggFileReader( OggFileHolder& holder ) :
holder_( holder ),
is_end_( false )
{
}


unsigned int
Core::DirectSoundStream::OggFileReader::GetSegment( char* buffer, unsigned int size )
{
  unsigned int sum_read_size = 0;
  unsigned int request_size = 4096;
  for (;;) {
    int bitstream;
    unsigned int read_size = ::ov_read( &holder_.vorbis_file_, buffer + sum_read_size, request_size, 0, 2, 1, &bitstream );
    if ( read_size == 0 ) {
      is_end_ = true;
      return sum_read_size;
    }
    sum_read_size += read_size;

    if ( sum_read_size >= size ) {
      return sum_read_size;
    }
    request_size = std::min( request_size, size - sum_read_size );
  }
}

bool
Core::DirectSoundStream::OggFileReader::IsEnd( void )
{
  return is_end_;
}
