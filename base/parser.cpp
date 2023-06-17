// base/parser.cpp

#include <optional>
#include <regex>
#include <numeric>
#include <random>

#include "ttl_define.h"
#include "tt_string.h"
#include "tt_exception.h"

#include "exception.h"
#include "utility.h"

#include "base/bmson_parser.h"

#include "base/parser.h"

using namespace BMX2WAV;


// -- Statement and other ------------------------------------------------
BL::Parser::Statement::Statement( std::shared_ptr<BL::Parser::RawLine> raw_line ) :
raw_line_( raw_line )
{
}

BL::Parser::Statement::~Statement()
{
}

void
BL::Parser::Statement::EvaluateBy( Parser& parser )
{
  NOT_USE( parser );
  throw BMX2WAV_INTERNAL_EXCEPTION;
}


// -- RandomStatement -----
void
BL::Parser::RandomStatement::EvaluateBy( Parser& parser )
{
  parser.frame_->random_value_stack_.push_back( this->Generate() );
}


// -- GenerateRandom -----
BL::Parser::GenerateRandom::GenerateRandom( std::shared_ptr<RawLine> raw_line, unsigned int max ) :
RandomStatement( raw_line ),
max_( max )
{
}

unsigned int
BL::Parser::GenerateRandom::Generate( void )
{
  std::random_device rd;
  std::mt19937 mt( rd() );
  return std::uniform_int_distribution<unsigned int>( 1, max_ )( mt );
}


// -- SetRandom -----
BL::Parser::SetRandom::SetRandom( std::shared_ptr<RawLine> raw_line, unsigned int value ) :
RandomStatement( raw_line ),
value_( value )
{
}

unsigned int
BL::Parser::SetRandom::Generate( void )
{
  return value_;
}


// -- EndRandom -----
void
BL::Parser::EndRandom::EvaluateBy( Parser& parser )
{
  if ( parser.frame_->random_value_stack_.size() <= 1 ) {
    parser.SafeErrorCallback<&Parser::Callbacks::no_beginning_end_random_>( *this );
    return;
  }
  else {
    parser.frame_->random_value_stack_.pop_back();
  }
}


// -- Block -----
BL::Parser::Block::Block( std::shared_ptr<RawLine> raw_line, Block* parent ) :
Statement( raw_line ),
parent_( parent ),
end_line_( nullptr ),
statements_()
{
}


void
BL::Parser::Block::EvaluateBy( Parser& parser )
{
  for ( auto statement : statements_ ) {
    statement->EvaluateBy( parser );
  }
}


// -- RootBlock -----
BL::Parser::RootBlock::RootBlock( void ) :
Block( nullptr, nullptr )
{
}

void
BL::Parser::RootBlock::RootEvaluation( Parser& parser )
{
  this->EvaluateBy( parser );
  for ( auto func : parser.frame_->lazy_eavluation_ ) {
    func( parser );
  }
}

// -- IfBlock -----
BL::Parser::IfBlock::IfBlock( std::shared_ptr<RawLine> raw_line, Block* parent, unsigned int prerequisite ) :
Block( raw_line, parent ),
prerequisite_( prerequisite ),
evaluation_type_( EvaluationType::Default )
{
}


void
BL::Parser::IfBlock::EvaluateBy( Parser& parser )
{
  parser.frame_->bms_data_.has_random_statement_ = true;
  switch ( evaluation_type_ ) {
  default:
  case EvaluationType::Default:
    if ( parser.frame_->random_value_stack_.back() == prerequisite_ ) {
      this->Block::EvaluateBy( parser );
    }
    break;

  case EvaluationType::MustApply:
    this->Block::EvaluateBy( parser );
    break;

  case EvaluationType::MustIgnore:
    // do nothing
    break;
  }
}

// -- Header -----
BL::Parser::Header::Header( std::shared_ptr<RawLine> raw_line, const std::string& key, const std::string& value ) :
Command( raw_line ),
key_( key ),
value_( value )
{
}

