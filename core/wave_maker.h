// core/wave_maker.h

#pragma once

#include <stdio.h>

#include "vorbis/vorbisfile.h"

#include "core/wave.h"


namespace BMX2WAV::Core {
  // -- DataBuffer ----------------------------------------------------
  template <class TYPE>
  class DataBuffer {
  public:
    explicit DataBuffer( unsigned int size );
    void Extend( unsigned int new_size );

    unsigned int            size_;
    std::unique_ptr<TYPE[]> data_;
  };

  // -- OggFileHolder ----------------------------------------------------
  class OggFileHolder {
  public:
    explicit OggFileHolder( const std::string& path );
    ~OggFileHolder();

    const std::string path_;
    FILE*             file_;
    OggVorbis_File    vorbis_file_;
  };


  // -- DataHolder -------------------------------------------------------
  class DataHolder {
  public:
    virtual bool    DataRemains( void ) = 0;
    virtual int64_t ReadOneData( void ) = 0;
  };

  // -- WavDataHolder ----------------------------------------------------
  class WavDataHolder : public DataHolder {
  public:
    explicit WavDataHolder( DataBuffer<unsigned char>& buffer );
    virtual bool DataRemains( void ) override;

  protected:
    DataBuffer<unsigned char>& buffer_;
    unsigned int               current_;
  };

  // -- EightBitWavDataHoler ---------------------------------------------
  class EightBitWavDataHoler : public WavDataHolder {
  public:
    using WavDataHolder::WavDataHolder;
    virtual int64_t ReadOneData( void ) override;
  };

  // -- SixteenBitWavDataHolder ------------------------------------------
  class SixteenBitWavDataHolder : public WavDataHolder {
  public:
    using WavDataHolder::WavDataHolder;
    virtual int64_t ReadOneData( void ) override;
  };

  // -- TwentyFourBitWavDataHolder ---------------------------------------
  class TwentyFourBitWavDataHolder : public WavDataHolder {
  public:
    using WavDataHolder::WavDataHolder;
    virtual int64_t ReadOneData( void ) override;
  };

  // -- SixteenBitOggDataHolder ------------------------------------------
  class SixteenBitOggDataHolder : public DataHolder {
  public:
    explicit SixteenBitOggDataHolder( OggFileHolder& ogg_file_holder );

    virtual bool    DataRemains( void ) override;
    virtual int64_t ReadOneData( void ) override;

  private:
    void PreRead( void );

  private:
    OggFileHolder&   ogg_file_holder_;
    DataBuffer<char> buffer_;
    unsigned int     current_;
    unsigned int     last_read_size_;
  };


  // -- ChannelReader ----------------------------------------------------
  class ChannelReader {
  public:
    explicit ChannelReader( std::shared_ptr<DataHolder> data_holder );

    bool DataRemains( void );

    virtual Core::Wave::Tick ReadTick( void ) = 0;

  protected:
    std::shared_ptr<DataHolder> data_holder_;
  };

  // -- OneChannelReader -------------------------------------------------
  class OneChannelReader : public ChannelReader {
  public:
    using ChannelReader::ChannelReader;
    virtual Core::Wave::Tick ReadTick( void ) override;
  };

  // -- TwoChannelReader -------------------------------------------------
  class TwoChannelReader : public ChannelReader {
  public:
    using ChannelReader::ChannelReader;
    virtual Core::Wave::Tick ReadTick( void ) override;
  };


  // -- ButterworthTwoOrderFilter ----------------------------------------
  class ButterworthTwoOrderFilter {
  public:
    explicit ButterworthTwoOrderFilter( unsigned int cutoff_frequency );

    Core::Wave::Tick Input( int64_t left, int64_t right );
    Core::Wave::Tick Input( const Core::Wave::Tick& tick );

  private:
    double a1_;
    double a2_;
    double b0_;
    struct {
      double z0_;
      double z1_;
      double z2_;
    } left_, right_;
  };


  // -- WaveMaker --------------------------------------------------------
  class WaveMaker {
  public:
    explicit WaveMaker( bool overlook_error );

    std::shared_ptr<Wave> MakeNewWaveFromPathAutoExtension( const std::string& path );

    std::shared_ptr<Wave> MakeNewWaveFromWavFile( const std::string& path );
    std::shared_ptr<Wave> MakeNewWaveFromOggFile( const std::string& path );

  private:
    std::shared_ptr<ChannelReader> CreateChannelReader( DataBuffer<unsigned char>& buffer, int channel_count, int bit_rate );
    std::shared_ptr<ChannelReader> SelectByChannel( std::shared_ptr<DataHolder> data_holder, int channel_count );

    void ReadFromChannelReader( Wave& wave, ChannelReader& reader, int frequency );

  private:
    bool overlook_error_;
  };
}


// -- DirectSoundStream --------------------------------------------------
#include "direct_sound_stream.h"

namespace BMX2WAV::Core::DirectSoundStream {

  // -- OggFileReader ----------------------------------------------------
  class OggFileReader : public DataReader {
  public:
    explicit OggFileReader( OggFileHolder& holder );

    virtual unsigned int GetSegment( char* buffer, unsigned int size ) override;
    virtual bool IsEnd( void ) override;

  private:
    OggFileHolder& holder_;
    bool           is_end_;
  };
}
