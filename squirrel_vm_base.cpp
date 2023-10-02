// squirrel_vm_base.cpp

#include <any>

#include "tt_string.h"
#include "tt_process.h"
#include "tt_directory.h"
#include "tt_utility.h"

#include "tt_squirrel_utility.h"

#include "utility.h"
#include "utility_dialogs.h"
#include "base/bmson_parser.h"
#include "core/direct_sound_stream.h"
#include "core/convert_parameter.h"
#include "core/bmson_converter.h"

#include "squirrel_vm_base.h"

using namespace BMX2WAV;


namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( system );
  DEFINE_PARAMETER_NAME_STRING( constructor );
  DEFINE_PARAMETER_NAME_STRING( BMX2WAV );
  DEFINE_PARAMETER_NAME_STRING( integer_to_word_string );
  DEFINE_PARAMETER_NAME_STRING( word_string_to_integer );
  DEFINE_PARAMETER_NAME_STRING( play_wav_file_async );
  DEFINE_PARAMETER_NAME_STRING( play_ogg_file_async );
  DEFINE_PARAMETER_NAME_STRING( SubMenu );
  DEFINE_PARAMETER_NAME_STRING( shell_execute );
  DEFINE_PARAMETER_NAME_STRING( create_process );
  DEFINE_PARAMETER_NAME_STRING( Separator );
  DEFINE_PARAMETER_NAME_STRING( name );
  DEFINE_PARAMETER_NAME_STRING( items );
  DEFINE_PARAMETER_NAME_STRING( ErrorLevel );
  DEFINE_PARAMETER_NAME_STRING( error_level_to_string );
  DEFINE_PARAMETER_NAME_STRING( ShowState );
  DEFINE_PARAMETER_NAME_STRING( expand_environment_string );
  DEFINE_PARAMETER_NAME_STRING( ConvertException );
  DEFINE_PARAMETER_NAME_STRING( message );
  DEFINE_PARAMETER_NAME_STRING( get_error_level );
  DEFINE_PARAMETER_NAME_STRING( reset_error_level );
  DEFINE_PARAMETER_NAME_STRING( BmsDescriptionException );
  DEFINE_PARAMETER_NAME_STRING( line );
  DEFINE_PARAMETER_NAME_STRING( NormalizeKind );
  DEFINE_PARAMETER_NAME_STRING( normalize_kind_to_string );
  DEFINE_PARAMETER_NAME_STRING( Utility );
  DEFINE_PARAMETER_NAME_STRING( convert_bmson_data_to_bms_data );
  DEFINE_PARAMETER_NAME_STRING( get_directory_entries );
  DEFINE_PARAMETER_NAME_STRING( UserInputDialog );
  DEFINE_PARAMETER_NAME_STRING( UserInputDialogNumber );
  DEFINE_PARAMETER_NAME_STRING( title );
  DEFINE_PARAMETER_NAME_STRING( explanation );
  DEFINE_PARAMETER_NAME_STRING( input );
  DEFINE_PARAMETER_NAME_STRING( show_dialog );
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
print_function_( nullptr )
{
  HANDLE_TABLE.insert( {this->GetHandle(), this} );

  this->SetParentWindow( parent_window );

  this->RegisterStandardLibrariesAndAllAdditionalLibraries();
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
  // -- BMX2WAV ���� -----
  this->NewSlotOfRootTableByString(
    Tag::BMX2WAV,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewTable();

      // -- system ��`
      this->NewSlotOfTopByString(
        Tag::system,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            std::string str = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            // ���� system
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
                std::string message = "CreateProcess Error";
                vm.Native().ThrowError( message );
                throw TtSquirrel::RuntimeException( message, vm.GetCallStack() );
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

      // -- ShowState ��`
      this->NewSlotOfTopByString(
        Tag::ShowState,
        [&] () {
          this->Native().NewTable();

          for ( auto& name : TtEnum<TtWindow::ShowState>::GetNames() ) {
            this->NewIntegerSlotOfTopByString( name, static_cast<int>( TtEnum<TtWindow::ShowState>::Parse( name ) ) );
          }
        } );

      // -- create_process ��`
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
              std::string message = "CreateProcess Error";
              vm.Native().ThrowError( message );
              throw TtSquirrel::RuntimeException( message, vm.GetCallStack() );
            }
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          this->Native().SetParamsCheck( 6, ".ss|os|oib" );
        } );

      // -- shell_execute ��`
      this->NewSlotOfTopByString(
        Tag::shell_execute,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
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

            auto ret = ::ShellExecute( vm.GetParentWindow()->GetHandle(), verb.c_str(), file.c_str(), parameters_pointer, directory_pointer, SW_SHOWNORMAL );
            vm.Native().PushBoolean( reinterpret_cast<int64_t>( ret ) > 32 );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 5, ".sss|os|o" );
        } );

      // -- expand_environment_string ��`
      this->NewSlotOfTopByString(
        Tag::expand_environment_string,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( TtUtility::ExpandEnvironmentString( vm.GetAsFromTop<std::string>() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, ".s" );
        } );

      // -- integer_to_word_string ��`
      this->NewSlotOfTopByString(
        Tag::integer_to_word_string,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( BL::Word( vm.GetAsFromTop<int>() ).ToString() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ti" );
        } );

      // -- word_string_to_integer ��`
      this->NewSlotOfTopByString(
        Tag::word_string_to_integer,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushInteger( BL::Word( vm.GetAsFromTop<std::string>() ).ToInteger() );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ts" );
        } );

      // -- play_wav_file_async ��`
      this->NewSlotOfTopByString(
        Tag::play_wav_file_async,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            int volume = vm.GetAsFromTop<int>();
            vm.Native().PopTop();
            std::string path = vm.GetAsFromTop<std::string>();
            Core::DirectSoundStream::Player::PlayWavFileAsync( *vm.GetParentWindow(), path, volume );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 3, "tsi" );
        } );

      // -- play_ogg_file_async ��`
      this->NewSlotOfTopByString(
        Tag::play_ogg_file_async,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            int volume = vm.GetAsFromTop<int>();
            vm.Native().PopTop();
            std::string path = vm.GetAsFromTop<std::string>();
            Core::DirectSoundStream::Player::PlayOggFileAsync( *vm.GetParentWindow(), path, volume );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 3, "tsi" );
        } );

      // -- get_directory_entries ��`
      this->NewSlotOfTopByString(
        Tag::get_directory_entries,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            TtDirectory dir( vm.GetAsFromTop<std::string>() );
            std::vector<std::string> v = dir.GetEntries( "*" );
            vm.NewArrayOf( v, [&] ( const std::string& file ) {
              vm.Native().PushString( file );
            } );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ts" );
        } );

    } ); // end of BMX2WAV


  // -- ConvertException ����
  this->NewSlotOfRootTableByString(
    Tag::ConvertException,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewClass( false );
      this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<ConvertException>() );

      // -- constructor ��` -----
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

      // -- �v���p�e�B
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

  // -- BmsDescriptionException ����
  this->NewSlotOfRootTableByString(
    Tag::BmsDescriptionException,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->GetByStringFromRootTable( Tag::ConvertException );
      this->Native().NewClass( true );
      this->Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<ConvertBmsDescriptionException>() );

      // -- constructor ��` -----
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

      // -- �v���p�e�B
      this->NewNullSlotOfTopByString( Tag::line );
    } );

  // -- ErrorLevel ����
  this->NewSlotOfRootTableByString(
    Tag::ErrorLevel,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewTable();

      for ( auto& name : TtEnum<ErrorLevel>::GetNames() ) {
        this->NewIntegerSlotOfTopByString( name, static_cast<int>( TtEnum<ErrorLevel>::Parse( name ) ) );
      }

      // -- error_level_to_string ����
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

  // -- NormalizeKind ����
  this->NewSlotOfRootTableByString(
    Tag::NormalizeKind,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewTable();

      for ( auto& name : TtEnum<Core::Parameter::Normalize>::GetNames() ) {
        this->NewIntegerSlotOfTopByString( name, static_cast<int>( TtEnum<Core::Parameter::Normalize>::Parse( name ) ) );
      }

      // -- normalize_kind_to_string ����
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

  // -- SubMenu ���� ----------
  this->NewSlotOfRootTableByString(
    Tag::SubMenu,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewClass( false );

      // -- static Separator ��` -----
      this->NewNullSlotOfTopByString( Tag::Separator, true );

      // -- constructor ��` -----
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

  {
    // -- UserInputDialog 2��
    auto make_closure = [] <class UserInputDialogType> ( TtUtility::TypeTag<UserInputDialogType> ) {
      return SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
        auto get_member = [&] ( const std::string& key ) -> std::string {
          TtSquirrel::StackRecoverer recoverer( &vm );
          vm.GetByStringFromTop( key );
          return (vm.GetTopType() == TtSquirrel::ObjectType::String) ? vm.GetAsFromTop<std::string>() : "";
        };

        UserInputDialogType dialog;
        dialog.SetTitle( get_member( Tag::title ) );
        dialog.SetExplanation( get_member( Tag::explanation ) );
        int ret = dialog.ShowDialog( *vm.GetParentWindow() );
        if ( ret == 0 ) {
          vm.SetStringToTopByString( Tag::input, dialog.GetInput() );
        }
        vm.Native().PushBoolean( ret );
        return TtSquirrel::Const::ExistReturnValue;
      } );
    };

    // -- UserInputDialog ���� -----
    this->NewSlotOfRootTableByString(
      Tag::UserInputDialog,
      [&] () {
        TtSquirrel::StackRecoverer recoverer( this, 1 );
        this->Native().NewClass( false );

        // -- constructor ��` -----
        this->NewSlotOfTopByString(
          Tag::constructor,
          [&] () {
            this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
              while ( vm.Native().GetTopIndex() > 3 ) {
                vm.Native().PopTop();
              }

              switch ( vm.Native().GetTopIndex() ) {
              case 1:
                vm.Native().PushNull();
              case 2:
                vm.Native().PushNull();
              }

              TtSquirrel::Object explanation = vm.GetStackTopObject();
              vm.Native().PopTop();
              TtSquirrel::Object title = vm.GetStackTopObject();
              vm.Native().PopTop();

              vm.SetToTopByString( Tag::title,       [&] () { vm.PushObject( title ); } );
              vm.SetToTopByString( Tag::explanation, [&] () { vm.PushObject( explanation ); } );

              return TtSquirrel::Const::NoneReturnValue;
            } ) );
            Native().SetParamsCheck( -1, "xss" );
          } );

        // -- show_dialog ��` -----
        this->NewSlotOfTopByString(
          Tag::show_dialog,
          [&] () {
            this->NewClosure( make_closure( TtUtility::TypeTag<UserInputDialog>() ) );
            Native().SetParamsCheck( 1, "x" );
          } );

        this->NewNullSlotOfTopByString( Tag::title );
        this->NewNullSlotOfTopByString( Tag::explanation );
        this->NewNullSlotOfTopByString( Tag::input );
      } );

    // -- UserInputDialogNumber ���� -----
    this->NewSlotOfRootTableByString(
      Tag::UserInputDialogNumber,
      [&] () {
        TtSquirrel::StackRecoverer recoverer( this, 1 );
        this->GetByStringFromRootTable( Tag::UserInputDialog );
        this->Native().NewClass( true );

        // -- show_dialog ��` -----
        this->NewSlotOfTopByString(
          Tag::show_dialog,
          [&] () {
            this->NewClosure( make_closure( TtUtility::TypeTag<UserInputDialogNumber>() ) );
            Native().SetParamsCheck( 1, "x" );
          } );
      } );
  }

  // -- Utility ���� -----
  this->NewSlotOfRootTableByString(
    Tag::Utility,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewTable();

      // -- convert_bmson_data_to_bms_data ��`
      this->NewSlotOfTopByString(
        Tag::convert_bmson_data_to_bms_data,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            std::string output_dir = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();
            std::string input_file = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            try {
              BL::Bmson::Parser bmson_parser;
              std::shared_ptr<BL::Bmson::BmsonData> bmson_data = bmson_parser.Parse( input_file );

              Core::BmsonConverter bmson_converter;
              bmson_converter.ConvertToFileAndWave( *bmson_data, input_file, output_dir );
            }
            catch ( Exception& e ) {
              vm.Native().ThrowError( e.GetMessage() );
              throw TtSquirrel::RuntimeException( e.GetMessage(), vm.GetCallStack() );
            }
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 3, ".ss" );
        } );
    } );


  // ---------------------------------------------------------------------
  // ���t�@�C���ɂĎ���
  this->InitializeBmsDataClass();
  this->InitializeWaveClass();
  this->InitializeStringTable();
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
