// direct_sound_stream.cpp

#include "tt_windows_h_include.h"
#include <mmsystem.h>

#include "ttl_define.h"

#include "wave_maker.h"

#include "direct_sound_stream.h"

#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )
#pragma comment ( lib, "winmm.lib" )

using namespace BMX2WAV;


// -- PlayThread ---------------------------------------------------------
Core::DirectSoundStream::Player::PlayThread::PlayThread( std::function<void ( void )> function ) :
TtFunctionThread( function ),
player_( new Player() )
{
}


void
Core::DirectSoundStream::Player::PlayThread::PlayStop( void )
{
  player_->Stop();
}


// -- Player -------------------------------------------------------------
void
Core::DirectSoundStream::Player::PlayOggFileAsync( TtWindow& parent, const std::string& path, unsigned int volume )
{
  Player::PlayAsync( [&parent, path, volume] ( void ) -> void {
    OggFileHolder                    holder( path );
    DirectSoundStream::OggFileReader reader( holder );

    PLAY_THREAD->player_->Initialize( parent );
    PLAY_THREAD->player_->Play( reader, volume );
  } );
}

void
Core::DirectSoundStream::Player::PlayWavFileAsync( TtWindow& parent, const std::string& path, unsigned int volume )
{
  Player::PlayAsync( [&parent, path, volume] ( void ) -> void {
    WAVEFORMATEX wave_format;

    HMMIO    mmio_handle = NULL;
    MMIOINFO mmio_info = {0};

    mmio_handle = ::mmioOpen( const_cast<LPSTR>( path.c_str() ), &mmio_info, MMIO_READ );
    if ( NOT( mmio_handle ) ) {
      return;
    }
    TtUtility::DestructorCall mmio_handle_closer( [mmio_handle] ( void ) {
      ::mmioClose( mmio_handle, 0 );
    } );

    MMRESULT result;

    // RIFFチャンク検索
    MMCKINFO riff_chunk;
    riff_chunk.fccType = mmioFOURCC( 'W', 'A', 'V', 'E' );
    result = ::mmioDescend( mmio_handle, &riff_chunk, NULL, MMIO_FINDRIFF );
    if( result != MMSYSERR_NOERROR ) {
      return;
    }

    // フォーマットチャンク検索
    MMCKINFO format_chunk;
    format_chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    result = ::mmioDescend( mmio_handle, &format_chunk, &riff_chunk, MMIO_FINDCHUNK );
    if( result != MMSYSERR_NOERROR ) {
      return;
    }

    DWORD fmsize = format_chunk.cksize;
    DWORD size   = ::mmioRead( mmio_handle, (HPSTR)&wave_format, fmsize );
    if( size != fmsize ) {
      return;
    }

    ::mmioAscend( mmio_handle, &format_chunk, 0 );

    // データチャンク検索
    MMCKINFO data_chunk;
    data_chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    result = ::mmioDescend( mmio_handle, &data_chunk, &riff_chunk, MMIO_FINDCHUNK );
    if( result != MMSYSERR_NOERROR ) {
      return;
    }

    class WaveFileReader : public DataReader {
    public:
      explicit WaveFileReader( HMMIO mmio_handle, unsigned int size  ) : mmio_handle_( mmio_handle ), remain_size_( size )  {}

      virtual unsigned int GetSegment( char* buffer, unsigned int size ) override {
        auto ret = ::mmioRead( mmio_handle_, buffer, std::min( remain_size_, static_cast<int>( size ) ) );
        remain_size_ -= ret;
        return ret;
      }

      virtual bool IsEnd( void ) override {
        return remain_size_ <= 0;
      }

    private:
      HMMIO mmio_handle_;
      int   remain_size_;
    };

    WaveFileReader reader( mmio_handle, data_chunk.cksize );

    PLAY_THREAD->player_->InitializeByWaveFormat( parent, wave_format );
    PLAY_THREAD->player_->Play( reader, volume );
  } );
}

void
Core::DirectSoundStream::Player::PlayWaveAsync( TtWindow& parent, std::shared_ptr<Core::Wave> wave, unsigned int volume )
{
  Player::PlayAsync( [&parent, wave, volume] ( void ) -> void {
    WaveReader reader( *wave );

    PLAY_THREAD->player_->Initialize( parent );
    PLAY_THREAD->player_->Play( reader, volume );
  } );
}

void
Core::DirectSoundStream::Player::PlayStop( void )
{
  if ( PLAY_THREAD ) {
    PLAY_THREAD->PlayStop();
  }
}

void
Core::DirectSoundStream::Player::PlayAsync( std::function<void ( void )> thread_function )
{
  if ( PLAY_THREAD && NOT( PLAY_THREAD->HasExited() ) ) {
    PLAY_THREAD->PlayStop();
    PLAY_THREAD->Join();
  }
  PLAY_THREAD.emplace( thread_function );
  PLAY_THREAD->Start();
}

std::optional<Core::DirectSoundStream::Player::PlayThread>
Core::DirectSoundStream::Player::PLAY_THREAD;



Core::DirectSoundStream::Player::Player( void ) :
ds8_( nullptr ),
wave_format_( {0} ),
ds_buffer_description_( {0} ),
temp_buffer_( nullptr ),
buffer_( nullptr )
{
}