void
BL::Parser::Header::EvaluateBy( Parser& parser )
{
  if ( auto tmp = parser.frame_->bms_data_.headers_.find( TtString::ToUpper( key_ ) ); tmp != parser.frame_->bms_data_.headers_.end() ) {
    parser.SafeErrorCallback<&Parser::Callbacks::header_collision_>( *this );
    return;
  }
  else {
    parser.frame_->bms_data_.headers_[TtString::ToUpper( key_ )] = value_;
  }

  // BPM Check
  if ( TtString::ToUpper( key_ ) == "BPM" ) {
    if ( double tmp; NOT( TtUtility::StringToDouble( value_, &tmp ) ) || tmp == 0.0 ) {
      parser.SafeErrorCallback<&Parser::Callbacks::invalid_bpm_>( *this );
    }
  }

  // LNOBJ Check
  if ( TtString::ToUpper( key_ ) == "LNOBJ" ) {
    if ( value_.size() != 2 || NOT( BL::Word::CanConstructAsWord( value_ ) ) || BL::Word( value_ ) == BL::Word::MIN ) {
      parser.SafeErrorCallback<&Parser::Callbacks::invalid_lnobj_>( *this );
    }
    parser.frame_->lnobj_word_ = BL::Word( value_ );
  }

  // WAV Array
  if ( parser.frame_->bms_data_.wav_array_.CanApply( key_ ) ) {
    BL::Word pos = BL::Word( key_.substr( key_.size() - 2 ) );
    parser.frame_->bms_data_.wav_array_.GetArray()[pos.ToInteger()] = std::make_optional<std::string>( value_ );
    parser.frame_->wav_used_table_.insert_or_assign( pos, std::pair<Header&, bool>( *this, false ) );
  }

  // BMP Array
  if ( parser.frame_->bms_data_.bmp_array_.CanApply( key_ ) ) {
    BL::Word pos = BL::Word( key_.substr( key_.size() - 2 ) );
    parser.frame_->bms_data_.bmp_array_.GetArray()[pos.ToInteger()] = std::make_optional<std::string>( value_ );
    parser.frame_->bmp_used_table_.insert_or_assign( pos, std::pair<Header&, bool>( *this, false ) );
  }

  // BPM Array
  if ( parser.frame_->bms_data_.extended_bpm_array_.CanApply( key_ ) ) {
    BL::Word pos = BL::Word( key_.substr( key_.size() - 2 ) );
    parser.frame_->bms_data_.extended_bpm_array_.GetArray()[pos.ToInteger()] = std::make_optional<std::string>( value_ );
    parser.frame_->extended_bpm_used_table_.insert_or_assign( pos, std::pair<Header&, bool>( *this, false ) );
    if ( double tmp; NOT( TtUtility::StringToDouble( parser.frame_->bms_data_.extended_bpm_array_[pos], &tmp ) ) || tmp == 0.0 ) {
      parser.SafeErrorCallback<&Parser::Callbacks::invalid_extended_bpm_>( *this );
    }
  }

  // STOP Array
  if ( parser.frame_->bms_data_.stop_sequence_array_.CanApply( key_ ) ) {
    BL::Word pos = BL::Word( key_.substr( key_.size() - 2 ) );
    parser.frame_->bms_data_.stop_sequence_array_.GetArray()[pos.ToInteger()] = std::make_optional<std::string>( value_ );
    parser.frame_->stop_sequence_used_table_.insert_or_assign( pos, std::pair<Header&, bool>( *this, false ) );
    if ( int tmp; NOT( TtUtility::StringToInteger( parser.frame_->bms_data_.stop_sequence_array_[pos], & tmp ) ) ) {
      parser.SafeErrorCallback<&Parser::Callbacks::invalid_stop_sequence_>( *this );
    }
  }
}


// -- Channel -----
BL::Parser::Channel::Channel( std::shared_ptr<RawLine> raw_line, unsigned int bar_number, BL::Word channel_number ) :
Command( raw_line ),
bar_number_( bar_number ),
channel_number_( channel_number ),
objects_( 0 )
{
}


