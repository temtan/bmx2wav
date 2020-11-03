// direct_sound_stream.h

#pragma once

#include <optional>
#include <functional>

#include <dsound.h>

#include "tt_window.h"
#include "tt_thread.h"


namespace BMX2WAV::Core {
  class Wave;
}

namespace BMX2WAV::Core::DirectSoundStream {

  // -- DataReader -------------------------------------------------------
  class DataReader {
  public:
    virtual unsigned int GetSegment( char* buffer, unsigned int size ) = 0;
    virtual bool IsEnd( void ) = 0;
  };

  // -- Player -----------------------------------------------------------
  class Player {
  private:
    // -- PlayThread -----------------------------------------------------
    class PlayThread : public TtFunctionThread {
    public:
      explicit PlayThread( std::function<void ( void )> function );
      void PlayStop( void );

    public:
      std::unique_ptr<Player> player_;
    };

  public:
    static void PlayOggFileAsync( TtWindow& parent, const std::string& path, unsigned int volume = 100 );
    static void PlayWavFileAsync( TtWindow& parent, const std::string& path, unsigned int volume = 100 );
    static void PlayWaveAsync( TtWindow& parent, std::shared_ptr<Core::Wave> wave, unsigned int volume = 100 );
    static void PlayStop( void );

  private:
    static void PlayAsync( std::function<void ( void )> thread_function );
    static std::optional<PlayThread> PLAY_THREAD;

  public:
    explicit Player( void );
    ~Player();

    void Initialize( TtWindow& parent );
    void InitializeByWaveFormat( TtWindow& parent, WAVEFORMATEX& wave_format );
    void Play( DataReader& reader, unsigned int volume = 100 );

    void Stop( void );

  private:
    IDirectSound8*       ds8_;
    WAVEFORMATEX         wave_format_;
    DSBUFFERDESC         ds_buffer_description_;
    IDirectSoundBuffer*  temp_buffer_;
    IDirectSoundBuffer8* buffer_;
    bool                 do_stop_;
  };
}
