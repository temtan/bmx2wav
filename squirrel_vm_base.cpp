// squirrel_vm_base.cpp

#include "tt_string.h"
#include "tt_process.h"
#include "tt_path.h"
#include "tt_message_box.h"

#include "tt_squirrel_utility.h"

#include "utility.h"
#include "core/direct_sound_stream.h"
#include "core/convert_parameter.h"

#include "squirrel_vm_base.h"

using namespace BMX2WAV;


namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( print );
  DEFINE_PARAMETER_NAME_STRING( system );
  DEFINE_PARAMETER_NAME_STRING( constructor );
  DEFINE_PARAMETER_NAME_STRING( BMX2WAV );
  DEFINE_PARAMETER_NAME_STRING( Path );
  DEFINE_PARAMETER_NAME_STRING( path );
  DEFINE_PARAMETER_NAME_STRING( BmsData );
  DEFINE_PARAMETER_NAME_STRING( basename );
  DEFINE_PARAMETER_NAME_STRING( dirname );
  DEFINE_PARAMETER_NAME_STRING( change_extension );
  DEFINE_PARAMETER_NAME_STRING( integer_to_word_string );
  DEFINE_PARAMETER_NAME_STRING( word_string_to_integer );
  DEFINE_PARAMETER_NAME_STRING( headers );
  DEFINE_PARAMETER_NAME_STRING( object_count );
  DEFINE_PARAMETER_NAME_STRING( object_count_of_1P );
  DEFINE_PARAMETER_NAME_STRING( object_count_of_2P );
  DEFINE_PARAMETER_NAME_STRING( has_random_statement );
  DEFINE_PARAMETER_NAME_STRING( calculate_playing_time );
  DEFINE_PARAMETER_NAME_STRING( play_wav_file_async );
  DEFINE_PARAMETER_NAME_STRING( play_ogg_file_async );
  DEFINE_PARAMETER_NAME_STRING( MessageBox );
  DEFINE_PARAMETER_NAME_STRING( Icon );
  DEFINE_PARAMETER_NAME_STRING( DefaultButton );
  DEFINE_PARAMETER_NAME_STRING( Result );
  DEFINE_PARAMETER_NAME_STRING( None );
  DEFINE_PARAMETER_NAME_STRING( Exclamation );
  DEFINE_PARAMETER_NAME_STRING( Warning );
  DEFINE_PARAMETER_NAME_STRING( Information );
  DEFINE_PARAMETER_NAME_STRING( Asterisk );
  DEFINE_PARAMETER_NAME_STRING( Question );
  DEFINE_PARAMETER_NAME_STRING( Stop );
  DEFINE_PARAMETER_NAME_STRING( Error );
  DEFINE_PARAMETER_NAME_STRING( Hand );
  DEFINE_PARAMETER_NAME_STRING( First );
  DEFINE_PARAMETER_NAME_STRING( Second );
  DEFINE_PARAMETER_NAME_STRING( Third );
  DEFINE_PARAMETER_NAME_STRING( Fourth );
  DEFINE_PARAMETER_NAME_STRING( Abort );
  DEFINE_PARAMETER_NAME_STRING( Cancel );
  DEFINE_PARAMETER_NAME_STRING( Ignore );
  DEFINE_PARAMETER_NAME_STRING( No );
  DEFINE_PARAMETER_NAME_STRING( Ok );
  DEFINE_PARAMETER_NAME_STRING( Retry );
  DEFINE_PARAMETER_NAME_STRING( Yes );
  DEFINE_PARAMETER_NAME_STRING( OkCancel );
  DEFINE_PARAMETER_NAME_STRING( RetryCancel );
  DEFINE_PARAMETER_NAME_STRING( YesNo );
  DEFINE_PARAMETER_NAME_STRING( YesNoCancel );
  DEFINE_PARAMETER_NAME_STRING( AbortRetryIgnore );
  DEFINE_PARAMETER_NAME_STRING( SubMenu );
  DEFINE_PARAMETER_NAME_STRING( shell_execute );
  DEFINE_PARAMETER_NAME_STRING( create_process );
  DEFINE_PARAMETER_NAME_STRING( Separator );
  DEFINE_PARAMETER_NAME_STRING( name );
  DEFINE_PARAMETER_NAME_STRING( items );
  DEFINE_PARAMETER_NAME_STRING( ErrorLevel );
  DEFINE_PARAMETER_NAME_STRING( get_execute_file_directory_path );
  DEFINE_PARAMETER_NAME_STRING( error_level_to_string );
  DEFINE_PARAMETER_NAME_STRING( most_serious_error_level );
  DEFINE_PARAMETER_NAME_STRING( get_not_exist_path_from );
  DEFINE_PARAMETER_NAME_STRING( remove_can_not_use_character_as_file_path_from );
  DEFINE_PARAMETER_NAME_STRING( ShowState );
  DEFINE_PARAMETER_NAME_STRING( expand_environment_string );
  DEFINE_PARAMETER_NAME_STRING( file_exist );
  DEFINE_PARAMETER_NAME_STRING( ConvertException );
  DEFINE_PARAMETER_NAME_STRING( message );
  DEFINE_PARAMETER_NAME_STRING( get_error_level );
  DEFINE_PARAMETER_NAME_STRING( reset_error_level );
  DEFINE_PARAMETER_NAME_STRING( BmsDescriptionException );
  DEFINE_PARAMETER_NAME_STRING( line );
  DEFINE_PARAMETER_NAME_STRING( max_resolution );
  DEFINE_PARAMETER_NAME_STRING( bar_number_of_max_resolution );
  DEFINE_PARAMETER_NAME_STRING( NormalizeKind );
  DEFINE_PARAMETER_NAME_STRING( normalize_kind_to_string );
  // DEFINE_PARAMETER_NAME_STRING(  );
}