void
BL::Parser::Channel::EvaluateBy( Parser& parser )
{
  parser.frame_->lazy_eavluation_.push_back( [this] ( Parser& parser ) {
    BL::Bar& current_bar = parser.frame_->bms_data_.bars_[bar_number_];
    unsigned int new_resolution = std::lcm( current_bar.GetResolution(), objects_.GetLength() );
    if ( new_resolution > ( parser.bar_resolution_max_ == 0 ? Const::BAR_RESOLUTION_MAX : parser.bar_resolution_max_ ) ) {
      parser.SafeErrorCallback<&Parser::Callbacks::resolution_exceeded_limit_>( *this, new_resolution );
      throw Parser::AbortController();
    }

    parser.frame_->bms_data_.bars_[bar_number_].MultiplyResolution( new_resolution / current_bar.GetResolution() );
    parser.SafeCallback<&Parser::Callbacks::resolution_extended_>( *this, new_resolution );

    objects_.MagnifyBy( new_resolution / objects_.GetLength() );
    BL::Channel& channel = ( channel_number_ == "01"_hex36 ) ? current_bar.MakeNewBgmChannel() : current_bar.GetChannelBy( channel_number_ );
    for ( unsigned int i = 0; i < new_resolution; ++i ) {
      if ( objects_[i] == BL::Word::MIN ) {
        continue;
      }

      if ( channel[i] != BL::Word::MIN ) {
        parser.SafeErrorCallback<&Parser::Callbacks::object_collision_>( *this, channel[i], objects_[i] );
      }
      channel[i] = objects_[i];

      // -- defined check -----
      if ( channel.IsWavChannel() ) {
        if ( channel.IsPlayerNoteChannel() && objects_[i] == parser.frame_->lnobj_word_ ) {
          // LNOBJ 指定なら無視
        }
        else if ( parser.frame_->bms_data_.wav_array_.IsNotExists( objects_[i] ) ) {
          parser.SafeErrorCallback<&Parser::Callbacks::undefined_object_used_>( *this, objects_[i] );
        }
        else {
          parser.frame_->wav_used_table_.at( objects_[i] ).second = true;
        }
      }
      if ( channel.IsBmpChannel() ) {
        if ( parser.frame_->bms_data_.bmp_array_.IsNotExists( objects_[i] ) ) {
          parser.SafeErrorCallback<&Parser::Callbacks::undefined_object_used_>( *this, objects_[i] );
        }
        else {
          parser.frame_->bmp_used_table_.at( objects_[i] ).second = true;
        }
      }
      if ( channel.IsExtendedBpmChangeChannel() ) {
        if ( parser.frame_->bms_data_.extended_bpm_array_.IsNotExists( objects_[i] ) ) {
          parser.SafeErrorCallback<&Parser::Callbacks::undefined_object_used_>( *this, objects_[i] );
        }
        else {
          parser.frame_->extended_bpm_used_table_.at( objects_[i] ).second = true;
        }
      }
      if ( channel.IsStopSequenceChannel() ) {
        if ( parser.frame_->bms_data_.stop_sequence_array_.IsNotExists( objects_[i] ) ) {
          parser.SafeErrorCallback<&Parser::Callbacks::undefined_object_used_>( *this, objects_[i] );
        }
        else {
          parser.frame_->stop_sequence_used_table_.at( objects_[i] ).second = true;
        }
      }
    }
  } );
}


// -- BarLengthChange -----
BL::Parser::BarLengthChange::BarLengthChange( std::shared_ptr<RawLine> raw_line, unsigned int bar_number, double ratio ) :
Command( raw_line ),
bar_number_( bar_number ),
ratio_( ratio )
{
}

void
BL::Parser::BarLengthChange::EvaluateBy( Parser& parser )
{
  if ( parser.frame_->bar_length_changed_[bar_number_] ) {
    parser.SafeErrorCallback<&Parser::Callbacks::bar_length_change_collision_>( *this );
  }
  parser.frame_->bms_data_.bars_[bar_number_].SetRatio( ratio_ );
  parser.frame_->bar_length_changed_[bar_number_] = true;
}


// -- Parser -------------------------------------------------------------
BL::Parser::Parser::Parser( void ) :
must_read_as_utf8_( false ),
not_nesting_if_statement_( true ),
must_abort_error_level_( ErrorLevel::ImmediatelyAbort ),
bar_resolution_max_( Const::BAR_RESOLUTION_MAX ),

frame_( std::nullopt ),

callbacks_()
{
}

std::shared_ptr<BL::BmsData>
BL::Parser::Parser::Parse( const std::string& path )
{
  if ( TtString::EndWith( TtString::ToUpper( path ), ".BMSON" ) ) {
    return this->ParseAsBmson( path );
  }
  return this->ParseAsBms( path );
}

