// exception.cpp

#include "tt_string.h"
#include "tt_enum.h"

#include "string_table.h"
#include "utility.h"

#include "exception.h"

using namespace BMX2WAV;


// -- Exception --------------------------------------------------------
/*
std::string
Exception::MakeMessage( const char* format, ... )
{
  va_list args;
  va_start( args, format );
  char tmp[1024];
  ::vsprintf_s( tmp, sizeof( tmp ), format, args );
  va_end( args );
  return tmp;
}
*/

Exception::Exception( void ) :
TtException()
{
}

std::string
Exception::GetMessage( void )
{
  return Utility::Format( "Error" );
}


// -- InternalException ------------------------------------------------
InternalException::InternalException( const char* file, unsigned int line ) :
file_( file ),
line_( line )
{
}


const char*
InternalException::GetFile( void ) const
{
  return file_;
}

unsigned int
InternalException::GetLine( void ) const
{
  return line_;
}


std::string
InternalException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InternalError.Get(), file_, line_ );
}


std::string
InternalException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << file_ << "(" << line_ << ")";
  return os.str();
}


// -- LanguageException --------------------------------------------------
LanguageException::LanguageException( const std::string& language ) :
language_( language )
{
}

const std::string&
LanguageException::GetLanguage( void )
{
  return language_;
}

std::string
LanguageException::GetMessage( void )
{
  throw BMX2WAV_INTERNAL_EXCEPTION;
}

std::string
LanguageException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << language_;
  return os.str();
}

// -- ConvertException ---------------------------------------------------
ConvertException::ConvertException( ErrorLevel error_level ) :
error_level_( error_level )
{
}

TtEnum<ErrorLevel>
ConvertException::GetErrorLevel( void ) const
{
  return error_level_;
}

void
ConvertException::ResetErrorLevel( TtEnum<ErrorLevel> error_level )
{
  error_level_ = error_level;
}


// -- MessageOnlyException -------------------------------------------------
MessageOnlyException::MessageOnlyException( const std::string& message ) :
message_( message )
{
}

std::string
MessageOnlyException::GetMessage( void )
{
  return message_;
}


// -- AudioFileError -----------------------------------------------------
AudioFileError::AudioFileError( ErrorLevel error_level, const std::string& path ) :
ConvertException( error_level ),
WithFilePathException( path )
{
}


// -- EntriedAudioFileNotFoundException ----------------------------------
EntriedAudioFileNotFoundException::EntriedAudioFileNotFoundException( BL::Word object_number, const std::string& path ) :
AudioFileErrorWith( path ),
WithObjectNumberException( object_number )
{
}

std::string
EntriedAudioFileNotFoundException::GetMessage( void )
{
  return Utility::Format( StrT::Message::EntriedAudioFileNotFound.Get(), this->GetObjectNumber().ToCharPointer(), this->GetFilePath() );
}


// -- AudioFileOpenException ---------------------------------------------
AudioFileOpenException::AudioFileOpenException( const std::string& path, int error_number ) :
AudioFileErrorWith( path ),
WithErrorNumberException( error_number )
{
}


std::string
AudioFileOpenException::GetMessage( void )
{
  return Utility::Format( StrT::Message::AudioFileOpenError.Get(), this->GetFilePath(), this->GetSystemErrorMessage() );
}


// -- WavFileReadException -----------------------------------------------
WavFileReadException::WavFileReadException( const std::string& path, int error_number ) :
AudioFileErrorWith( path ),
WithErrorNumberException( error_number )
{
}

std::string
WavFileReadException::GetMessage( void )
{
  return Utility::Format( StrT::Message::WavFileReadError.Get(), this->GetFilePath(), this->GetSystemErrorMessage() );
}


// -- WavFileWriteException ----------------------------------------------
WavFileWriteException::WavFileWriteException( const std::string& path, int error_number ) :
AudioFileErrorWith( path ),
WithErrorNumberException( error_number )
{
}

std::string
WavFileWriteException::GetMessage( void )
{
  return Utility::Format( StrT::Message::WavFileWriteError.Get(), this->GetFilePath(), this->GetSystemErrorMessage() );
}



// -- OggFileOpenException ---------------------------------------------
OggFileOpenException::OggFileOpenException( const std::string& path, int error_number ) :
AudioFileErrorWith( path ),
WithErrorNumberException( error_number )
{
}


std::string
OggFileOpenException::GetMessage( void )
{
  return Utility::Format( StrT::Message::OggFileOpenError.Get(), this->GetFilePath(), error_number_ );
}


// -- OggFileReadException -----------------------------------------------
OggFileReadException::OggFileReadException( const std::string& path ) :
AudioFileErrorWith( path )
{
}

std::string
OggFileReadException::GetMessage( void )
{
  return Utility::Format( StrT::Message::OggFileReadError.Get(), this->GetFilePath() );
}


