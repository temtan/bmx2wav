// squirrel_vm_string_table.cpp

#include "string_table.h"
#include "squirrel_vm_base.h"

using namespace BMX2WAV;


namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  // DEFINE_PARAMETER_NAME_STRING(  );
  DEFINE_PARAMETER_NAME_STRING( StringTable );
  DEFINE_PARAMETER_NAME_STRING( get_by_number );
  DEFINE_PARAMETER_NAME_STRING( ID );
  DEFINE_PARAMETER_NAME_STRING( number );
  DEFINE_PARAMETER_NAME_STRING( get );
  DEFINE_PARAMETER_NAME_STRING( constructor );
  DEFINE_PARAMETER_NAME_STRING( ParameterPropertySheet );
  DEFINE_PARAMETER_NAME_STRING( PPS );
  DEFINE_PARAMETER_NAME_STRING( Searcher );
  DEFINE_PARAMETER_NAME_STRING( SettingsPropertySheet );
  DEFINE_PARAMETER_NAME_STRING( SPS );
  DEFINE_PARAMETER_NAME_STRING( StrT );
}


// -- SquirrelVM ---------------------------------------------------------
void
SquirrelVMBase::InitializeStringTable( void )
{
  this->NewSlotOfRootTableByString(
    Tag::StringTable,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      this->Native().NewTable();

      // -- get_by_number 実装
      this->NewSlotOfTopByString(
        Tag::get_by_number,
        [&] () {
          this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
            vm.Native().PushString( StringTable::Get( vm.GetAsFromTop<int>() ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, ".i" );
        } );

      // -- ID 実装
      this->NewSlotOfTopByString(
        Tag::ID,
        [&] () {
          TtSquirrel::StackRecoverer recoverer( this, 1 );
          Native().NewClass( false );

          // -- constructor 定義 -----
          this->NewSlotOfTopByString(
            Tag::constructor,
            [&] () {
              this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
                auto number = vm.GetAsFromTop<int>();
                vm.Native().PopTop();

                vm.SetToTopByString( Tag::number, [&] () { vm.Native().PushInteger( number ); } );

                return TtSquirrel::Const::NoneReturnValue;
              } ) );
              Native().SetParamsCheck( 2, "xi" );
            } );

          // -- get 実装 -----
          this->NewSlotOfTopByString(
            Tag::get,
            [&] () {
              this->NewClosure( SquirrelVMBase::ConvertClosure( [] ( SquirrelVMBase& vm ) -> int {
                vm.Native().PushString( StringTable::Get( vm.GetByStringFromTopAndGetAs<int>( Tag::number ) ) );
                return TtSquirrel::Const::ExistReturnValue;
              } ) );
              Native().SetParamsCheck( 1, "x" );
            } );

          // -- プロパティ
          this->NewNullSlotOfTopByString( Tag::number );
        } );
    } );


  // -----------------------------------------------------------------
  // ID 登録
  auto register_id = [&] ( const StringTable::ID id, const char* cur ) {
    TtSquirrel::StackRecoverer recoverer( this );
    this->Native().PushRootTable();
    std::string key;
    for (;;) {
      if ( *cur == '\0' ) {
        this->NewSlotOfTopByString( key, [&] () {
          this->CallAndPushReturnValue(
            [&] () { this->GetByStringFromRootTableJoined( {Tag::StringTable, Tag::ID} ); },
            [&] () {
              this->Native().PushRootTable();
              this->Native().PushInteger( id.number_ );
              return 2;
            } );
        } );
        return;
      }
      else if ( *cur == ':' ) {
        try {
          TtSquirrel::StackRecoverer recoverer2( this );
          this->GetByStringFromTop( key );
        }
        catch ( TtSquirrel::Exception ) {
          this->NewSlotOfTopByString( key, [&] () {
            this->Native().NewTable();
          } );
        }
        this->GetByStringFromTop( key );
        key.clear();
        cur += 2;
      }
      else {
        key.append( 1, *cur );
        cur += 1;
      }
    }
  };