std::shared_ptr<BL::BmsData>
BL::Parser::Parser::ParseAsBms( const std::string& path )
{
  this->Initialize( path );

  try {
    try {
      this->ReadFile();
    }
    catch ( TtFileAccessException& ex ) {
      throw FileAccessException( ex );
    }

    this->RandomSyntacticAnalysis();
    this->SafeCallback<&Callbacks::complete_random_syntactic_analysis_>();

    this->StatementEvaluating();
  }
  catch ( AbortController ) {
    if ( callbacks_.aborted_ ) {
      callbacks_.aborted_( *this );
    }
  }

  this->Finalize();
  return std::make_shared<BmsData>( frame_->bms_data_ );
}

std::shared_ptr<BL::BmsData>
BL::Parser::Parser::ParseAsBmson( const std::string& path )
{
  Bmson::Parser parser;
  try {
    return parser.Parse( path );
  }
  catch ( TtFileAccessException& ex ) {
    throw FileAccessException( ex );
  }
}


void
BL::Parser::Parser::Initialize( const std::string& path )
{
  frame_.emplace();
  frame_->bms_data_.path_ = path;
}

void
BL::Parser::Parser::ReadFile( void )
{
  Utility::TextFileReader reader( frame_->bms_data_.path_ );
  if ( must_read_as_utf8_ ) {
    reader.read_as_utf8_ = true;
  }

  unsigned int line_number = 0;
  for (;;) {
    auto line = reader.ReadLineAutoEncode();
    if ( NOT( line ) ) {
      break;
    }
    line_number += 1;

    std::string striped = TtString::Strip( line.value() );
    RawLine::Type type = RawLine::Type::Undefined;
    std::string key;
    std::string value;

    const char* current = striped.c_str();

    if ( *current == '#' ) {
      ++current;

      if ( std::isdigit( *current ) ) {
        if ( std::isdigit( current[1] ) &&
             std::isdigit( current[2] ) &&
             std::isalnum( current[3] ) &&
             std::isalnum( current[4] ) &&
             current[5] == ':' ) {
          type = RawLine::Type::Channel;
          key = std::string( current, 5 );
          value = std::string( current + 6 );
        }
        else {
          RawLine raw_line = {line_number, line.value(), RawLine::Type::Undefined, "", ""};
          this->SafeErrorCallback<&Callbacks::invalid_channel_or_header_syntax_>( raw_line );
        }
      }
      else if ( *current != ' ' && *current != '\t' && *current != '\0' ) {
        const char* key_start = current;
        size_t      key_len   = 0;

        while ( *current != ' ' && *current != '\t' && *current != '\0' ) {
          key_len += 1;
          ++current;
        }
        while ( *current == ' ' || *current == '\t' ) {
          ++current;
        }
        type = RawLine::Type::Header;
        key = std::string( key_start, key_len );
        value = std::string( current );
      }
    }
    frame_->raw_data_.push_back( std::make_shared<RawLine>( RawLine( {line_number, line.value(), type, key, value} ) ) );
  }


  /* 性能の為、正規表現の使用を止め
  Utility::TextFileReader reader( frame_->bms_data_.path_ );

  unsigned int line_number = 0;

  std::regex channel_regex( R"(\#(\d\d\d[0-9a-zA-Z][0-9a-zA-Z])\:(.*))" );
  std::regex header_regex( R"(\#([^\t ]+)[\t ]*(.*))" );
  std::regex first_digit_regex( R"(^\#\d.*)" );
  for (;;) {
    auto line = reader.ReadLine();
    if ( NOT( line ) ) {
      break;
    }
    line_number += 1;

    std::string striped = TtString::Strip( line.value() );
    RawLine::Type type = RawLine::Type::Undefined;
    std::string key;
    std::string value;

    if ( std::smatch m; std::regex_match( striped, m, channel_regex ) ) {
      type = RawLine::Type::Channel;
      key = m.str( 1 );
      value = m.str( 2 );
    }
    else if ( std::regex_match( striped, m, first_digit_regex ) ) {
      RawLine raw_line = {line_number, line.value(), RawLine::Type::Undefined, "", ""};
      this->SafeErrorCallback( std::make_shared<InvalidChannelOrHeaderSyntaxException>( raw_line ), callbacks_.invalid_channel_or_header_syntax_, raw_line );
    }
    else if ( std::regex_match( striped, m, header_regex ) ) {
      type = RawLine::Type::Header;
      key = m.str( 1 );
      value = m.str( 2 );
    }
    frame_->raw_data_.push_back( std::make_shared<RawLine>( RawLine( {line_number, line.value(), type, key, value} ) ) );
  }
  */
}

