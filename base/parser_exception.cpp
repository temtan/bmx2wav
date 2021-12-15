// base/parser.cpp

#include "string_table.h"
#include "utility.h"

// parser_exception.h ÇÕ parser.h Ç©ÇÁàÍïîêÿÇËèoÇµÇ»ÇÃÇ≈ parser.h Çì«Ç›çûÇﬁ
#include "base/parser.h"

using namespace BMX2WAV;


// -- FileAccessException -----
BL::Parser::FileAccessException::FileAccessException( TtFileAccessException& ex ) :
path_( ex.GetFileName() ),
error_number_( ex.GetErrorNumber() )
{
}

const std::string&
BL::Parser::FileAccessException::GetPath( void ) const
{
  return path_;
}

int
BL::Parser::FileAccessException::GetErrorNumber( void ) const
{
  return error_number_;
}

std::string
BL::Parser::FileAccessException::GetMessage( void )
{
  auto tmp = TtUtility::GetANSIErrorMessage( error_number_ );
  return Utility::Format( StrT::Message::Parser::FileAccessError.Get(), path_.c_str(), tmp.c_str() );
}


// -- BmsDescriptionException --------------------------------------------
BL::Parser::BmsDescriptionException::BmsDescriptionException( BL::Parser::RawLine& line, ErrorLevel error_level ) :
line_( line ),
error_level_( error_level )
{
}


const BL::Parser::RawLine
BL::Parser::BmsDescriptionException::GetLine( void ) const
{
  return line_;
}

TtEnum<ErrorLevel>
BL::Parser::BmsDescriptionException::GetErrorLevel( void ) const
{
  return error_level_;
}


void
BL::Parser::BmsDescriptionException::ResetErrorLevel( TtEnum<ErrorLevel> error_level )
{
  error_level_ = error_level;
}

// -- BmsDescriptionCustomMessageException -------------------------------
BL::Parser::BmsDescriptionCustomMessageException::BmsDescriptionCustomMessageException( RawLine& line, ErrorLevel level, const std::string& message ) :
BmsDescriptionException( line, level ),
message_( message )
{
}

std::string
BL::Parser::BmsDescriptionCustomMessageException::GetMessage( void )
{
  return message_;
}

// -- NoBeginningEndRandomException --------------------------------------
BL::Parser::NoBeginningEndRandomException::NoBeginningEndRandomException( EndRandom& end_random ) :
BmsDescriptionExceptionWith( *end_random.raw_line_ ),
end_random_( end_random )
{
}

const BL::Parser::EndRandom&
BL::Parser::NoBeginningEndRandomException::GetEndRandom( void ) const
{
  return end_random_;
}

std::string
BL::Parser::NoBeginningEndRandomException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::NoBeginningEndRandom.Get() );
}

// -- HeaderCollisionException -------------------------------------------
BL::Parser::HeaderCollisionException::HeaderCollisionException( Header& header ) :
BmsDescriptionExceptionWith( *header.raw_line_ ),
WithHeader( header )
{
}

std::string
BL::Parser::HeaderCollisionException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::HeaderCollision.Get(), line_.key_.c_str() );
}

// -- InvalidBpmException ------------------------------------------------
BL::Parser::InvalidBpmException::InvalidBpmException( Header& header ) :
BmsDescriptionExceptionWith( *header.raw_line_ ),
WithHeader( header )
{
}

std::string
BL::Parser::InvalidBpmException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::InvalidBpm.Get() );
}

// -- InvalidLnobjException --------------------------------------------
BL::Parser::InvalidLnobjException::InvalidLnobjException( Header& header ) :
BmsDescriptionExceptionWith( *header.raw_line_ ),
WithHeader( header )
{
}

std::string
BL::Parser::InvalidLnobjException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::InvalidLnobj.Get() );
}

// -- ObjectCollisionException -------------------------------------------
BL::Parser::ObjectCollisionException::ObjectCollisionException( Channel& channel, BL::Word old_word, BL::Word new_word ) :
BmsDescriptionExceptionWith( *channel.raw_line_ ),
WithChannel( channel ),
old_word_( old_word ),
new_word_( new_word )
{
}

BL::Word
BL::Parser::ObjectCollisionException::GetOldWord( void ) const
{
  return old_word_;
}
BL::Word
BL::Parser::ObjectCollisionException::GetNewWord( void ) const
{
  return new_word_;
}

std::string
BL::Parser::ObjectCollisionException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::ObjectCollision.Get() );
}

// -- BarLengthChangeCollisionException ----------------------------------
BL::Parser::BarLengthChangeCollisionException::BarLengthChangeCollisionException( BarLengthChange& bar_length_change ) :
BmsDescriptionExceptionWith( *bar_length_change.raw_line_ ),
bar_length_change_( bar_length_change )
{
}

const BL::Parser::BarLengthChange&
BL::Parser::BarLengthChangeCollisionException::GetBarLengthChange( void ) const
{
  return bar_length_change_;
}

std::string
BL::Parser::BarLengthChangeCollisionException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::BarLengthChangeCollision.Get() );
}

// -- InvalidChannelOrHeaderSyntaxException ------------------------------
BL::Parser::InvalidChannelOrHeaderSyntaxException::InvalidChannelOrHeaderSyntaxException( RawLine& raw_line ) :
BmsDescriptionExceptionWith( raw_line )
{
}