#define REGISTER_ID( id ) register_id( id, #id )
  // ---------------------------------------------------------------------
  // main frame
  // ----------------
  // menu
  REGISTER_ID( StringTable::Main::Menu::File );
  REGISTER_ID( StringTable::Main::Menu::FileClose );
  REGISTER_ID( StringTable::Main::Menu::View );
  REGISTER_ID( StringTable::Main::Menu::ViewAutoDisplayCells );
  REGISTER_ID( StringTable::Main::Menu::ViewColumnGroup );
  REGISTER_ID( StringTable::Main::Menu::ViewDisplayCells );
  REGISTER_ID( StringTable::Main::Menu::ViewReload );
  REGISTER_ID( StringTable::Main::Menu::Edit );
  REGISTER_ID( StringTable::Main::Menu::EditEditCommonParameter );
  REGISTER_ID( StringTable::Main::Menu::EditAddEntry );
  REGISTER_ID( StringTable::Main::Menu::EditDeleteEntry );
  REGISTER_ID( StringTable::Main::Menu::EditDeleteAllEntry );
  REGISTER_ID( StringTable::Main::Menu::EditShowPropertyOfEnty );
  REGISTER_ID( StringTable::Main::Menu::EditEditIndividualParameter );
  REGISTER_ID( StringTable::Main::Menu::Convert );
  REGISTER_ID( StringTable::Main::Menu::ConvertConvertOneBms );
  REGISTER_ID( StringTable::Main::Menu::ConvertConvertAllBms );
  REGISTER_ID( StringTable::Main::Menu::Tool );
  REGISTER_ID( StringTable::Main::Menu::ToolSettings );
  REGISTER_ID( StringTable::Main::Menu::ToolReloadSquirrelScript );
  REGISTER_ID( StringTable::Main::Menu::ToolShowSquirrelOutputDialog );
  REGISTER_ID( StringTable::Main::Menu::ToolStartSearcher );
  REGISTER_ID( StringTable::Main::Menu::Help );
  REGISTER_ID( StringTable::Main::Menu::HelpVersionInformation );

  // ----------------
  // tool bar
  REGISTER_ID( StringTable::Main::Toolbar::AutoDisplayCells );
  REGISTER_ID( StringTable::Main::Toolbar::SelectColumnLael );
  REGISTER_ID( StringTable::Main::Toolbar::DisplayCells );
  REGISTER_ID( StringTable::Main::Toolbar::Reload );
  REGISTER_ID( StringTable::Main::Toolbar::EditCommonParameter );
  REGISTER_ID( StringTable::Main::Toolbar::AddEntry );
  REGISTER_ID( StringTable::Main::Toolbar::DeleteEntry );
  REGISTER_ID( StringTable::Main::Toolbar::DeleteAllEntry );
  REGISTER_ID( StringTable::Main::Toolbar::ShowPropertyOfEntry );
  REGISTER_ID( StringTable::Main::Toolbar::EditIndividualParameter );
  REGISTER_ID( StringTable::Main::Toolbar::ConvertOneBms );
  REGISTER_ID( StringTable::Main::Toolbar::ConvertAllBms );
  REGISTER_ID( StringTable::Main::Toolbar::ReloadSquirrelScript );
  REGISTER_ID( StringTable::Main::Toolbar::ShowSquirrelOutputDialog );
  REGISTER_ID( StringTable::Main::Toolbar::StartSearcher );

  // ----------------
  // popup entry menu
  REGISTER_ID( StringTable::Main::PopupEntry::ShowPropertyOfEntry );
  REGISTER_ID( StringTable::Main::PopupEntry::EditIndividualParameter );
  REGISTER_ID( StringTable::Main::PopupEntry::ConvertOneBms );
  REGISTER_ID( StringTable::Main::PopupEntry::DeleteEntry );
  REGISTER_ID( StringTable::Main::PopupEntry::ProcessorMenu );

  REGISTER_ID( StringTable::Main::PopupEntry::ProcessorMenuPlayPreview );
  REGISTER_ID( StringTable::Main::PopupEntry::ProcessorMenuOpen );
  REGISTER_ID( StringTable::Main::PopupEntry::ProcessorMenuOpenByNotepad );
  REGISTER_ID( StringTable::Main::PopupEntry::ProcessorMenuFileOperations );

  // ----------------
  // column name (use at script)
  REGISTER_ID( StringTable::Main::ColumnName::ObjectCount );
  REGISTER_ID( StringTable::Main::ColumnName::ObjectOf1PCount );
  REGISTER_ID( StringTable::Main::ColumnName::ObjectOf2PCount );
  REGISTER_ID( StringTable::Main::ColumnName::MostSeriousError );
  REGISTER_ID( StringTable::Main::ColumnName::RandomStatement );
  REGISTER_ID( StringTable::Main::ColumnName::RandomStatementExist );
  REGISTER_ID( StringTable::Main::ColumnName::RandomStatementNotExist );
  REGISTER_ID( StringTable::Main::ColumnName::FullPath );
  REGISTER_ID( StringTable::Main::ColumnName::Basename );
  REGISTER_ID( StringTable::Main::ColumnName::ParentDirectory );
  REGISTER_ID( StringTable::Main::ColumnName::ParentsParentDirectory );
  REGISTER_ID( StringTable::Main::ColumnName::PlayingTime );

  REGISTER_ID( StringTable::Main::ColumnName::Parameter );
  REGISTER_ID( StringTable::Main::ColumnName::ParameterIndividual );
  REGISTER_ID( StringTable::Main::ColumnName::ParameterCommon );

  // column group name (use at script)
  // Searcher 兼用
  REGISTER_ID( StringTable::Main::ColumnGroupName::Simple );
  REGISTER_ID( StringTable::Main::ColumnGroupName::Details );
  REGISTER_ID( StringTable::Main::ColumnGroupName::Various );
  REGISTER_ID( StringTable::Main::ColumnGroupName::ForSearch );

  // ----------------
  // main frame
  REGISTER_ID( StringTable::Main::MBScriptErrorCaption );
  REGISTER_ID( StringTable::Main::MBScriptErrorMessage );
  REGISTER_ID( StringTable::Main::SquirrelStandardOutputDialogTitle );
  REGISTER_ID( StringTable::Main::DefaultSortMethodName );
  REGISTER_ID( StringTable::Main::StatusBarSortMethodTitle );
  REGISTER_ID( StringTable::Main::DefaultFirstColumnName );
  REGISTER_ID( StringTable::Main::StatusBarSortMethodDescending );
  REGISTER_ID( StringTable::Main::AddEntryFileDialogBmsBmeFile );
  REGISTER_ID( StringTable::Main::AddEntryFileDialogAllFile );
  REGISTER_ID( StringTable::Main::MBDeleteAllEntryCautionCaption );
  REGISTER_ID( StringTable::Main::MBDeleteAllEntryCautionMessage );
  REGISTER_ID( StringTable::Main::MBEditIndividualParameterCopyCautionCaption );
  REGISTER_ID( StringTable::Main::MBEditIndividualParameterCopyCautionMessage );
  REGISTER_ID( StringTable::Main::MBStartSearcherErrorCaption );
  REGISTER_ID( StringTable::Main::MBStartSearcherErrorMessage );
  REGISTER_ID( StringTable::Main::EntryProcessorMenuEmpty );

  // ---------------------------------------------------------------------
  // settings
  // reserve

  // ---------------------------------------------------------------------
  // parameter property sheet
  // Abbreviation = PPS
  REGISTER_ID( StringTable::ParameterPropertySheet::Title );

  REGISTER_ID( StringTable::ParameterPropertySheet::General );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralLoadSaveGroup );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralLoadButton );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralSaveButton );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralNameLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralUseScriptCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralScriptLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralScriptRefButton );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralOutputLogCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralLogLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralLogRefButton );
  REGISTER_ID( StringTable::ParameterPropertySheet::GeneralFileDialogAllFile );

  REGISTER_ID( StringTable::ParameterPropertySheet::Output );
  REGISTER_ID( StringTable::ParameterPropertySheet::OutputOutputAsOggCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::OutputOggBaseQualityLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::OutputNeverOverwriteCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::OutputRemoveCharCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::OutputOutputFileTemplateLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::OutputOutputFileHelpButton );
  REGISTER_ID( StringTable::ParameterPropertySheet::OutputMBOggBaseQualityCaption );
  REGISTER_ID( StringTable::ParameterPropertySheet::OutputMBOggBaseQualityMessage );

  REGISTER_ID( StringTable::ParameterPropertySheet::Parser );
  REGISTER_ID( StringTable::ParameterPropertySheet::ParserIgnoreBGAChannelCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::ParserNotNestingIfStatementCheck );

  REGISTER_ID( StringTable::ParameterPropertySheet::Mixin );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinCancelAtResoundingCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinStartLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinEndLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinConvertStartEndBarNumberLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinDoTrimingCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinRemoveFrontSilenceCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinInsertFrontSilenceCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinInsertFrontSilenceSecondLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinMBKillActiveCaption );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinMBKillActiveMessageStart );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinMBKillActiveMessageEnd );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinMBKillActiveMessageOrder );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinMBKillActiveMessageTrimStart );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinMBKillActiveMessageTrimEnd );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinMBKillActiveMessageTrimOrder );
  REGISTER_ID( StringTable::ParameterPropertySheet::MixinMBKillActiveMessageFrontSilence );

  REGISTER_ID( StringTable::ParameterPropertySheet::Audio );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioNormalizeKindNoneRadio );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioNormalizeKindPeakRadio );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioNormalizeKindAverageRadio );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioNormalizeKindOverRadio );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioNormalizeKindGroup );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioNormalizeOverPPMLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioVolumeLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioMBKillActiveCaption );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioMBKillActiveMessageOverPPM );
  REGISTER_ID( StringTable::ParameterPropertySheet::AudioMBKillActiveMessageVolume );

  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcess );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessExecuteAfterProcessCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessExecuteFileLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessExecuteFileRefButton );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessExecuteArgumentsLabel );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessExecuteArgumentsHelp );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessStartOnBackgroundCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessWaitForProcessExitCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessDeleteOutputFileCheck );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessFileDialogExeFile );
  REGISTER_ID( StringTable::ParameterPropertySheet::AfterProcessFileDialogAllFile );

  // ---------------------------------------------------------------------
  // convert dialog
  REGISTER_ID( StringTable::Convert::Title );

  REGISTER_ID( StringTable::Convert::AbortButton );
  REGISTER_ID( StringTable::Convert::InitializeTitleLabel );
  REGISTER_ID( StringTable::Convert::InitializeReportLabel );
  REGISTER_ID( StringTable::Convert::ParseTitleLabel );
  REGISTER_ID( StringTable::Convert::ParseReportLabel );
  REGISTER_ID( StringTable::Convert::WavTitleLabel );
  REGISTER_ID( StringTable::Convert::WavReportLabel );
  REGISTER_ID( StringTable::Convert::MixinTitleLabel );
  REGISTER_ID( StringTable::Convert::MixinReportLabel );
  REGISTER_ID( StringTable::Convert::AfterTitleLabel );
  REGISTER_ID( StringTable::Convert::AfterReportLabel );
  REGISTER_ID( StringTable::Convert::PlayWavButton );
  REGISTER_ID( StringTable::Convert::StopWavButton );
  REGISTER_ID( StringTable::Convert::SaveAsWavButton );
  REGISTER_ID( StringTable::Convert::SaveAsOggButton );
  REGISTER_ID( StringTable::Convert::OpenWavButton );
  REGISTER_ID( StringTable::Convert::ShowOutputBoxButton );

  REGISTER_ID( StringTable::Convert::FileDialogWavFile );
  REGISTER_ID( StringTable::Convert::FileDialogOggFile );
  REGISTER_ID( StringTable::Convert::FileDialogAllFile );
  REGISTER_ID( StringTable::Convert::MBOpenWavCaption );
  REGISTER_ID( StringTable::Convert::MBOpenWavMessage );
  REGISTER_ID( StringTable::Convert::OutputDialogTitle );
  REGISTER_ID( StringTable::Convert::OpenLogErrorMessage );
  REGISTER_ID( StringTable::Convert::OutputToLogError );

  REGISTER_ID( StringTable::Convert::MBErrorAbortCaption );
  REGISTER_ID( StringTable::Convert::MBErrorAbortMessage );
  REGISTER_ID( StringTable::Convert::MBErrorMemoryCaption );
  REGISTER_ID( StringTable::Convert::MBErrorMemoryMessage );
  REGISTER_ID( StringTable::Convert::MBErrorUnexpectedCaption );
  REGISTER_ID( StringTable::Convert::MBErrorUnexpectedMessage );
  REGISTER_ID( StringTable::Convert::MBErrorScriptCaption );
  REGISTER_ID( StringTable::Convert::MBErrorScriptMessage );
  REGISTER_ID( StringTable::Convert::MBInfoRandomDialogCaption );
  REGISTER_ID( StringTable::Convert::MBInfoRandomDialogMessage );

  REGISTER_ID( StringTable::Convert::ReportLabelCompleted );
  REGISTER_ID( StringTable::Convert::ReportLabelErrorExist );
  REGISTER_ID( StringTable::Convert::ReportLabelAbort );
  REGISTER_ID( StringTable::Convert::ReportResultAbort );
  REGISTER_ID( StringTable::Convert::ReportLabelInitializeRunning );
  REGISTER_ID( StringTable::Convert::ReportResultInitializeRunning );
  REGISTER_ID( StringTable::Convert::ReportLabelParseRunning );
  REGISTER_ID( StringTable::Convert::ReportResultParseRunning );
  REGISTER_ID( StringTable::Convert::ReportLabelReadAudioFilesRunning );
  REGISTER_ID( StringTable::Convert::ReportResultReadAudioFilesRunning );
  REGISTER_ID( StringTable::Convert::ReportLabelOneAudioFileReadRunning );
  REGISTER_ID( StringTable::Convert::ReportLabelMixinRunning );
  REGISTER_ID( StringTable::Convert::ReportResultMixinRunning );
  REGISTER_ID( StringTable::Convert::ReportLabelAffectWaveRunning );
  REGISTER_ID( StringTable::Convert::ReportResultAffectWaveRunning );
  REGISTER_ID( StringTable::Convert::ReportResultOutputToFileRunning );
  REGISTER_ID( StringTable::Convert::ReportLabelAfterOutputToFileNoOutput );
  REGISTER_ID( StringTable::Convert::ReportResultAfterOutputToFileCompleted );

  // ---------------------------------------------------------------------
  // multiple convert dialog
  REGISTER_ID( StringTable::Multiple::Title );
  REGISTER_ID( StringTable::Multiple::MenuShowLog );
  REGISTER_ID( StringTable::Multiple::AbortButton );
  REGISTER_ID( StringTable::Multiple::ColumnBms );
  REGISTER_ID( StringTable::Multiple::ColumnResult );
  REGISTER_ID( StringTable::Multiple::ColumnMostSeriousError );
  REGISTER_ID( StringTable::Multiple::ColumnDefaultTextUnconverted );
  REGISTER_ID( StringTable::Multiple::LogDioalogTitle );
  REGISTER_ID( StringTable::Multiple::StatusTextConverting );
  REGISTER_ID( StringTable::Multiple::StatusTextAborted );
  REGISTER_ID( StringTable::Multiple::StatusTextCompleted );

  // ---------------------------------------------------------------------
  // entry dialog
  REGISTER_ID( StringTable::Entry::Title );
  REGISTER_ID( StringTable::Entry::UsingParameterTitleLabel );
  REGISTER_ID( StringTable::Entry::UseCommonParameterButton );
  REGISTER_ID( StringTable::Entry::UseIndividualParameterButton );
  REGISTER_ID( StringTable::Entry::ConvertButton );
  REGISTER_ID( StringTable::Entry::ColumnName );
  REGISTER_ID( StringTable::Entry::ColumnValue );
  REGISTER_ID( StringTable::Entry::UsingParameterLabelCommon );
  REGISTER_ID( StringTable::Entry::UsingParameterLabelIndividual );
  REGISTER_ID( StringTable::Entry::MBUseCommonCaption );
  REGISTER_ID( StringTable::Entry::MBUseCommonMessage );
  REGISTER_ID( StringTable::Entry::MBUseIndividualCaption );
  REGISTER_ID( StringTable::Entry::MBUseIndividualMessage );

  // ---------------------------------------------------------------------
  // random statement dialog
  REGISTER_ID( StringTable::Random::Title );
  REGISTER_ID( StringTable::Random::CheckButton );
  REGISTER_ID( StringTable::Random::OKButton );

  // ---------------------------------------------------------------------
  // template path translate help dialog
  REGISTER_ID( StringTable::TemplatePath::Title );
  REGISTER_ID( StringTable::TemplatePath::MenuCopy );
  REGISTER_ID( StringTable::TemplatePath::ColumnName );
  REGISTER_ID( StringTable::TemplatePath::ColumnDescription );
  REGISTER_ID( StringTable::TemplatePath::TextInputBmsPath );
  REGISTER_ID( StringTable::TemplatePath::TextInputBmsPathWithoutExtension );
  REGISTER_ID( StringTable::TemplatePath::TextInputBmsDirectory );
  REGISTER_ID( StringTable::TemplatePath::TextInputBmsParentDirectory );
  REGISTER_ID( StringTable::TemplatePath::TextInputBmsBasename );
  REGISTER_ID( StringTable::TemplatePath::TextInputBmsBasenameWithoutExtension );
  REGISTER_ID( StringTable::TemplatePath::TextInputBmsAutoExtensionChange );
  REGISTER_ID( StringTable::TemplatePath::TextInputBmsBasenameAutoExtensionChange );
  REGISTER_ID( StringTable::TemplatePath::TextAutoExtension );
  REGISTER_ID( StringTable::TemplatePath::TextBMX2WAVDirectory );
  REGISTER_ID( StringTable::TemplatePath::TextHeaderXXXXX );


  // ---------------------------------------------------------------------
  // ===== searcher =====
  // ---------------------------------------------------------------------
  // searcher main frame
  // ----------------
  // searcher main menu
  REGISTER_ID( StringTable::Searcher::Main::Menu::File );
  REGISTER_ID( StringTable::Searcher::Main::Menu::FileClose );
  REGISTER_ID( StringTable::Searcher::Main::Menu::FileExport );
  REGISTER_ID( StringTable::Searcher::Main::Menu::FileExportCSVToFile );
  REGISTER_ID( StringTable::Searcher::Main::Menu::FileExportTSVToClipboard );
  REGISTER_ID( StringTable::Searcher::Main::Menu::Tool );
  REGISTER_ID( StringTable::Searcher::Main::Menu::ToolSettings );

  // ----------------
  // searcher tree menu
  REGISTER_ID( StringTable::Searcher::Main::PopupTree::OpenDirectory );

  // ----------------
  // searcher entry menu
  REGISTER_ID( StringTable::Searcher::Main::PopupEntry::HideEntry );
  REGISTER_ID( StringTable::Searcher::Main::PopupEntry::ProcessorMenu );

  // BMX2WAV 本体兼用
  REGISTER_ID( StringTable::Searcher::Main::PopupEntry::ProcessorMenuPlayPreview );
  REGISTER_ID( StringTable::Searcher::Main::PopupEntry::ProcessorMenuOpen );
  REGISTER_ID( StringTable::Searcher::Main::PopupEntry::ProcessorMenuOpenByNotepad );
  REGISTER_ID( StringTable::Searcher::Main::PopupEntry::ProcessorMenuFileOperations );

  // ----------------
  // searcher tool bar
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::ReloadSquirrelScript );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::ShowSquirrelOutputDialog );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::Export );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::FilterLabel );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::AutoDisplayCells );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::SelectColumnLabel );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::DisplayCells );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::ExecuteSearchLabel );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::ExecuteSearch );

  // display filter (use at script)
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::DisplayFilterAllPass );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::DisplayFilterBmsGeneral );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::DisplayFilterOnlyBms );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::DisplayFilterOnlyBme );

  // search method (use at script)
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::SearchMethodBmsAll );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::SearchMethodFilterBmsGeneral );
  REGISTER_ID( StringTable::Searcher::Main::Toolbar::SearchMethodMostPlaylevel );

  // ----------------
  // column group name (use at script)
  // Searcher 兼用
  REGISTER_ID( StringTable::Main::ColumnGroupName::Simple );
  REGISTER_ID( StringTable::Main::ColumnGroupName::Details );
  REGISTER_ID( StringTable::Main::ColumnGroupName::Various );
  REGISTER_ID( StringTable::Main::ColumnGroupName::ForSearch );

  // ----------------
  // searcher main frame
  REGISTER_ID( StringTable::Searcher::Main::MBScriptErrorCaption );
  REGISTER_ID( StringTable::Searcher::Main::MBScriptErrorMessage );
  REGISTER_ID( StringTable::Searcher::Main::SquirrelStandardOutputDialogTitle );
  REGISTER_ID( StringTable::Searcher::Main::DefaultSortMethodName );
  REGISTER_ID( StringTable::Searcher::Main::StatusBarSortMethodTitle );
  REGISTER_ID( StringTable::Searcher::Main::EntryProcessorMenuEmpty );
  REGISTER_ID( StringTable::Searcher::Main::StatusBarSortMethodDescending );
  REGISTER_ID( StringTable::Searcher::Main::DefaultFirstColumnName );
  REGISTER_ID( StringTable::Searcher::Main::MBExecuteSearchImpossibleDirectoryCaption );
  REGISTER_ID( StringTable::Searcher::Main::MBExecuteSearchImpossibleDirectoryMessage );
  REGISTER_ID( StringTable::Searcher::Main::MBExecuteSearchExecuteConfirmCaption );
  REGISTER_ID( StringTable::Searcher::Main::MBExecuteSearchExecuteConfirmMessage );
  REGISTER_ID( StringTable::Searcher::Main::MBSearchResultDestructionConfirmCaption );
  REGISTER_ID( StringTable::Searcher::Main::MBSearchResultDestructionConfirmMessage );

  // -------------------------------------------------------------------
  // searcher settings property sheet
  // Abbreviation = SPS
  REGISTER_ID( StringTable::Searcher::SettingsPropertySheet::Title );

  REGISTER_ID( StringTable::Searcher::SettingsPropertySheet::Settings );
  REGISTER_ID( StringTable::Searcher::SettingsPropertySheet::SettingsHomeLabel );
  REGISTER_ID( StringTable::Searcher::SettingsPropertySheet::SettingsHomeRefButton );
  REGISTER_ID( StringTable::Searcher::SettingsPropertySheet::SettingsAutoDisplayCheck );
  REGISTER_ID( StringTable::Searcher::SettingsPropertySheet::SettingsAsynchronousCheck );
  REGISTER_ID( StringTable::Searcher::SettingsPropertySheet::SettingsHomeRefFolderDialogDescription );

  // -------------------------------------------------------------------
  // searcher entry wait dialog
  REGISTER_ID( StringTable::Searcher::EntryWaitDialog::Title );
  REGISTER_ID( StringTable::Searcher::EntryWaitDialog::Text );



  // ---------------------------------------------------------------------
  // Message
  // ----------------
  // main
  REGISTER_ID( StringTable::Message::InternalError );
  REGISTER_ID( StringTable::Message::UnexpectedError );
  REGISTER_ID( StringTable::Message::UnexpectedErrorCaption );
  REGISTER_ID( StringTable::Message::CommandLineError );
  REGISTER_ID( StringTable::Message::CommandLineErrorCaption );

  // ----------------
  // convert
  REGISTER_ID( StringTable::Message::EntriedAudioFileNotFound );
  REGISTER_ID( StringTable::Message::AudioFileOpenError );
  REGISTER_ID( StringTable::Message::WavFileReadError );
  REGISTER_ID( StringTable::Message::WavFileWriteError );
  REGISTER_ID( StringTable::Message::OggFileOpenError );
  REGISTER_ID( StringTable::Message::OggFileReadError );
  REGISTER_ID( StringTable::Message::InvalidWavFileFormat );
  REGISTER_ID( StringTable::Message::UnsupportedWavFileFormat );
  REGISTER_ID( StringTable::Message::BadAllocation );
  REGISTER_ID( StringTable::Message::BmsFileAccessError );
  REGISTER_ID( StringTable::Message::OutputFileIsInputFilePath );
  REGISTER_ID( StringTable::Message::InvalidFormatAsBpmHeader );
  REGISTER_ID( StringTable::Message::InvalidFormatAsExtendedBpm );
  REGISTER_ID( StringTable::Message::InvalidFormatAsStopSequence );
  REGISTER_ID( StringTable::Message::InvalidFormatAsLNOBJHeader );
  REGISTER_ID( StringTable::Message::LNObjectInvalidEnclose );
  REGISTER_ID( StringTable::Message::LNObjectInvalidEncloseAdd );
  REGISTER_ID( StringTable::Message::LNObjectNotEnclosed );
  REGISTER_ID( StringTable::Message::NotEntriedWavWasUsed );
  REGISTER_ID( StringTable::Message::InvalidFormatAsBpmChange );
  REGISTER_ID( StringTable::Message::ExBpmChangeEntryNotExist );
  REGISTER_ID( StringTable::Message::StopSequenceEntryNotExist );
  REGISTER_ID( StringTable::Message::OutputFileAccessError );

  // ----------------
  // base
  REGISTER_ID( StringTable::Message::InvalidCharUsedAsWord );
  REGISTER_ID( StringTable::Message::InvalidWordValueUsed );
  REGISTER_ID( StringTable::Message::BufferOutOfRangeAccess );
  REGISTER_ID( StringTable::Message::RequiredResolutionIsTooBig );
  // parser
  REGISTER_ID( StringTable::Message::Parser::FileAccessError );
  REGISTER_ID( StringTable::Message::Parser::NoBeginningEndRandom );
  REGISTER_ID( StringTable::Message::Parser::HeaderCollision );
  REGISTER_ID( StringTable::Message::Parser::InvalidBpm );
  REGISTER_ID( StringTable::Message::Parser::InvalidLnobj );
  REGISTER_ID( StringTable::Message::Parser::ObjectCollision );
  REGISTER_ID( StringTable::Message::Parser::BarLengthChangeCollision );
  REGISTER_ID( StringTable::Message::Parser::InvalidChannelOrHeaderSyntax );
  REGISTER_ID( StringTable::Message::Parser::CorrespondingEndIfIsNoting );
  REGISTER_ID( StringTable::Message::Parser::InvalidRandomStatementValue );
  REGISTER_ID( StringTable::Message::Parser::CorrespondingIfIsNoting );
  REGISTER_ID( StringTable::Message::Parser::InvalidBarLengthChangeValue );
  REGISTER_ID( StringTable::Message::Parser::InvalidObjectExpression );
  REGISTER_ID( StringTable::Message::Parser::InvalidExtendedBPM );
  REGISTER_ID( StringTable::Message::Parser::InvalidStopSequence );
  REGISTER_ID( StringTable::Message::Parser::UndefinedObjectUsed );
  REGISTER_ID( StringTable::Message::Parser::DefinedObjectNotUsed );
  REGISTER_ID( StringTable::Message::Parser::ResolutionExceededLimit );

  // ----------------
  // core
  // converter
  REGISTER_ID( StringTable::Message::Converter::UsedMemoryExceededLimit );
  REGISTER_ID( StringTable::Message::Converter::UseDefaultBPM );
  REGISTER_ID( StringTable::Message::Converter::UseDefaultBPMNoSpecified );
  REGISTER_ID( StringTable::Message::Converter::TreatExtendedBPMAsNotSpecified );
  REGISTER_ID( StringTable::Message::Converter::TreatStopSequenceAsNotSpecified );
  REGISTER_ID( StringTable::Message::Converter::DecimalIsSpecifiedAsStopSequence );
  REGISTER_ID( StringTable::Message::Converter::TreatLNOBJAsNotSpecified );
  REGISTER_ID( StringTable::Message::Converter::TreatWavAsNotSpecified );
  REGISTER_ID( StringTable::Message::Converter::TreatEmptyFileAsSpecifiedAsWav );
  REGISTER_ID( StringTable::Message::Converter::TreatBPMAsNotChangedWithNumber );
  REGISTER_ID( StringTable::Message::Converter::TreatBPMAsNotChangedWithNumberObject );
  REGISTER_ID( StringTable::Message::Converter::TreatStopSequenceAsNothing );

  // use at script
  REGISTER_ID( StringTable::Message::Converter::Aborted );
  REGISTER_ID( StringTable::Message::Converter::RequestToAbortByUser );
  REGISTER_ID( StringTable::Message::Converter::ExceptionOccurredErrorExist );
  REGISTER_ID( StringTable::Message::Converter::ConvertStart );
  REGISTER_ID( StringTable::Message::Converter::InitializeStart );
  REGISTER_ID( StringTable::Message::Converter::InitializeEnd );
  REGISTER_ID( StringTable::Message::Converter::ParseStart );
  REGISTER_ID( StringTable::Message::Converter::ParseEnd );
  REGISTER_ID( StringTable::Message::Converter::MaxResolutionAndBarNumber );
  REGISTER_ID( StringTable::Message::Converter::ReadAudioFilesStart );
  REGISTER_ID( StringTable::Message::Converter::ReadAudioFilesEnd );
  REGISTER_ID( StringTable::Message::Converter::MixinWavesStart );
  REGISTER_ID( StringTable::Message::Converter::MixinWavesEnd );
  REGISTER_ID( StringTable::Message::Converter::AffectWaveStart );
  REGISTER_ID( StringTable::Message::Converter::AffectWaveEnd );
  REGISTER_ID( StringTable::Message::Converter::NormalizeKind );
  REGISTER_ID( StringTable::Message::Converter::NormalizeRatio );
  REGISTER_ID( StringTable::Message::Converter::OutputToFileStart );
  REGISTER_ID( StringTable::Message::Converter::OutputToFileEnd );
  REGISTER_ID( StringTable::Message::Converter::OutputFile );
  REGISTER_ID( StringTable::Message::Converter::OutputToFileNoOutputNotice );
  REGISTER_ID( StringTable::Message::Converter::OutputToFileNoOutputEnd );
  REGISTER_ID( StringTable::Message::Converter::ProcessingTime );
#undef REGISTER_ID

  // 略称登録
  auto alias = [&] ( std::vector<std::string> v, const std::string& new_name ) {
    auto v2 = v;
    v2.pop_back();
    this->NewSlot(
      [&] () { this->GetByStringFromRootTableJoined( v2 ); },
      [&] () { this->Native().PushString( new_name ); },
      [&] () { this->GetByStringFromRootTableJoined( v ); } );
  };
  alias( {Tag::StringTable},                                            Tag::StrT );
  alias( {Tag::StringTable, Tag::ParameterPropertySheet},               Tag::PPS );
  alias( {Tag::StringTable, Tag::Searcher, Tag::SettingsPropertySheet}, Tag::SPS );
}