// -- InvalidWavFileFormatException --------------------------------------
InvalidWavFileFormatException::InvalidWavFileFormatException( const std::string& path, const std::string& reason ) :
AudioFileErrorWith( path ),
reason_( reason )
{
}

std::string
InvalidWavFileFormatException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InvalidWavFileFormat.Get(), this->GetFilePath(), reason_ );
}

const std::string&
InvalidWavFileFormatException::GetReason( void ) const
{
  return reason_;
}

// -- UnsupportedWavFileFormatException ----------------------------------
UnsupportedWavFileFormatException::UnsupportedWavFileFormatException( const std::string& path, const std::string& reason ) :
AudioFileErrorWith( path ),
reason_( reason )
{
}

std::string
UnsupportedWavFileFormatException::GetMessage( void )
{
  return Utility::Format( StrT::Message::UnsupportedWavFileFormat.Get(), this->GetFilePath(), reason_ );
}

const std::string&
UnsupportedWavFileFormatException::GetReason( void ) const
{
  return reason_;
}


// -- BadAllocationException ---------------------------------------------
BadAllocationException::BadAllocationException( std::bad_alloc source ) :
source_( source )
{
}

std::string
BadAllocationException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BadAllocation.Get(), source_.what() );
}


// -- BmsFileAccessException ---------------------------------------------
BmsFileAccessException::BmsFileAccessException( const std::string& path, int error_number ) :
WithFilePathException( path ),
error_number_( error_number )
{
}

int
BmsFileAccessException::GetErrorNumber( void ) const
{
  return error_number_;
}

std::string
BmsFileAccessException::GetMessage( void )
{
  auto tmp = TtUtility::GetANSIErrorMessage( error_number_ );
  return Utility::Format( StrT::Message::BmsFileAccessError.Get(), this->GetFilePath(), tmp );
}


// -- OutputFileIsInputFilePathException ---------------------------------
OutputFileIsInputFilePathException::OutputFileIsInputFilePathException( BL::Word number, const std::string& path ) :
WithObjectNumberException( number ),
WithFilePathException( path )
{
}

std::string
OutputFileIsInputFilePathException::GetMessage( void )
{
  return Utility::Format( StrT::Message::OutputFileIsInputFilePath.Get(), this->GetObjectNumber().ToCharPointer(), this->GetFilePath() );
}


// -- InvalidFormatAsBpmHeaderException ----------------------------------
InvalidFormatAsBpmHeaderException::InvalidFormatAsBpmHeaderException( void )
{
}

std::string
InvalidFormatAsBpmHeaderException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InvalidFormatAsBpmHeader.Get() );
}


// -- InvalidFormatAsExtendedBpmChangeValueException ---------------------
InvalidFormatAsExtendedBpmChangeValueException::InvalidFormatAsExtendedBpmChangeValueException( BL::Word object_number ) :
WithObjectNumberException( object_number )
{
}

std::string
InvalidFormatAsExtendedBpmChangeValueException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InvalidFormatAsExtendedBpm.Get(), this->GetObjectNumber().ToCharPointer() );
}


// -- InvalidFormatAsStopSequenceException -----------------------------
InvalidFormatAsStopSequenceException::InvalidFormatAsStopSequenceException( BL::Word object_number ) :
WithObjectNumberException( object_number )
{
}

std::string
InvalidFormatAsStopSequenceException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InvalidFormatAsStopSequence.Get(), this->GetObjectNumber().ToCharPointer() );
}

// -- InvalidFormatAsLongNoteObjectHeaderException -----------------------
InvalidFormatAsLongNoteObjectHeaderException::InvalidFormatAsLongNoteObjectHeaderException( void )
{
}

std::string
InvalidFormatAsLongNoteObjectHeaderException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InvalidFormatAsLNOBJHeader.Get() );
}


// -- LongNoteObjectInvalidEncloseException ------------------------------
LongNoteObjectInvalidEncloseException::LongNoteObjectInvalidEncloseException( BL::ObjectWithLocation start, BL::ObjectWithLocation end ) :
start_( start ),
end_( end )
{
}

std::string
LongNoteObjectInvalidEncloseException::GetMessage( void )
{
  auto tmp1 = Utility::Format( StrT::Message::LNObjectInvalidEncloseAdd.Get(), start_.bar_number_, start_.channel_number_.ToCharPointer(), start_.object_number_.ToCharPointer() );
  auto tmp2 = Utility::Format( StrT::Message::LNObjectInvalidEncloseAdd.Get(), end_.bar_number_,   end_.channel_number_.ToCharPointer(),   end_.object_number_.ToCharPointer() );
  return Utility::Format( StrT::Message::LNObjectInvalidEnclose.Get(), tmp1, tmp2 );
}

