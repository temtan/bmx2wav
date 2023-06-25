// base/bmson_parser.h

#pragma once

#include <unordered_map>
#include <vector>

#include "tt_json.h"

#include "exception.h"

#include "base/bms_data.h"
#include "base/parser.h"


namespace BMX2WAV::BL::Bmson {
  // -- Exceptions -------------------------------------------------------
  // -- BmsonDescriptionException ----------------------------------------
  class BmsonDescriptionException : public Parser::ParseException {
  public:
    explicit BmsonDescriptionException( unsigned int line );

    unsigned int GetLine( void ) const;

  private:
    unsigned int line_;
  };

  // -- UnexpectedTokenException -----------------------------------------
  class UnexpectedTokenException : public BmsonDescriptionException {
  public:
    explicit UnexpectedTokenException( TtJson::UnexpectedTokenException& origin );

    char GetToken( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    char token_;
  };

  // -- NumberFormatException --------------------------------------------
  class NumberFormatException : public BmsonDescriptionException {
  public:
    explicit NumberFormatException( TtJson::NumberFormatException& origin );

    virtual std::string GetMessage( void ) override;
  };
  
  // -- UnicodeFormatException -------------------------------------------
  class UnicodeFormatException : public BmsonDescriptionException {
  public:
    explicit UnicodeFormatException( TtJson::UnicodeFormatException& origin );

    virtual std::string GetMessage( void ) override;
  };

  // -- BadCastException -------------------------------------------------
  class BadCastException : public BmsonDescriptionException {
  public:
    explicit BadCastException( TtJson::BadCastException& origin );

    const type_info& GetExpectedType( void ) const;
    const type_info& GetActualType( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    const type_info& expected_;
    const type_info& actual_;
  };

  // -- RequiredKeyIsNothingException ------------------------------------
  class RequiredKeyIsNothingException : public BmsonDescriptionException {
  public:
    explicit RequiredKeyIsNothingException( TtJson::HashTable& table, const std::string& key );

    const std::string& GetRequiredKey( void );

    virtual std::string GetMessage( void ) override;

  private:
    std::string required_key_;
  };
}

namespace BMX2WAV {
  // -- ConvertBmsonDescriptionException --------------------------------------------
  class ConvertBmsonDescriptionException : public ConvertException {
  public:
    explicit ConvertBmsonDescriptionException( BL::Bmson::BmsonDescriptionException& origin );

    unsigned int GetLine( void ) const;

    virtual std::string GetMessage( void ) override;

  private:
    unsigned int line_;
    std::string  message_;
  };
}


namespace BMX2WAV::BL::Bmson {
  // -- BmsonData --------------------------------------------------------
  struct BmsonData {
    // -- data class -----
    struct Information {
      std::string              title_;
      std::string              sub_title_ = "";
      std::string              artist_;
      std::vector<std::string> sub_artists_;
      std::string              genre_;
      std::string              mode_hint_ = "beat-7k";
      std::string              chart_name_;
      uint64_t                 level_;
      double                   init_bpm_;
      double                   judge_rank_ = 100.0;
      double                   total_ = 100.0;
      std::string              back_image_;
      std::string              eyecatch_image_;
      std::string              banner_image_;
      std::string              preview_music_;
      uint64_t                 resolution_ = 240;
    };

    struct BarLine {
      uint64_t position_;
    };

    struct Note {
      unsigned int lane_;
      uint64_t     position_;
      uint64_t     length_;
      bool         continuation_flag_;
    };

    struct SoundChannel {
      std::string       name_;
      std::vector<Note> notes_;
    };

    struct BpmEvent {
      uint64_t position_;
      double   bpm_;
    };

    struct StopEvent {
      uint64_t position_;
      uint64_t duration_;
    };

    struct BgaHeader {
      uint64_t    id_;
      std::string name_;
    };

    struct BgaEvent {
      uint64_t position_;
      uint64_t id_;
    };

    struct Bga {
      std::vector<BgaHeader> bga_header_;
      std::vector<BgaEvent>  bga_events_;
      std::vector<BgaEvent>  layer_events_;
      std::vector<BgaEvent>  poor_events_;
    };

    // -- member -----
    std::string               version_;
    Information               info_;
    std::vector<BarLine>      lines_;
    std::vector<BpmEvent>     bpm_events_;
    std::vector<StopEvent>    stop_events_;
    std::vector<SoundChannel> sound_channels_;
    Bga                       bga_;
  };

  // -- JsonToBmsonDataConverter -----------------------------------------
  class JsonToBmsonDataConverter {
  public:
    explicit JsonToBmsonDataConverter( void );

    BmsonData Convert( TtJson::Value& root );

  private:
    template <class TYPE>
    auto HashTableGetValueAs( TtJson::HashTable& table, const std::string& key );

    template <class TYPE>
    void IfHashTableContainsKeyDoValueAs( TtJson::HashTable& table, const std::string& key, std::function<void ( TYPE& )> func );

    void ProcessRoot( TtJson::HashTable& root );
    void ProcessInfo( TtJson::HashTable& info );
    void ProcessLines( TtJson::Array& lines );
    void ProcessBpmEvents( TtJson::Array& bpm_events );
    void ProcessStopEvents( TtJson::Array& stop_events );
    void ProcessSoundChannels( TtJson::Array& sound_channels );
    void ProcessNotes( TtJson::Array& notes, std::vector<BmsonData::Note>& v );
    void ProcessBga( TtJson::HashTable& bga );
    void ProcessBgaHeader( TtJson::Array& bga_header );
    void ProcessBgaEvents( TtJson::Array& bga_events, std::vector<BmsonData::BgaEvent>& v );

  private:
    BmsonData bmson_;
  };

  // -- Parser -----------------------------------------------------------
  class Parser {
  public:
    explicit Parser( void );

    // throw FileAccessException
    std::shared_ptr<BmsonData> Parse( const std::string& path );

    // partial
    std::shared_ptr<TtJson::Value> FileToJsonData( const std::string& path );
    std::shared_ptr<BmsonData>     JsonDataToBmsonData( TtJson::Value& json_data );
  };
}
