// core/wave.h

#pragma once

#include <vector>


namespace BMX2WAV::Core {
  // -- Wave -------------------------------------------------------------
  class Wave {
    friend class WaveMaker;

  public:
    // -- Tick -----
    // Åu32bitîÕàÕÅvÇ™ê≥èÌÇ∆ÇµÇƒÇªÇÍÇ 64bit ïœêîÇ≈àµÇ§
    struct Tick {
    public:
      static const int64_t MaxValue = std::numeric_limits<int>::max();
      static const int64_t MinValue = std::numeric_limits<int>::min();

    public:
      explicit Tick( void );
      explicit Tick( int64_t center );
      explicit Tick( int64_t left, int64_t right );

      void Change( double ratio );
      void Mixin( const Tick& other );
      void Deduct( const Tick& other );

    public :
      int64_t left_;
      int64_t right_;
    };

    // -- Wave -----
  public:
    static const int RIFF_HEADER = 0x46464952; // *((int *)"RIFF");
    static const int WAVE_HEADER = 0x45564157; // *((int *)"WAVE");
    static const int FMT_CHUNK   = 0x20746D66; // *((int *)"fmt ");
    static const int DATA_CHUNK  = 0x61746164; // *((int *)"data");
    static const int FMT_BYTE_SIZE = 16;
    static const int FMT_ID        =  1;
    static const int FREQUENCY     = 44100;

  public:
    explicit Wave( void );

    unsigned int GetLength( void ) const;
    unsigned int GetPositiveLength( void ) const;
    unsigned int GetNegativeLength( void ) const;
    Tick& At( int index );
    const Tick& At( int index ) const;
    bool HasValue( int index ) const;
    int GetFrontPosition( void ) const;

    void ChangeVolume( double ratio );

    void MixinAt( int start, const Wave& other, int other_start = 0 );
    void DeductAt( int start, const Wave& other, int other_start = 0 );
  private:
    using TickOperation = void (Tick::*)( const Tick& );
    void SynthesizeAt( int start, const Wave& other, int other_start, TickOperation operation );

  public:
    // return value is changed ratio
    double PeakNormalize( void );
    double AverageNormalize( void );
    double GetOverNormalizeRatio( double over_ratio );
    double OverNormalize( double over_ratio );
    double OverNormalizeOld( double over_ratio );
    double OverNormalizeOld2( double over_ratio );

    void Trim( int start, int end );
    void TrimFrontSilence( void );

    void InsertFrontSilence( unsigned int length );
    void InsertFrontSilenceBySecond( double second );

    void FadeIn( int start, int end, double ratio = 0.0 );
    void FadeOut( int start, int end, double ratio = 0.0 );

    void Reserve( unsigned int size );
    void ExtendTo( int size );

    void Clear( void );

    void WriteToFile( const std::string& path );
    void WriteToFileAsOgg( const std::string path, float base_quality );

    // -- SimpleIterator -----
    class SimpleIterator {
    public:
      explicit SimpleIterator( Wave& wave );
      explicit SimpleIterator( Wave& wave, int start );

      Tick& operator *( void );
      void operator ++( void );
      bool operator !=( SimpleIterator& other );

    private:
      Wave& wave_;
      int   index_;
    };

  private:
    SimpleIterator begin( void );
    SimpleIterator end( void );

  private:
    std::vector<Tick> data_;
    std::vector<Tick> opposite_data_;
  };
}


// -- DirectSoundStream --------------------------------------------------
#include "direct_sound_stream.h"

namespace BMX2WAV::Core::DirectSoundStream {

  // -- WaveReader -------------------------------------------------------
  class WaveReader : public DataReader {
  public:
    explicit WaveReader( Wave& wave );

    virtual unsigned int GetSegment( char* buffer, unsigned int size ) override;
    virtual bool IsEnd( void ) override;

  private:
    Wave& wave_;
    int   position_;
  };
}