BL::ObjectWithLocation
LongNoteObjectInvalidEncloseException::GetStartObject( void )
{
  return start_;
}

BL::ObjectWithLocation
LongNoteObjectInvalidEncloseException::GetEndObject( void )
{
  return end_;
}


// -- LongNoteObjectNotEnclosedException ---------------------------------
LongNoteObjectNotEnclosedException::LongNoteObjectNotEnclosedException( BL::ObjectWithLocation object ) :
WithObjectWithLocationException( object )
{
}

std::string
LongNoteObjectNotEnclosedException::GetMessage( void )
{
  auto tmp = this->GetObjectWithLocation();
  return Utility::Format( StrT::Message::LNObjectNotEnclosed.Get(), tmp.bar_number_, tmp.channel_number_.ToCharPointer(), tmp.object_number_.ToCharPointer() );
}


// -- NotEntriedWavWasUsedException --------------------------------------
NotEntriedWavWasUsedException::NotEntriedWavWasUsedException( BL::ObjectWithLocation object ) :
WithObjectWithLocationException( object )
{
}

std::string
NotEntriedWavWasUsedException::GetMessage( void )
{
  auto tmp = this->GetObjectWithLocation();
  return Utility::Format( StrT::Message::NotEntriedWavWasUsed.Get(), tmp.bar_number_, tmp.channel_number_.ToCharPointer(), tmp.object_number_.ToCharPointer() );
}

// -- InvalidFormatAsBpmChangeValueException -----------------------------
InvalidFormatAsBpmChangeValueException::InvalidFormatAsBpmChangeValueException( unsigned int bar_number, BL::Word object_number ) :
WithBarNumberException( bar_number ),
WithObjectNumberException( object_number )
{
}

std::string
InvalidFormatAsBpmChangeValueException::GetMessage( void )
{
  return Utility::Format( StrT::Message::InvalidFormatAsBpmChange.Get(), this->GetBarNumber(), this->GetObjectNumber().ToCharPointer() );
}

// -- ExtendedBpmChangeEntryNotExistException ----------------------------
ExtendedBpmChangeEntryNotExistException::ExtendedBpmChangeEntryNotExistException( unsigned int bar_number, BL::Word object_number ) :
WithBarNumberException( bar_number ),
WithObjectNumberException( object_number )
{
}

std::string
ExtendedBpmChangeEntryNotExistException::GetMessage( void )
{
  return Utility::Format( StrT::Message::ExBpmChangeEntryNotExist.Get(), this->GetBarNumber(), this->GetObjectNumber().ToCharPointer() );
}

// -- StopSequenceEntryNotExistException ---------------------------------
StopSequenceEntryNotExistException::StopSequenceEntryNotExistException( unsigned int bar_number, BL::Word object_number ) :
WithBarNumberException( bar_number ),
WithObjectNumberException( object_number )
{
}

std::string
StopSequenceEntryNotExistException::GetMessage( void )
{
  return Utility::Format( StrT::Message::StopSequenceEntryNotExist.Get(), this->GetBarNumber(), this->GetObjectNumber().ToCharPointer() );
}

// -- OutputFileAccessException ------------------------------------------
OutputFileAccessException::OutputFileAccessException( AudioFileOpenException& parent ) :
WithFilePathException( parent.GetFilePath() ),
WithErrorNumberException( parent.GetErrorNumber() )
{
}

std::string
OutputFileAccessException::GetMessage( void )
{
  return Utility::Format( StrT::Message::OutputFileAccessError.Get(), this->GetFilePath(), this->GetSystemErrorMessage() );
}


// -- Bmson ŠÖ˜A ---------------------------------------------------------
// -- OutOfBmsRangeException ---------------------------------------------

// -- BarIsOutOfBmsRangeException ----------------------------------------
std::string
BarIsOutOfBmsRangeException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::BarIsOutOfBmsRange.Get() );
}

// -- NumberOfObjectsIsOutOfBmsRangeException ----------------------------
NumberOfObjectsIsOutOfBmsRangeException::NumberOfObjectsIsOutOfBmsRangeException( const std::string& object_kind ) :
object_kind_( object_kind )
{
}

const std::string&
NumberOfObjectsIsOutOfBmsRangeException::GetObjectKind( void )
{
  return object_kind_;
}

std::string
NumberOfObjectsIsOutOfBmsRangeException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::NumberOfObjectsIsOutOfBmsRange.Get(), this->GetObjectKind() );
}

// -- BmsonObjectIsOutOfBmsonLineRangeException --------------------------
std::string
BmsonObjectIsOutOfBmsonLineRangeException::GetMessage( void )
{
  return Utility::Format( StrT::Message::BmsonParser::BmsonObjectIsOutOfBmsonLineRange.Get() );
}