std::string
BL::Parser::InvalidChannelOrHeaderSyntaxException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::InvalidChannelOrHeaderSyntax.Get() );
}

// -- CorrespondingEndIfIsNotingException --------------------------------
BL::Parser::CorrespondingEndIfIsNotingException::CorrespondingEndIfIsNotingException( Block& block ) :
BmsDescriptionExceptionWith( *block.raw_line_ ),
block_( block )
{
}

const BL::Parser::Block&
BL::Parser::CorrespondingEndIfIsNotingException::GetBlock( void ) const
{
  return block_;
}

std::string
BL::Parser::CorrespondingEndIfIsNotingException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::CorrespondingEndIfIsNoting.Get() );
}

// -- InvalidRandomStatementValueException -------------------------------
BL::Parser::InvalidRandomStatementValueException::InvalidRandomStatementValueException( RawLine& raw_line ) :
BmsDescriptionExceptionWith( raw_line )
{
}

std::string
BL::Parser::InvalidRandomStatementValueException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::InvalidRandomStatementValue.Get(), line_.key_.c_str() );
}

// -- CorrespondingIfIsNotingException -----------------------------------
BL::Parser::CorrespondingIfIsNotingException::CorrespondingIfIsNotingException( RawLine& raw_line ) :
BmsDescriptionExceptionWith( raw_line )
{
}

std::string
BL::Parser::CorrespondingIfIsNotingException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::CorrespondingIfIsNoting.Get() );
}

// -- InvalidBarLengthChangeValueException -------------------------------
BL::Parser::InvalidBarLengthChangeValueException::InvalidBarLengthChangeValueException( RawLine& raw_line ) :
BmsDescriptionExceptionWith( raw_line )
{
}

std::string
BL::Parser::InvalidBarLengthChangeValueException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::InvalidBarLengthChangeValue.Get() );
}

// -- InvalidObjectExpressionException -----------------------------------
BL::Parser::InvalidObjectExpressionException::InvalidObjectExpressionException( RawLine& raw_line ) :
BmsDescriptionExceptionWith( raw_line )
{
}

std::string
BL::Parser::InvalidObjectExpressionException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::InvalidObjectExpression.Get() );
}

// -- InvalidExtendedBpmException --------------------------------------
BL::Parser::InvalidExtendedBpmException::InvalidExtendedBpmException( Header& header ) :
BmsDescriptionExceptionWith( *header.raw_line_ ),
WithHeader( header )
{
}

std::string
BL::Parser::InvalidExtendedBpmException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::InvalidExtendedBPM.Get() );
}

// -- InvalidStopSequenceException ---------------------------------------
BL::Parser::InvalidStopSequenceException::InvalidStopSequenceException( Header& header ) :
BmsDescriptionExceptionWith( *header.raw_line_ ),
WithHeader( header )
{
}

std::string
BL::Parser::InvalidStopSequenceException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::InvalidStopSequence.Get() );
}

// -- UndefinedObjectUsedException ---------------------------------------
BL::Parser::UndefinedObjectUsedException::UndefinedObjectUsedException( Channel& channel, BL::Word used_object ) :
BmsDescriptionExceptionWith( *channel.raw_line_ ),
WithChannel( channel ),
used_object_( used_object )
{
}

BL::Word
BL::Parser::UndefinedObjectUsedException::GetUsedObject( void ) const
{
  return used_object_;
}

std::string
BL::Parser::UndefinedObjectUsedException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::UndefinedObjectUsed.Get(), used_object_.ToCharPointer() );
}

// -- DefinedObjectNotUsedException ------------------------------------
BL::Parser::DefinedObjectNotUsedException::DefinedObjectNotUsedException( Header& header ) :
BmsDescriptionExceptionWith( *header.raw_line_ ),
WithHeader( header )
{
}

std::string
BL::Parser::DefinedObjectNotUsedException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::DefinedObjectNotUsed.Get(), line_.key_.c_str() );
}

// -- ResolutionExceededLimitException ---------------------------------
BL::Parser::ResolutionExceededLimitException::ResolutionExceededLimitException( Channel& channel, unsigned int required_resolution ) :
BmsDescriptionExceptionWith( *channel.raw_line_ ),
WithChannel( channel ),
required_resolution_( required_resolution )
{
}

unsigned int
BL::Parser::ResolutionExceededLimitException::GetRequiredResolution( void ) const
{
  return required_resolution_;
}

std::string
BL::Parser::ResolutionExceededLimitException::GetMessage( void )
{
  return Utility::Format( StrT::Message::Parser::ResolutionExceededLimit.Get(), required_resolution_ );
}


// -- BMX2WAV ----------
// -- ConvertBmsDescriptionException -------------------------------------
ConvertBmsDescriptionException::ConvertBmsDescriptionException( BL::Parser::BmsDescriptionException& origin ) :
ConvertException( origin.GetErrorLevel() ),
line_( origin.GetLine() ),
message_( origin.GetMessage() )
{
}

const BL::Parser::RawLine
ConvertBmsDescriptionException::GetLine( void ) const
{
  return line_;
}

std::string
ConvertBmsDescriptionException::GetMessage( void )
{
  return message_;
}
