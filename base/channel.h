// base/channel.h

#pragma once

#include <memory>
#include <vector>

#include "base/word.h"


namespace BMX2WAV::BL {
  // -- Buffer -----------------------------------------------------------
  class Buffer {
  public:
    explicit Buffer( void );
    explicit Buffer( unsigned int length );

    unsigned int GetLength( void ) const;
    std::vector<BL::Word>& GetArray( void );

    BL::Word& operator []( unsigned int position );
    const BL::Word& operator []( unsigned int position ) const;

    void ZeroClearAt( unsigned int position );
    void ZeroClearAll( void );

    // don't overwrite
    void Merge( const BL::Buffer& buffer );
    void Merge( unsigned int start, const BL::Buffer& buffer );

    bool Contains( BL::Word word ) const;
    bool ContainsAnyObject( void ) const;
    bool Empty( void ) const;

    unsigned int GetObjectCount( void ) const;

    // unsafety
    void Resize( size_t size );
    void MagnifyBy( unsigned int multiplier );

    std::string ToString( void );

    // -- Iterator -----
    using Iterator = std::vector<BL::Word>::iterator;
    Iterator begin( void );
    Iterator end( void );

  private:
    void IfPositionIsOverTheLengthThenRaiseError( unsigned int pos ) const;

  private:
    std::vector<BL::Word> array_;
  };

  // -- Channel ----------------------------------------------------------
  class Channel : public Buffer {
  public:
    static bool NumberIsWavChannel( BL::Word number );
    static bool NumberIsShoudPlayChannel( BL::Word number );
    static bool NumberIsBmpChannel( BL::Word number );
    static bool NumberIsBpmChangeChannel( BL::Word number );
    static bool NumberIsInvisibilityObjectChannel( BL::Word number );
    static bool NumberIsExtendedBpmChangeChannel( BL::Word number );
    static bool NumberIsLongNoteChannel( BL::Word number );
    static bool NumberIsStopSequenceChannel( BL::Word number );
    static bool NumberIsPlayerNoteChannel( BL::Word number );
    static bool NumberIsFirstPlayerNoteChannel( BL::Word number );
    static bool NumberIsSecondPlayerNoteChannel( BL::Word number );
    static bool NumberIsLandmineObjectChannel( BL::Word number );

  private:
    friend class Bar;
    explicit Channel( BL::Word number );
    explicit Channel( BL::Word number, unsigned int length );

  public:
    BL::Word GetChannelNumber( void ) const;
    bool IsWavChannel( void ) const;
    bool IsShoudPlayChannel( void ) const;
    bool IsBmpChannel( void ) const;
    bool IsBpmChangeChannel( void ) const;
    bool IsInvisibilityObjectChannel( void ) const;
    bool IsExtendedBpmChangeChannel( void ) const;
    bool IsLongNoteChannel( void ) const;
    bool IsStopSequenceChannel( void ) const;
    bool IsPlayerNoteChannel( void ) const;
    bool IsFirstPlayerNoteChannel( void ) const;
    bool IsSecondPlayerNoteChannel( void ) const;
    bool IsLandmineObjectChannel( void ) const;

  private:
    BL::Word number_;
  };
}