void
BL::Parser::Parser::RandomSyntacticAnalysis( void )
{
  Block* current = &frame_->root_;
  for ( auto& line : frame_->raw_data_ ) {
    frame_->skip_parse_line_ = false;

    switch ( line->type_ ) {
    case RawLine::Type::Header: {
      if ( Utility::KeyEqual( line->key_, "RANDOM" ) ) {
        line->type_ = RawLine::Type::FlowControl;
        int max;
        if ( NOT( TtUtility::StringToInteger( line->value_, &max ) ) ) {
          this->SafeErrorCallback<&Callbacks::invalid_random_statement_value_>( *line );
          max = 1;
        }

        current->statements_.push_back( std::make_shared<GenerateRandom>( line, max ) );
      }
      else if ( Utility::KeyEqual( line->key_, "SETRANDOM" ) ) {
        line->type_ = RawLine::Type::FlowControl;
        int value;
        if ( NOT( TtUtility::StringToInteger( line->value_, &value ) ) ) {
          this->SafeErrorCallback<&Callbacks::invalid_random_statement_value_>( *line );
          value = 0;
        }

        current->statements_.push_back( std::make_shared<SetRandom>( line, value ) );
      }
      else if ( Utility::KeyEqual( line->key_, "ENDRANDOM" ) ) {
        line->type_ = RawLine::Type::FlowControl;

        current->statements_.push_back( std::make_shared<EndRandom>( line ) );
      }
      else if ( Utility::KeyEqual( line->key_, "IF" ) ) {
        line->type_ = RawLine::Type::FlowControl;

        if ( not_nesting_if_statement_ && current->parent_ != nullptr ) {
          current->end_line_ = line;
          current = current->parent_;
        }

        int prerequisite;
        if ( NOT( TtUtility::StringToInteger( line->value_, &prerequisite ) ) ) {
          this->SafeErrorCallback<&Callbacks::invalid_random_statement_value_>( *line );
          prerequisite = 1;
        }

        auto tmp = std::make_shared<IfBlock>( line, current, prerequisite );
        current->statements_.push_back( tmp );
        current = tmp.get();
      }
      else if ( Utility::KeyEqual( line->key_, "ENDIF" ) ) {
        line->type_ = RawLine::Type::FlowControl;
        if ( current->parent_ == nullptr ) {
          this->SafeErrorCallback<&Callbacks::corresponding_if_is_nothing_>( *line );
        }
        else {
          current->end_line_ = line;
          current = current->parent_;
        }
      }
      else {
        // 特定のヘッダは読込しないの実装
        this->SafeCallback<&Callbacks::header_detected_>( line->key_ );
        if ( frame_->skip_parse_line_ ) {
          continue;
        }
        current->statements_.push_back( std::make_shared<Header>( line, line->key_, line->value_ ) );
      }
    }
      break;

    case RawLine::Type::Channel: {
      int bar_number;
      if ( NOT( TtUtility::StringToInteger( line->key_.substr( 0, 3 ), &bar_number  ) ) ) {
        throw BMX2WAV_INTERNAL_EXCEPTION;
      }

      if ( NOT( BL::Word::CanConstructAsWord( line->key_.substr( 3, 2 ) ) ) ) {
        throw BMX2WAV_INTERNAL_EXCEPTION;
      }
      BL::Word channel_number( line->key_.substr( 3, 2 ) );

      // 特定のチャンネルは読込しないの実装
      this->SafeCallback<&Callbacks::channel_number_detected_>( channel_number );
      if ( frame_->skip_parse_line_ ) {
        continue;
      }

      if ( channel_number == "02"_hex36 ) {
        double ratio;
        if ( NOT( TtUtility::StringToDouble( line->value_, &ratio ) ) && ratio <= 0.0 ) {
          this->SafeErrorCallback<&Callbacks::invalid_bar_length_change_>( *line );
        }
        else {
          current->statements_.push_back( std::make_shared<BarLengthChange>( line, bar_number, ratio ) );
        }
      }
      else {
        auto tmp = std::make_shared<Channel>( line, bar_number, channel_number );
        auto tmp_func = [&line, &tmp] ( void ) -> bool {
          if ( line->value_.empty() ) {
            return false;
          }
          for ( const char* cp = line->value_.c_str(); cp[0] != '\0'; cp += 2 ) {
            if ( NOT( BL::Word::CanConstructAsWord( cp ) ) ) {
              return false;
            }
            tmp->objects_.GetArray().push_back( BL::Word( cp ) );
          }
          return true;
        };
        if ( tmp_func() ) {
          current->statements_.push_back( tmp );
        }
        else {
          this->SafeErrorCallback<&Callbacks::invalid_object_expression_>( *line );
        }
      }
    }
      break;

    default:
      line->type_ = RawLine::Type::Other;
    }
  }
  while ( current != &frame_->root_ ) {
    this->SafeErrorCallback<&Callbacks::corresponding_end_if_is_nothing_>( *current );
    current->end_line_ = frame_->raw_data_.back();
    current = current->parent_;
  }
}