// -- SquirrelVM ---------------------------------------------------------
const char* const
SquirrelVMBase::BASE_FILENAME = "bmx2wav.base.nut";

std::unordered_map<HSQUIRRELVM, SquirrelVMBase*>
SquirrelVMBase::HANDLE_TABLE;

void
SquirrelVMBase::PrintFunctionStatic( HSQUIRRELVM vm_, const SQChar* s, ... )
{
  SquirrelVMBase& vm = SquirrelVMBase::GetVM( vm_ );
  if ( vm.print_function_ ) {
    va_list arglist;
    va_start( arglist, s );
    TtString::UniqueString buffer( _vscprintf( s, arglist ) );
    vsprintf_s( buffer.GetPointer(), buffer.GetCapacity(), s, arglist );
    va_end( arglist );
    vm.print_function_( buffer.GetPointer() );
  }
}

TtSquirrel::VirtualMachine::Closure
SquirrelVMBase::ConvertClosure( std::function<int ( SquirrelVMBase& )> closure )
{
  return [closure] ( TtSquirrel::VirtualMachine& vm ) -> int {
    return closure( SquirrelVMBase::GetVM( vm ) );
  };
}


SquirrelVMBase&
SquirrelVMBase::GetVM( HSQUIRRELVM vm )
{
  auto it = HANDLE_TABLE.find( vm );
  if ( it == HANDLE_TABLE.end() ) {
    throw BMX2WAV_INTERNAL_EXCEPTION;
  }
  return *it->second;
}

SquirrelVMBase&
SquirrelVMBase::GetVM( TtSquirrel::VirtualMachine& vm )
{
  return SquirrelVMBase::GetVM( vm.GetHandle() );
}


SquirrelVMBase::SquirrelVMBase( TtWindow* parent_window ) :
TtSquirrel::VirtualMachine(),
print_function_( nullptr ),
parent_window_( parent_window )
{
  HANDLE_TABLE.insert( {this->GetHandle(), this} );

  this->RegisterStandardLibraries();
  this->Native().SetPrintFunction( SquirrelVMBase::PrintFunctionStatic, SquirrelVMBase::PrintFunctionStatic );
}

SquirrelVMBase::~SquirrelVMBase()
{
  if ( auto it = HANDLE_TABLE.find( this->GetHandle() ); it != HANDLE_TABLE.end() ) {
    HANDLE_TABLE.erase( it );
  }
}

void
SquirrelVMBase::SetPrintFunction( SquirrelVMBase::PrintFunction print_function )
{
  print_function_ = print_function;
}


