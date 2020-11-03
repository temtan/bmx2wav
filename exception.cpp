// exception.cpp

#include "tt_string.h"
#include "tt_enum.h"

#include "exception.h"

using namespace BMX2WAV;


// -- Exception --------------------------------------------------------
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

Exception::Exception( void ) :
TtException()
{
}

std::string
Exception::GetMessage( void )
{
  return Exception::MakeMessage( "Error" );
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
  return Exception::MakeMessage( "内部エラーが発生しました。\r\nファイル : %s\r\n行番号 : %d", file_, line_ );
}


std::string
InternalException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << file_ << "(" << line_ << ")";
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
  return Exception::MakeMessage( "指定された音声ファイルが存在しません。ヘッダ : WAV%s ; ファイル : %s", this->GetObjectNumber().ToCharPointer(), this->GetFilePath().c_str() );
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
  return Exception::MakeMessage( "音声ファイルを開くのに失敗しました。ファイル : %s ; メッセージ : %s", this->GetFilePath().c_str(), this->GetSystemErrorMessage().c_str() );
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
  return Exception::MakeMessage( "WAV ファイルの読み込みに失敗しました。ファイル : %s ; メッセージ : %s", this->GetFilePath().c_str(), this->GetSystemErrorMessage().c_str() );
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
  return Exception::MakeMessage( "WAV ファイルの書き込みに失敗しました。ファイル : %s ; メッセージ : %s", this->GetFilePath().c_str(), this->GetSystemErrorMessage().c_str() );
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
  return Exception::MakeMessage( "ogg ファイルを開くのに失敗しました。ファイル : %s ; エラー番号 : %d", this->GetFilePath().c_str(), error_number_ );
}


// -- OggFileReadException -----------------------------------------------
OggFileReadException::OggFileReadException( const std::string& path ) :
AudioFileErrorWith( path )
{
}

std::string
OggFileReadException::GetMessage( void )
{
  return Exception::MakeMessage( "ogg ファイルの読み込みに失敗しました。ファイル : %s", this->GetFilePath().c_str() );
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
  return Exception::MakeMessage( "WAV ファイルの形式が不正です。ファイル : %s ; 理由 : %s", this->GetFilePath().c_str(), reason_.c_str() );
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
  return Exception::MakeMessage( "サポートしていない WAV ファイルの形式です。ファイル : %s ; 理由 : %s", this->GetFilePath().c_str(), reason_.c_str() );
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
  return Exception::MakeMessage( "メモリ確保に失敗しました。 メッセージ : %s", source_.what() );
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
  return Exception::MakeMessage( "BMS ファイルのアクセスでエラーがありました。ファイル : %s ; メッセージ : %s", this->GetFilePath().c_str(), tmp.c_str() );
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
  return Exception::MakeMessage( "出力ファイルのパスが BMS ファイルの WAV で指定されている入力ファイルになっています。ヘッダ名 : WAV%s ; ファイルパス : %s", this->GetObjectNumber().ToCharPointer(), this->GetFilePath().c_str() );
}


// -- InvalidFormatAsBpmHeaderException ----------------------------------
InvalidFormatAsBpmHeaderException::InvalidFormatAsBpmHeaderException( void )
{
}

std::string
InvalidFormatAsBpmHeaderException::GetMessage( void )
{
  return Exception::MakeMessage( "指定された BPM の表記が不正です。" );
}


// -- InvalidFormatAsExtendedBpmChangeValueException ---------------------
InvalidFormatAsExtendedBpmChangeValueException::InvalidFormatAsExtendedBpmChangeValueException( BL::Word object_number ) :
WithObjectNumberException( object_number )
{
}

std::string
InvalidFormatAsExtendedBpmChangeValueException::GetMessage( void )
{
  return Exception::MakeMessage( "指定された拡張 BPM の表記が不正です。ヘッダ名 BPM%s", this->GetObjectNumber().ToCharPointer() );
}


// -- InvalidFormatAsStopSequenceException -----------------------------
InvalidFormatAsStopSequenceException::InvalidFormatAsStopSequenceException( BL::Word object_number ) :
WithObjectNumberException( object_number )
{
}

std::string
InvalidFormatAsStopSequenceException::GetMessage( void )
{
  return Exception::MakeMessage( "指定されたストップシーケンスの表記が不正です。ヘッダ名 STOP%s", this->GetObjectNumber().ToCharPointer() );
}

// -- InvalidFormatAsLongNoteObjectHeaderException -----------------------
InvalidFormatAsLongNoteObjectHeaderException::InvalidFormatAsLongNoteObjectHeaderException( void )
{
}

std::string
InvalidFormatAsLongNoteObjectHeaderException::GetMessage( void )
{
  return Exception::MakeMessage( "LNOBJ で指定された表記が不正です。" );
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
  auto tmp1 = Exception::MakeMessage( "( 小節:%d - ch:%s - object:%s )", start_.bar_number_, start_.channel_number_.ToCharPointer(), start_.object_number_.ToCharPointer() );
  auto tmp2 = Exception::MakeMessage( "( 小節:%d - ch:%s - object:%s )", end_.bar_number_,   end_.channel_number_.ToCharPointer(),   end_.object_number_.ToCharPointer() );
  return Exception::MakeMessage( "ロングノートの終端のオブジェクトが始端と異なっています。 始端 : %s ; 終端 : %s", tmp1.c_str(), tmp2.c_str() );
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
  return Exception::MakeMessage( "ロングノートの終端がありません。始端小節番号 : %d ; チャンネル : %s ; オブジェクト : %s", tmp.bar_number_, tmp.channel_number_.ToCharPointer(), tmp.object_number_.ToCharPointer() );
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
  return Exception::MakeMessage( "WAV として登録されていないオブジェクトが使用されました。 小節番号 : %d ; チャンネル : %s ; オブジェクト : %s", tmp.bar_number_, tmp.channel_number_.ToCharPointer(), tmp.object_number_.ToCharPointer() );
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
  return Exception::MakeMessage( "通常の BPM 変更に使用されたオブジェクトが不正です。 小節番号 : %d ; オブジェクト : %s", this->GetBarNumber(), this->GetObjectNumber().ToCharPointer() );
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
  return Exception::MakeMessage( "拡張 BPM 変更でヘッダに未設定のオブジェクトが使用されました。 小節番号 : %d ; オブジェクト : %s", this->GetBarNumber(), this->GetObjectNumber().ToCharPointer() );
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
  return Exception::MakeMessage( "ストップシーケンスでヘッダに未設定のオブジェクトが使用されました。 小節番号 : %d ; オブジェクト : %s", this->GetBarNumber(), this->GetObjectNumber().ToCharPointer() );
}

// -- OutputFileAccessException ----------------------------------------
OutputFileAccessException::OutputFileAccessException( AudioFileOpenException& parent ) :
WithFilePathException( parent.GetFilePath() ),
WithErrorNumberException( parent.GetErrorNumber() )
{
}

std::string
OutputFileAccessException::GetMessage( void )
{
  return Exception::MakeMessage( "出力用の音声ファイルを開くのに失敗しました。ファイル : %s ; メッセージ : %s", this->GetFilePath().c_str(), this->GetSystemErrorMessage().c_str() );
}