void
BL::Parser::Parser::StatementEvaluating( void )
{
  frame_->root_.RootEvaluation( *this );

  auto tmp = [&] ( auto& table ) {
    for ( auto& one : table ) {
      if ( NOT( one.second.second ) && ( one.first != BL::Word::MIN ) ) {
        this->SafeErrorCallback<&Callbacks::defined_object_not_used_>( one.second.first );
      }
    }
  };
  tmp( frame_->wav_used_table_ );
  tmp( frame_->bmp_used_table_ );
  tmp( frame_->extended_bpm_used_table_ );
  tmp( frame_->stop_sequence_used_table_ );
}

void
BL::Parser::Parser::Finalize( void )
{
  for ( auto ex : frame_->exceptions_ ) {
    if ( ex->GetErrorLevel().ToValue() < frame_->bms_data_.most_serious_error_level_.ToValue() ) {
      frame_->bms_data_.most_serious_error_level_ = ex->GetErrorLevel();
    }
  }
}


namespace {
  template <class T> struct ExceptionOf {};
  template <class T>
  struct ExceptionOf<std::function<void ( BL::Parser::Parser&, T& )> BL::Parser::Parser::Callbacks::*> {
    using Type = typename T;
  };
}

template <class Exception>
void
BL::Parser::Parser::SafeErrorCallbackOf( std::shared_ptr<Exception> exception, Callbacks::FunctionOf<Exception&> f )
{
  frame_->exceptions_.push_back( exception );

  if ( callbacks_.exception_occurred_for_converter_ ) {
    callbacks_.exception_occurred_for_converter_( *this, *exception );
  }
  if ( callbacks_.exception_occurred_ ) {
    callbacks_.exception_occurred_( *this, *exception );
  }
  if ( f ) {
    f( *this, *exception );
  }
  if ( exception->GetErrorLevel().ToInteger() <= must_abort_error_level_.ToInteger() ) {
    frame_->need_to_abort_ = true;
  }
  if ( frame_->need_to_abort_ ) {
    throw AbortController();
  }
}

template <auto BL::Parser::Parser::Callbacks::* callback, class... Args>
void
BL::Parser::Parser::SafeErrorCallback( Args... args )
{
  auto exception = std::make_shared<typename ExceptionOf<decltype( callback )>::Type>( args... );
  this->SafeErrorCallbackOf( exception, callbacks_.*callback );
}

void
BL::Parser::Parser::CauseErrorWithoutCallback( std::shared_ptr<BmsDescriptionException> exception )
{
  this->SafeErrorCallbackOf<BmsDescriptionException>( exception, nullptr );
}


template <auto BL::Parser::Parser::Callbacks::* callback, class... Args>
void
BL::Parser::Parser::SafeCallback( Args... args )
{
  if ( callbacks_.*callback ) {
    (callbacks_.*callback)( *this, args... );
  }
  if ( frame_->need_to_abort_ ) {
    throw AbortController();
  }
}


// -- Frame --------------------------------------------------------------
BL::Parser::Parser::Frame::Frame( void ) :
bms_data_(),
raw_data_(),
root_(),
random_value_stack_( {0} ),
bar_length_changed_(),
lazy_eavluation_(),
exceptions_(),
need_to_abort_( false ),
wav_used_table_(),
bmp_used_table_(),
extended_bpm_used_table_(),
stop_sequence_used_table_(),
lnobj_word_( BL::Word::MIN ),
skip_parse_line_( false )
{
}