Core::DirectSoundStream::Player::~Player()
{
  if ( buffer_ ) {
    buffer_->Release();
  }
  if ( temp_buffer_ ) {
    temp_buffer_->Release();
  }
  if ( ds8_ ) {
    ds8_->Release();
  }
}

void
Core::DirectSoundStream::Player::Initialize( TtWindow& parent )
{
  wave_format_.wFormatTag      = WAVE_FORMAT_PCM;
  wave_format_.nChannels       = 2;
  wave_format_.nSamplesPerSec  = 44100;
  wave_format_.wBitsPerSample  = 16;
  wave_format_.nBlockAlign     = wave_format_.nChannels * 16 / 8;
  wave_format_.nAvgBytesPerSec = wave_format_.nSamplesPerSec * wave_format_.nBlockAlign;
  wave_format_.cbSize          = 0;
  this->InitializeByWaveFormat( parent, wave_format_ );
}

void
Core::DirectSoundStream::Player::InitializeByWaveFormat( TtWindow& parent, WAVEFORMATEX& wave_format )
{
  DirectSoundCreate8( NULL, &ds8_, NULL );
  ds8_->SetCooperativeLevel( parent.GetHandle(), DSSCL_PRIORITY );

  wave_format_ = wave_format;

  ds_buffer_description_.dwSize          = sizeof( DSBUFFERDESC );
  ds_buffer_description_.dwFlags         = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
  ds_buffer_description_.dwBufferBytes   = wave_format_.nAvgBytesPerSec;
  ds_buffer_description_.dwReserved      = 0;
  ds_buffer_description_.lpwfxFormat     = &wave_format_;
  ds_buffer_description_.guid3DAlgorithm = GUID_NULL;

  auto ret = ds8_->CreateSoundBuffer( &ds_buffer_description_, &temp_buffer_, NULL );
  if ( NOT( SUCCEEDED( ret ) ) ) {
    ds8_->Release();
    ds8_ = nullptr;
    throw TtWindowsSystemCallException( "DirectSoundCreate8::CreateSoundBuffer", ret, __FILE__, __LINE__ );
  }
  temp_buffer_->QueryInterface( IID_IDirectSoundBuffer8 , (void**)&buffer_ );
}


void
Core::DirectSoundStream::Player::Play( DataReader& reader, unsigned int volume )
{
  unsigned int finish_point = 0;
  auto lock_and_read_to_buffer = [this, &reader, &finish_point] ( DWORD offset, DWORD size ) {
    LPVOID can_write_pointer;
    LPVOID second_pointer;
    DWORD  length;
    DWORD  second_length;
    auto ret = buffer_->Lock( offset, size, &can_write_pointer, &length, &second_pointer, &second_length, 0 );

    if ( NOT( SUCCEEDED( ret ) ) ) {
      throw TtWindowsSystemCallException( "IDirectSoundBuffer8::Lock", ret, __FILE__, __LINE__ );
    }
    auto write_byte = reader.GetSegment( static_cast<char*>( can_write_pointer ), length );
    if ( reader.IsEnd() ) {
      finish_point = offset + write_byte;
      if ( ds_buffer_description_.lpwfxFormat->wBitsPerSample == 8 ) {
        for ( unsigned int i = 0; i < (length - write_byte); ++i ) {
          (static_cast<char*>( can_write_pointer ) + write_byte )[i] = -128;
        }
      }
      else {
        std::memset( static_cast<char*>( can_write_pointer ) + write_byte, 0, length - write_byte );
      }
    }
    buffer_->Unlock( can_write_pointer, length, second_pointer, second_length );
  };

  unsigned int buffer_size = ds_buffer_description_.dwBufferBytes;
  unsigned int half_buffer_size = buffer_size / 2;
  bool next_is_first_half = false;

  lock_and_read_to_buffer( 0, half_buffer_size );
  buffer_->SetVolume( static_cast<int>( std::log10( static_cast<double>( std::min( volume, static_cast<unsigned int>( 100 ) ) ) / 100.0 ) * 2000.0 ) );
  buffer_->Play( 0, 0, DSBPLAY_LOOPING );

  do_stop_ = false;
  for ( DWORD previous_point = 0; ;) {
    ::Sleep( 20 );
    DWORD current_point;
    buffer_->GetCurrentPosition( &current_point, 0 );

    if ( do_stop_ ) {
      buffer_->Stop();
      break;
    }
    if ( reader.IsEnd() ) {
      if ( ( current_point >= previous_point &&
             ( previous_point <= finish_point && current_point >= finish_point ) ) ||
           ( current_point < previous_point &&
             ( previous_point <= finish_point || current_point >= finish_point ) ) ) {
        buffer_->Stop();
        break;
      }
    }
    else {
      if ( current_point >= half_buffer_size && next_is_first_half ) {
        lock_and_read_to_buffer( 0, half_buffer_size );
        next_is_first_half = false;
      }
      else if ( current_point < half_buffer_size && NOT( next_is_first_half ) ) {
        lock_and_read_to_buffer( half_buffer_size, buffer_size );
        next_is_first_half = true;
      }
    }
    previous_point = current_point;
  }
}


void
Core::DirectSoundStream::Player::Stop( void )
{
  do_stop_ = true;
}