void
SquirrelVMBase::Initialize( void )
{
  // -- MessageBox 実装 -----
  this->NewSlotOfRootTableByString(
    Tag::MessageBox,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewTable();

      // Icon 定義
      this->NewSlotOfTopByString(
        Tag::Icon,
        [&] () {
          TtSquirrel::StackRecoverer recoverer( this, 1 );
          this->Native().NewTable();
          this->NewIntegerSlotOfTopByString( Tag::None,        TtMessageBox::Icon::NONE        );
          this->NewIntegerSlotOfTopByString( Tag::Exclamation, TtMessageBox::Icon::EXCLAMATION );
          this->NewIntegerSlotOfTopByString( Tag::Warning,     TtMessageBox::Icon::WARNING );
          this->NewIntegerSlotOfTopByString( Tag::Information, TtMessageBox::Icon::INFORMATION );
          this->NewIntegerSlotOfTopByString( Tag::Asterisk,    TtMessageBox::Icon::ASTERISK    );
          this->NewIntegerSlotOfTopByString( Tag::Question,    TtMessageBox::Icon::QUESTION    );
          this->NewIntegerSlotOfTopByString( Tag::Stop,        TtMessageBox::Icon::STOP        );
          this->NewIntegerSlotOfTopByString( Tag::Error,       TtMessageBox::Icon::ERROR       );
          this->NewIntegerSlotOfTopByString( Tag::Hand,        TtMessageBox::Icon::HAND        );
        } );

      // DefaultButton 定義
      this->NewSlotOfTopByString(
        Tag::DefaultButton,
        [&] () {
          TtSquirrel::StackRecoverer recoverer( this, 1 );
          Native().NewTable();
          this->NewIntegerSlotOfTopByString( Tag::First,  TtMessageBox::DefaultButton::FIRST  );
          this->NewIntegerSlotOfTopByString( Tag::Second, TtMessageBox::DefaultButton::SECOND );
          this->NewIntegerSlotOfTopByString( Tag::Third,  TtMessageBox::DefaultButton::THIRD  );
          this->NewIntegerSlotOfTopByString( Tag::Fourth, TtMessageBox::DefaultButton::FOURTH );
        } );

      // Result 定義
      this->NewSlotOfTopByString(
        Tag::Result,
        [&] () {
          TtSquirrel::StackRecoverer recoverer( this, 1 );
          Native().NewTable();
          this->NewIntegerSlotOfTopByString( Tag::Abort,  TtMessageBox::Result::ABORT  );
          this->NewIntegerSlotOfTopByString( Tag::Cancel, TtMessageBox::Result::CANCEL );
          this->NewIntegerSlotOfTopByString( Tag::Ignore, TtMessageBox::Result::IGNORE );
          this->NewIntegerSlotOfTopByString( Tag::No,     TtMessageBox::Result::NO     );
          this->NewIntegerSlotOfTopByString( Tag::Ok,     TtMessageBox::Result::OK     );
          this->NewIntegerSlotOfTopByString( Tag::Retry,  TtMessageBox::Result::RETRY  );
          this->NewIntegerSlotOfTopByString( Tag::Yes,    TtMessageBox::Result::YES    );
        } );

      using ShowFunction = int (*)( TtWindow&, const std::string&, const std::string&, unsigned int );
      auto define_closure = [&] ( const std::string& name, ShowFunction show_function ) {
        this->NewSlotOfTopByString(
          name,
          [&] () {
            this->NewClosure( SquirrelVMBase::ConvertClosure( [&parent = parent_window_, show_function] ( SquirrelVMBase& vm ) -> int {
              int icon = vm.GetAsFromTop<int>();
              vm.Native().PopTop();
              std::string caption = vm.GetAsFromTop<std::string>();
              vm.Native().PopTop();
              std::string message = vm.GetAsFromTop<std::string>();
              vm.Native().PopTop();

              int result = show_function( *parent, message, caption, icon );
              vm.Native().PushInteger( result );
              return TtSquirrel::Const::ExistReturnValue;
            } ) );
            Native().SetParamsCheck( 4, "tssi" );
          } );
      };

      define_closure( Tag::Ok,               static_cast<ShowFunction>( &TtMessageBoxOk::Show ) );
      define_closure( Tag::OkCancel,         static_cast<ShowFunction>( &TtMessageBoxOkCancel::Show ) );
      define_closure( Tag::RetryCancel,      static_cast<ShowFunction>( &TtMessageBoxRetryCancel::Show ) );
      define_closure( Tag::YesNo,            static_cast<ShowFunction>( &TtMessageBoxYesNo::Show ) );
      define_closure( Tag::YesNoCancel,      static_cast<ShowFunction>( &TtMessageBoxYesNoCancel::Show ) );
      define_closure( Tag::AbortRetryIgnore, static_cast<ShowFunction>( &TtMessageBoxAbortRetryIgnore::Show ) );
    } );

  // -- Path 実装 -----
  this->NewSlotOfRootTableByString(
    Tag::Path,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewTable();

      // -- file_exist 定義
      this->NewSlotOfTopByString(
        Tag::file_exist,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushBoolean( TtPath::FileExists( vm.GetAsFromTop<std::string>() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, ".s" );
        } );

      // -- basename 定義
      this->NewSlotOfTopByString(
        Tag::basename,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( TtPath::BaseName( vm.GetAsFromTop<std::string>() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ts" );
        } );

      // -- dirname 定義
      this->NewSlotOfTopByString(
        Tag::dirname,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( TtPath::DirName( vm.GetAsFromTop<std::string>() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ts" );
        } );

      // -- change_extension 定義
      this->NewSlotOfTopByString(
        Tag::change_extension,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            std::string extension = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();
            std::string path = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();
            vm.Native().PushString( TtPath::ChangeExtension( path, extension ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 3, "tss" );
        } );

      // -- get_execute_file_directory_path 定義
      this->NewSlotOfTopByString(
        Tag::get_execute_file_directory_path,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( TtPath::GetExecutingDirectoryPath() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "t" );
        } );

      // -- get_not_exist_path_from 定義
      this->NewSlotOfTopByString(
        Tag::get_not_exist_path_from,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( Utility::GetNotExistPathFrom( vm.GetAsFromTop<std::string>() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ts" );
        } );

      // -- remove_can_not_use_character_as_file_path_from 定義
      this->NewSlotOfTopByString(
        Tag::remove_can_not_use_character_as_file_path_from,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( Utility::RemoveCanNotUseCharacterAsFilePathFrom( vm.GetAsFromTop<std::string>() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ts" );
        } );

    } ); // end of Path


  // -- BMX2WAV 実装
  this->NewSlotOfRootTableByString(
    Tag::BMX2WAV,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewTable();

      // -- system 定義
      this->NewSlotOfTopByString(
        Tag::system,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            std::string str = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            // 自作 system
            {
              TtPipe pipe;
              TtPipe::Handle for_error = pipe.GetWritePipeHandle().Duplicate( true );
              pipe.GetReadPipeHandle().SetInherit( false );

              TtProcess::CreateInfo tmp_info;
              tmp_info.filename_ = "cmd.exe";
              tmp_info.arguments_ = "/C "+ str;
              tmp_info.show_state_ = TtWindow::ShowState::HIDE;
              tmp_info.use_standard_handles_ = true;

              tmp_info.inherit_handles_ = true;
              tmp_info.standard_output_ = pipe.GetWritePipeHandle();
              tmp_info.standard_error_ = for_error;

              TtProcess process;
              try {
                process.Create( tmp_info );
              }
              catch ( TtWindowsSystemCallException& ) {
                throw TtSquirrel::RuntimeException( "CreateProcess Error", vm.GetCallStack() );
              }
              pipe.GetWritePipeHandle().Close();
              for_error.Close();

              TtString::UniqueString buffer( 1024 );
              for (;;) {
                DWORD received;

                if ( ::ReadFile( pipe.GetReadPipeHandle().GetWindowsHandle(),
                                 buffer.GetPointer(), static_cast<DWORD>( buffer.GetCapacity() ) - 1, &received, NULL ) == 0 ) {
                  break;
                }
                buffer.GetPointer()[received] = '\0';
                vm.CallPrint( buffer.GetPointer() );
              }
              process.Wait();
              pipe.GetReadPipeHandle().Close();
              vm.Native().PushInteger( static_cast<int>( process.GetExitCode() ) );
            }
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, ".s" );
        } );

      // -- ShowState 定義
      this->NewSlotOfTopByString(
        Tag::ShowState,
        [&] () {
          this->Native().NewTable();

          for ( auto& name : TtEnum<TtWindow::ShowState>::GetNames() ) {
            this->NewIntegerSlotOfTopByString( name, static_cast<int>( TtEnum<TtWindow::ShowState>::Parse( name ) ) );
          }
        } );

      // -- create_process 定義
      // create_process( filename, arguments, current_directory, show_state, need_to_wait )
      this->NewSlotOfTopByString(
        Tag::create_process,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            TtProcess::CreateInfo info;

            bool need_to_wait = vm.GetAsFromTop<bool>();
            vm.Native().PopTop();

            info.show_state_ = TtEnum<TtWindow::ShowState>( vm.GetAsFromTop<int>() );
            vm.Native().PopTop();

            if ( vm.GetTopType() != TtSquirrel::ObjectType::Null ) {
              info.current_directory_ = vm.GetAsFromTop<std::string>();
            }
            vm.Native().PopTop();

            if ( vm.GetTopType() != TtSquirrel::ObjectType::Null ) {
              info.arguments_ = vm.GetAsFromTop<std::string>();
            }
            vm.Native().PopTop();

            info.filename_ = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            TtProcess process;
            try {
              process.Create( info );
              if ( need_to_wait ) {
                process.Wait();
                vm.Native().PushInteger( static_cast<int>( process.GetExitCode() ) );
              }
              else {
                vm.Native().PushNull();
              }
            }
            catch ( TtWindowsSystemCallException& ) {
              throw TtSquirrel::RuntimeException( "CreateProcess Error", vm.GetCallStack() );
            }
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          this->Native().SetParamsCheck( 6, ".ss|os|oib" );
        } );

      // -- shell_execute 定義
      this->NewSlotOfTopByString(
        Tag::shell_execute,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [this] ( SquirrelVMBase& vm ) -> int {
            const char* directory_pointer = nullptr;
            std::string directory;
            if ( vm.GetTopType() != TtSquirrel::ObjectType::Null ) {
              directory = vm.GetAsFromTop<std::string>();
              directory_pointer = directory.c_str();
            }
            vm.Native().PopTop();

            const char* parameters_pointer = nullptr;
            std::string parameters;
            if ( vm.GetTopType() != TtSquirrel::ObjectType::Null ) {
              parameters = vm.GetAsFromTop<std::string>();
              parameters_pointer = parameters.c_str();
            }
            vm.Native().PopTop();

            std::string file = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();
            std::string verb = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            auto ret = ::ShellExecute( parent_window_->GetHandle(), verb.c_str(), file.c_str(), parameters_pointer, directory_pointer, SW_SHOWNORMAL );
            vm.Native().PushBoolean( reinterpret_cast<int64_t>( ret ) > 32 );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 5, ".sss|os|o" );
        } );

      // -- expand_environment_string 定義
      this->NewSlotOfTopByString(
        Tag::expand_environment_string,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [this] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( TtUtility::ExpandEnvironmentString( vm.GetAsFromTop<std::string>() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, ".s" );
        } );

      // -- integer_to_word_string 定義
      this->NewSlotOfTopByString(
        Tag::integer_to_word_string,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( BL::Word( vm.GetAsFromTop<int>() ).ToString() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ti" );
        } );

      // -- word_string_to_integer 定義
      this->NewSlotOfTopByString(
        Tag::word_string_to_integer,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushInteger( BL::Word( vm.GetAsFromTop<std::string>() ).ToInteger() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ts" );
        } );

      // -- play_wav_file_async 定義
      this->NewSlotOfTopByString(
        Tag::play_wav_file_async,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [this] ( SquirrelVMBase& vm ) -> int {
            int volume = vm.GetAsFromTop<int>();
            vm.Native().PopTop();
            std::string path = vm.GetAsFromTop<std::string>();
            Core::DirectSoundStream::Player::PlayWavFileAsync( *parent_window_, path, volume );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 3, "tsi" );
        } );

      // -- play_ogg_file_async 定義
      this->NewSlotOfTopByString(
        Tag::play_ogg_file_async,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [this] ( SquirrelVMBase& vm ) -> int {
            int volume = vm.GetAsFromTop<int>();
            vm.Native().PopTop();
            std::string path = vm.GetAsFromTop<std::string>();
            Core::DirectSoundStream::Player::PlayOggFileAsync( *parent_window_, path, volume );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 3, "tsi" );
        } );

    } ); // end of BMX2WAV

  // -- BmsData 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::BmsData,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewClass( false );
      this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<BL::BmsData>() );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

            BL::BmsData& self = *vm.GetAsPointerOf<BL::BmsData>( TtSquirrel::Const::StackTop );
            vm.Native().PopTop();

            vm.SetToTopByString(
              Tag::headers,
              [&] () {
                vm.Native().NewTable();
                for ( auto& one : self.headers_ ) {
                  vm.NewStringSlotOfTopByString( one.first, one.second );
                }
              } );

            vm.SetStringToTopByString(  Tag::path,                         self.path_ );
            vm.SetIntegerToTopByString( Tag::object_count,                 self.GetObjectCount() );
            vm.SetIntegerToTopByString( Tag::object_count_of_1P,           self.GetObjectCountOf1P() );
            vm.SetIntegerToTopByString( Tag::object_count_of_2P,           self.GetObjectCountOf2P() );
            vm.SetIntegerToTopByString( Tag::most_serious_error_level,     self.most_serious_error_level_ );
            vm.SetBooleanToTopByString( Tag::has_random_statement,         self.has_random_statement_ );
            vm.SetIntegerToTopByString( Tag::max_resolution,               self.GetMaxResolution() );
            vm.SetIntegerToTopByString( Tag::bar_number_of_max_resolution, self.GetBarNumberOfMaxResolution() );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xp" );
        } );

      // -- プロパティ
      this->NewNullSlotOfTopByString( Tag::headers );
      this->NewNullSlotOfTopByString( Tag::path );
      this->NewNullSlotOfTopByString( Tag::object_count );
      this->NewNullSlotOfTopByString( Tag::object_count_of_1P );
      this->NewNullSlotOfTopByString( Tag::object_count_of_2P );
      this->NewNullSlotOfTopByString( Tag::most_serious_error_level );
      this->NewNullSlotOfTopByString( Tag::has_random_statement );
      this->NewNullSlotOfTopByString( Tag::max_resolution );
      this->NewNullSlotOfTopByString( Tag::bar_number_of_max_resolution );

      // -- calculate_playing_time -----
      this->NewSlotOfTopByString(
        Tag::calculate_playing_time,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            BL::BmsData& self = *vm.GetInstanceUserPointerAs<BL::BmsData>( TtSquirrel::Const::StackTop );

            try {
              vm.Native().PushFloat( static_cast<float>( self.CalculatePlayingTime() ) );
            }
            catch ( TtException ) {
              vm.Native().PushNull();
            }

            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

    } );


  // -- ConvertException 実装
  this->NewSlotOfRootTableByString(
    Tag::ConvertException,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewClass( false );
      this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<ConvertException>() );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

            ConvertException& self = *vm.GetAsPointerOf<ConvertException>( TtSquirrel::Const::StackTop );
            vm.Native().PopTop();

            self.squirrel_object_ = vm.GetStackTopObject();
            vm.SetStringToTopByString( Tag::message, self.GetMessage() );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xp" );
        } );

      // -- プロパティ
      this->NewNullSlotOfTopByString( Tag::message );

      // -- get_error_level -----
      this->NewSlotOfTopByString(
        Tag::get_error_level,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            ConvertException& self = *vm.GetInstanceUserPointerAs<ConvertException>( TtSquirrel::Const::StackTop );
            vm.Native().PushInteger( self.GetErrorLevel() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- reset_error_level -----
      this->NewSlotOfTopByString(
        Tag::reset_error_level,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            int error_level = vm.GetAsFromTop<int>();
            vm.Native().PopTop();

            ConvertException& self = *vm.GetInstanceUserPointerAs<ConvertException>( TtSquirrel::Const::StackTop );
            self.ResetErrorLevel( TtEnum<ErrorLevel>( error_level ) );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xi" );
        } );
    } );

  // -- BmsDescriptionException 実装
  this->NewSlotOfRootTableByString(
    Tag::BmsDescriptionException,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->GetByStringFromRootTable( Tag::ConvertException );
      this->Native().NewClass( true );
      this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<ConvertBmsDescriptionException>() );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

            ConvertBmsDescriptionException& self = *vm.GetAsPointerOf<ConvertBmsDescriptionException>( TtSquirrel::Const::StackTop );
            vm.Native().PopTop();

            self.squirrel_object_ = vm.GetStackTopObject();
            vm.SetStringToTopByString(  Tag::message, self.GetMessage() );
            vm.SetIntegerToTopByString( Tag::line,    self.GetLine().number_ );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xp" );
        } );

      // -- プロパティ
      this->NewNullSlotOfTopByString( Tag::line );
    } );

  // -- ErrorLevel 実装
  this->NewSlotOfRootTableByString(
    Tag::ErrorLevel,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewTable();

      for ( auto& name : TtEnum<ErrorLevel>::GetNames() ) {
        this->NewIntegerSlotOfTopByString( name, static_cast<int>( TtEnum<ErrorLevel>::Parse( name ) ) );
      }

      // -- error_level_to_string 実装
      this->NewSlotOfTopByString(
        Tag::error_level_to_string,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            int error_level = vm.GetAsFromTop<int>();
            vm.Native().PushString( TtEnum<ErrorLevel>( error_level ).ToString() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ti" );
        } );
    } );

  // -- NormalizeKind 実装
  this->NewSlotOfRootTableByString(
    Tag::NormalizeKind,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewTable();

      for ( auto& name : TtEnum<Core::Parameter::Normalize>::GetNames() ) {
        this->NewIntegerSlotOfTopByString( name, static_cast<int>( TtEnum<Core::Parameter::Normalize>::Parse( name ) ) );
      }

      // -- normalize_kind_to_string 実装
      this->NewSlotOfTopByString(
        Tag::normalize_kind_to_string,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            int normalize_kind = vm.GetAsFromTop<int>();
            vm.Native().PushString( TtEnum<Core::Parameter::Normalize>( normalize_kind ).ToString() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ti" );
        } );
    } );

  // -- SubMenu 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::SubMenu,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewClass( false );

      // -- static Separator 定義 -----
      this->NewNullSlotOfTopByString( Tag::Separator, true );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {

            TtSquirrel::Object items_object = vm.GetStackTopObject();
            vm.Native().PopTop();
            TtSquirrel::Object name_object = vm.GetStackTopObject();
            vm.Native().PopTop();

            vm.SetToTopByString( Tag::name, [&] () { vm.PushObject( name_object ); } );
            vm.SetToTopByString( Tag::items, [&] () { vm.PushObject( items_object ); } );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 3, "xs." );
        } );

      this->NewNullSlotOfTopByString( Tag::name );
      this->NewNullSlotOfTopByString( Tag::items );
    } );

  // ---------------------------------------------------------------------
  // 他ファイルにて実装
  this->InitializeWaveClass();
  this->InitializeStringTable();
}


void
SquirrelVMBase::CallBmsDataContructorAndPushIt( BL::BmsData& bms_data )
{
  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTable( Tag::BmsData ); },
    [&] () {
      this->Native().PushRootTable();
      this->PushAsUserPointer( &bms_data );
      return 2;
    } );
}

void
SquirrelVMBase::CallConvertExceptionConstructorAndPushIt( ConvertException& exception )
{
  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTable( Tag::ConvertException ); },
    [&] () {
      this->Native().PushRootTable();
      this->PushAsUserPointer( &exception );
      return 2;
    } );
}

void
SquirrelVMBase::CallBmsDescriptionExceptionConstructorAndPushIt( ConvertBmsDescriptionException& exception )
{
  this->CallAndPushReturnValue(
    [&] () { this->GetByStringFromRootTable( Tag::BmsDescriptionException ); },
    [&] () {
      this->Native().PushRootTable();
      this->PushAsUserPointer( &exception );
      return 2;
    } );
}
