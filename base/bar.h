// base/bar.h

#pragma once

#include <unordered_map>
#include <memory>

#include "base/word.h"
#include "base/channel.h"


namespace BMX2WAV::BL {
  // -- Bar --------------------------------------------------------------
  class Bar {
  public:
    explicit Bar( void );

    unsigned int GetResolution( void ) const;
    double       GetRatio( void ) const;

    void SetRatio( double ratio );

    void MultiplyResolution( unsigned int multiplier );

    bool ContainsAnyObject( void ) const;
    bool Emtpy( void ) const;

    unsigned int GetObjectCountOf( std::function<bool ( BL::Word channel_number )> should_count_channel ) const;

    // channel 01 ÇÕêÊì™ÇÃÇ›
    BL::Channel& GetChannelBy( BL::Word channel_number );

    std::vector<std::shared_ptr<BL::Channel>>& GetBgmChannels( void );

    BL::Channel& MakeNewBgmChannel( void );

    // -- SimpleIterator -----
    class SimpleIterator {
    public:
      explicit SimpleIterator( Bar& bar, bool is_end );

      BL::Channel& operator *( void );
      void operator ++( void );
      bool operator !=( SimpleIterator& other );

    private:
      Bar& bar_;
      std::vector<std::shared_ptr<BL::Channel>>::iterator                  bgm_it_;
      std::unordered_map<BL::Word, std::shared_ptr<BL::Channel>>::iterator channels_it_;
    };
    SimpleIterator begin( void );
    SimpleIterator end( void );

  private:
    unsigned int resolution_;
    double       ratio_;
    std::vector<std::shared_ptr<BL::Channel>>                  bgm_channels_;
    std::unordered_map<BL::Word, std::shared_ptr<BL::Channel>> channels_;
  };
}
