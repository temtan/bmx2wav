// core/bmson_converter.h

#pragma once

#include "base/bms_data.h"
#include "base/bmson_parser.h"

#include "exception.h"


namespace BMX2WAV::Core {
  // -- BmsonConverter ---------------------------------------------------
  class BmsonConverter {
  public:
    explicit BmsonConverter( void );

    std::shared_ptr<BL::BmsData> ConvertForWaveConvert( BL::Bmson::BmsonData& bmson );
    std::shared_ptr<BL::BmsData> ConvertForWaveConvertFromFile( const std::string& path );

    void ConvertToFileAndWave( BL::Bmson::BmsonData& original, const std::string& input_file, const std::string& output_dir );
  };
}
