// string_table.h

#pragma once

#include "string_table_id.h"


namespace BMX2WAV::StringTable {
  class ID {
  public:
    ID( unsigned int number );
    std::string Get( void ) const;

    unsigned int number_;
  };

  void Initialize( const std::string& language );
  std::string Get( ID id );

  // ---------------------------------------------------------------------
  // main frame
  namespace Main {
    // ----------------
    // menu
    namespace Menu {
      static const ID File      = RSID_STR_MAIN_MENU_FILE;
      static const ID FileClose = RSID_STR_MAIN_MENU_FILE_CLOSE;

      static const ID View                 = RSID_STR_MAIN_MENU_VIEW;
      static const ID ViewAutoDisplayCells = RSID_STR_MAIN_MENU_VIEW_AUTO_DISPLAY_CELLS;
      static const ID ViewColumnGroup      = RSID_STR_MAIN_MENU_VIEW_COLUMN_GROUP;
      static const ID ViewDisplayCells     = RSID_STR_MAIN_MENU_VIEW_DISPLAY_CELLS;
      static const ID ViewReload           = RSID_STR_MAIN_MENU_VIEW_RELOAD;

      static const ID Edit                        = RSID_STR_MAIN_MENU_EDIT;
      static const ID EditEditCommonParameter     = RSID_STR_MAIN_MENU_EDIT_EDIT_COMMON_PARAMETER;
      static const ID EditAddEntry                = RSID_STR_MAIN_MENU_EDIT_ADD_ENTRY;
      static const ID EditDeleteEntry             = RSID_STR_MAIN_MENU_EDIT_DELETE_ENTRY;
      static const ID EditDeleteAllEntry          = RSID_STR_MAIN_MENU_EDIT_DELETE_ALL_ENTRY;
      static const ID EditShowPropertyOfEnty      = RSID_STR_MAIN_MENU_EDIT_SHOW_PROPERTY_OF_ENTRY;
      static const ID EditEditIndividualParameter = RSID_STR_MAIN_MENU_EDIT_EDIT_INDIVIDUAL_PARAMETER;

      static const ID Convert              = RSID_STR_MAIN_MENU_CONVERT;
      static const ID ConvertConvertOneBms = RSID_STR_MAIN_MENU_CONVERT_CONVERT_ONE_BMS;
      static const ID ConvertConvertAllBms = RSID_STR_MAIN_MENU_CONVERT_CONVERT_ALL_BMS;

      static const ID Tool                         = RSID_STR_MAIN_MENU_TOOL;
      static const ID ToolSettings                 = RSID_STR_MAIN_MENU_TOOL_SETTINGS;
      static const ID ToolExecuteSquirrelScript    = RSID_STR_MAIN_MENU_TOOL_EXECUTE_SQUIRREL_SCRIPT;
      static const ID ToolReloadSquirrelScript     = RSID_STR_MAIN_MENU_TOOL_RELOAD_SQUIRREL_SCRIPT;
      static const ID ToolShowSquirrelOutputDialog = RSID_STR_MAIN_MENU_TOOL_SHOW_SQUIRREL_OUTPUT_DIALOG;
      static const ID ToolConvertBmsonToBms        = RSID_STR_MAIN_MENU_TOOL_CONVERT_BMSON_TO_BMS;
      static const ID ToolStartSearcher            = RSID_STR_MAIN_MENU_TOOL_START_SEARCHER;

      static const ID Help                   = RSID_STR_MAIN_MENU_HELP;
      static const ID HelpVersionInformation = RSID_STR_MAIN_MENU_HELP_VERSION_INFORMATION;
    }

    // ----------------
    // tool bar
    namespace Toolbar {
      static const ID AutoDisplayCells         = RSID_STR_MAIN_TB_AUTO_DISPLAY_CELLS;
      static const ID SelectColumnLael         = RSID_STR_MAIN_TB_SELECT_COLUMN_LABEL;
      static const ID DisplayCells             = RSID_STR_MAIN_TB_DISPLAY_CELLS;
      static const ID Reload                   = RSID_STR_MAIN_TB_RELOAD;
      static const ID EditCommonParameter      = RSID_STR_MAIN_TB_EDIT_COMMON_PARAMETER;
      static const ID AddEntry                 = RSID_STR_MAIN_TB_ADD_ENTRY;
      static const ID DeleteEntry              = RSID_STR_MAIN_TB_DELETE_ENTRY;
      static const ID DeleteAllEntry           = RSID_STR_MAIN_TB_DELETE_ALL_ENTRY;
      static const ID ShowPropertyOfEntry      = RSID_STR_MAIN_TB_SHOW_PROPERTY_OF_ENTRY;
      static const ID EditIndividualParameter  = RSID_STR_MAIN_TB_EDIT_INDIVIDUAL_PARAMETER;
      static const ID ConvertOneBms            = RSID_STR_MAIN_TB_CONVERT_ONE_BMS;
      static const ID ConvertAllBms            = RSID_STR_MAIN_TB_CONVERT_ALL_BMS;
      static const ID ExecuteSquirrelScript    = RSID_STR_MAIN_TB_EXECUTE_SQUIRREL_SCRIPT;
      static const ID ReloadSquirrelScript     = RSID_STR_MAIN_TB_RELOAD_SQUIRREL_SCRIPT;
      static const ID ShowSquirrelOutputDialog = RSID_STR_MAIN_TB_SHOW_SQUIRREL_OUTPUT_DIALOG;
      static const ID StartSearcher            = RSID_STR_MAIN_TB_START_SEARCHER;
    }

    // ----------------
    // popup entry menu
    namespace PopupEntry {
      static const ID ShowPropertyOfEntry     = RSID_STR_MAIN_PO_ENTRY_SHOW_PROPERTY_OF_ENTRY;
      static const ID EditIndividualParameter = RSID_STR_MAIN_PO_ENTRY_EDIT_INDIVIDUAL_PARAMETER;
      static const ID ConvertOneBms           = RSID_STR_MAIN_PO_ENTRY_CONVERT_ONE_BMS;
      static const ID DeleteEntry             = RSID_STR_MAIN_PO_ENTRY_DELETE_ENTRY;
      static const ID ProcessorMenu           = RSID_STR_MAIN_PO_ENTRY_PROCESSOR_MENU;

      // Searcher 兼用
      static const ID ProcessorMenuPlayPreview    = RSID_STR_MAIN_PO_ENTRY_PROCESSOR_MENU_PLAY_PREVIEW;
      static const ID ProcessorMenuOpen           = RSID_STR_MAIN_PO_ENTRY_PROCESSOR_MENU_OPEN;
      static const ID ProcessorMenuOpenByNotepad  = RSID_STR_MAIN_PO_ENTRY_PROCESSOR_MENU_OPEN_BY_NOTEPAD;
      static const ID ProcessorMenuFileOperations = RSID_STR_MAIN_PO_ENTRY_PROCESSOR_MENU_FILE_OPERATIONS;
    }

    // ----------------
    // column name (use at script)
    namespace ColumnName {
      static const ID ObjectCount             = RSID_STR_MAIN_COLUMN_NAME_OBJECT_COUNT;
      static const ID ObjectOf1PCount         = RSID_STR_MAIN_COLUMN_NAME_OBJECT_OF_1P_COUNT;
      static const ID ObjectOf2PCount         = RSID_STR_MAIN_COLUMN_NAME_OBJECT_OF_2P_COUNT;
      static const ID MostSeriousError        = RSID_STR_MAIN_COLUMN_NAME_MOST_SERIOUS_ERROR;
      static const ID RandomStatement         = RSID_STR_MAIN_COLUMN_NAME_RANDOM_STATEMENT;
      static const ID RandomStatementExist    = RSID_STR_MAIN_COLUMN_NAME_RANDOM_STATEMENT_EXIST;
      static const ID RandomStatementNotExist = RSID_STR_MAIN_COLUMN_NAME_RANDOM_STATEMENT_NOT_EXIST;
      static const ID FullPath                = RSID_STR_MAIN_COLUMN_NAME_FULL_PATH;
      static const ID Basename                = RSID_STR_MAIN_COLUMN_NAME_BASENAME;
      static const ID ParentDirectory         = RSID_STR_MAIN_COLUMN_NAME_PARENT_DIRECTORY;
      static const ID ParentsParentDirectory  = RSID_STR_MAIN_COLUMN_NAME_PARENTS_PARENT_DIRECTORY;
      static const ID PlayingTime             = RSID_STR_MAIN_COLUMN_NAME_PLAYING_TIME;

      static const ID Parameter               = RSID_STR_MAIN_COLUMN_NAME_PARAMETER;
      static const ID ParameterIndividual     = RSID_STR_MAIN_COLUMN_NAME_PARAMETER_INDIVIDUAL;
      static const ID ParameterCommon         = RSID_STR_MAIN_COLUMN_NAME_PARAMETER_COMMON;
    }

    // column group name (use at script)
    // Searcher 兼用
    namespace ColumnGroupName {
      static const ID Simple    = RSID_STR_MAIN_COLUMN_GROUP_NAME_SIMPLE;
      static const ID Details   = RSID_STR_MAIN_COLUMN_GROUP_NAME_DETAILS;
      static const ID Various   = RSID_STR_MAIN_COLUMN_GROUP_NAME_VARIOUS;
      static const ID ForSearch = RSID_STR_MAIN_COLUMN_GROUP_NAME_FOR_SEARCH;
    }

    // ----------------
    // main frame
    static const ID MBScriptErrorCaption                        = RSID_STR_MAIN_MB_SCRIPT_ERROR_CAP;
    static const ID MBScriptErrorMessage                        = RSID_STR_MAIN_MB_SCRIPT_ERROR_MSG;
    static const ID SquirrelStandardOutputDialogTitle           = RSID_STR_MAIN_SQUIRREL_STANDARD_OUTPUT_DIALOG_TITLE;
    static const ID DefaultSortMethodName                       = RSID_STR_MAIN_DEFAULT_SORT_METHOD_NAME;
    static const ID StatusBarSortMethodTitle                    = RSID_STR_MAIN_STATUS_BAR_SORT_METHOD_TITLE;
    static const ID DefaultFirstColumnName                      = RSID_STR_MAIN_DEFAULT_FIRST_COLUMN_NAME;
    static const ID StatusBarSortMethodDescending               = RSID_STR_MAIN_STATUS_BAR_SORT_METHOD_DESCENDING;
    static const ID AddEntryFileDialogBmsBmeFile                = RSID_STR_MAIN_ADD_ENTRY_FILE_DIALOG_BMS_BME_FILE;
    static const ID AddEntryFileDialogAllFile                   = RSID_STR_MAIN_ADD_ENTRY_FILE_DIALOG_ALL_FILE;
    static const ID MBDeleteAllEntryCautionCaption              = RSID_STR_MAIN_MB_DELETE_ALL_ENTRY_CAUTION_CAP;
    static const ID MBDeleteAllEntryCautionMessage              = RSID_STR_MAIN_MB_DELETE_ALL_ENTRY_CAUTION_MSG;
    static const ID MBEditIndividualParameterCopyCautionCaption = RSID_STR_MAIN_MB_EDIT_INDIVIDUAL_PARAMETER_COPY_CAUTION_CAP;
    static const ID MBEditIndividualParameterCopyCautionMessage = RSID_STR_MAIN_MB_EDIT_INDIVIDUAL_PARAMETER_COPY_CAUTION_MSG;
    static const ID MBStartSearcherErrorCaption                 = RSID_STR_MAIN_MB_START_SEARCHER_ERROR_CAP;
    static const ID MBStartSearcherErrorMessage                 = RSID_STR_MAIN_MB_START_SEARCHER_ERROR_MSG;
    static const ID EntryProcessorMenuEmpty                     = RSID_STR_MAIN_ENTRY_PROCESSOR_MENU_EMPTY;
    static const ID ExecuteScriptFileDialogNutFile              = RSID_STR_MAIN_EXECUTE_SCRIPT_FILE_DIALOG_NUT_FILE;
    static const ID ExecuteScriptFileDialogAllFile              = RSID_STR_MAIN_EXECUTE_SCRIPT_FILE_DIALOG_ALL_FILE;
    static const ID ConvertBmsonFileDialogBmsonFile             = RSID_STR_MAIN_CONVERT_BMSON_FILE_DIALOG_BMSON_FILE;
    static const ID ConvertBmsonFileDialogAllFile               = RSID_STR_MAIN_CONVERT_BMSON_FILE_DIALOG_ALL_FILE;
    static const ID ConvertBmsonConfirmCaption                  = RSID_STR_MAIN_CONVERT_BMSON_CONFIRM_CAP;
    static const ID ConvertBmsonConfirmMessage                  = RSID_STR_MAIN_CONVERT_BMSON_CONFIRM_MSG;
    static const ID ConvertBmsonCompleteCaption                 = RSID_STR_MAIN_CONVERT_BMSON_COMPLETE_CAP;
    static const ID ConvertBmsonCompleteMessage                 = RSID_STR_MAIN_CONVERT_BMSON_COMPLETE_MSG;
    static const ID ConvertBmsonErrorCaption                    = RSID_STR_MAIN_CONVERT_BMSON_ERROR_CAP;
    static const ID ConvertBmsonErrorMessage                    = RSID_STR_MAIN_CONVERT_BMSON_ERROR_MSG;
  }

  // ---------------------------------------------------------------------
  // settings property sheet
  namespace SettingsPropertySheet {
    static const ID Title = RSID_STR_SPS_TITLE;
    
    static const ID Language              = RSID_STR_SPS_LANGUAGE;
    static const ID LanguageLanguageGroup = RSID_STR_SPS_LANGUAGE_LANGUAGE_GROUP;
    static const ID LanguageNoticeLabel   = RSID_STR_SPS_LANGUAGE_NOTICE_LABEL;
    static const ID LanguageSelectButton  = RSID_STR_SPS_LANGUAGE_SELECT_BUTTON;
    static const ID LanguageAuto          = RSID_STR_SPS_LANGUAGE_AUTO;
  }
  namespace SPS = SettingsPropertySheet;

  // ---------------------------------------------------------------------
  // parameter property sheet
  // Abbreviation = PPS
  namespace ParameterPropertySheet {
    static const ID Title = RSID_STR_PPS_TITLE;

    static const ID General                  = RSID_STR_PPS_GENERAL;
    static const ID GeneralLoadSaveGroup     = RSID_STR_PPS_GENERAL_LOAD_SAVE_GROUP;
    static const ID GeneralLoadButton        = RSID_STR_PPS_GENERAL_LOAD_BUTTON;
    static const ID GeneralSaveButton        = RSID_STR_PPS_GENERAL_SAVE_BUTTON;
    static const ID GeneralNameLabel         = RSID_STR_PPS_GENERAL_NAME_LABEL;
    static const ID GeneralUseScriptCheck    = RSID_STR_PPS_GENERAL_USE_SCRIPT_CHECK;
    static const ID GeneralScriptLabel       = RSID_STR_PPS_GENERAL_SCRIPT_LABEL;
    static const ID GeneralScriptRefButton   = RSID_STR_PPS_GENERAL_SCRIPT_REF_BUTTON;
    static const ID GeneralOutputLogCheck    = RSID_STR_PPS_GENERAL_OUTPUT_LOG_CHECK;
    static const ID GeneralLogLabel          = RSID_STR_PPS_GENERAL_LOG_LABEL;
    static const ID GeneralLogRefButton      = RSID_STR_PPS_GENERAL_LOG_REF_BUTTON;
    static const ID GeneralFileDialogAllFile = RSID_STR_PPS_GENERAL_FILE_DIALOG_ALL_FILE;

    static const ID Output                        = RSID_STR_PPS_OUTPUT;
    static const ID OutputOutputAsOggCheck        = RSID_STR_PPS_OUTPUT_OUTPUT_AS_OGG_CHECK;
    static const ID OutputOggBaseQualityLabel     = RSID_STR_PPS_OUTPUT_OGG_BASE_QUALITY_LABEL;
    static const ID OutputNeverOverwriteCheck     = RSID_STR_PPS_OUTPUT_NEVER_OVERWRITE_CHECK;
    static const ID OutputRemoveCharCheck         = RSID_STR_PPS_OUTPUT_REMOVE_CHAR_CHECK;
    static const ID OutputOutputFileTemplateLabel = RSID_STR_PPS_OUTPUT_OUTPUT_FILE_TEMPLATE_LABEL;
    static const ID OutputOutputFileHelpButton    = RSID_STR_PPS_OUTPUT_OUTPUT_FILE_HELP_BUTTON;
    static const ID OutputOutputFileExampleBms    = RSID_STR_PPS_OUTPUT_OUTPUT_FILE_EXAMPLE_BMS;
    static const ID OutputOutputFileExampleResult = RSID_STR_PPS_OUTPUT_OUTPUT_FILE_EXAMPLE_RESULT;
    static const ID OutputMBOggBaseQualityCaption = RSID_STR_PPS_OUTPUT_MB_OGG_BASE_QUALITY_CAP;
    static const ID OutputMBOggBaseQualityMessage = RSID_STR_PPS_OUTPUT_MB_OGG_BASE_QUALITY_MSG;

    static const ID Parser                           = RSID_STR_PPS_PARSER;
    static const ID ParserReadAsUTF8Check            = RSID_STR_PPS_PARSER_READ_AS_UTF8_CHECK;
    static const ID ParserIgnoreBGAChannelCheck      = RSID_STR_PPS_PARSER_IGNORE_BGA_CHANNEL_CHECK;
    static const ID ParserNotNestingIfStatementCheck = RSID_STR_PPS_PARSER_NOT_NESTING_IF_STATEMENT_CHECK;

    static const ID Mixin                                = RSID_STR_PPS_MIXIN;
    static const ID MixinCancelAtResoundingCheck         = RSID_STR_PPS_MIXIN_CANCEL_AT_RESOUNDING_CHECK;
    static const ID MixinStartLabel                      = RSID_STR_PPS_MIXIN_START_LABEL;
    static const ID MixinEndLabel                        = RSID_STR_PPS_MIXIN_END_LABEL;
    static const ID MixinConvertStartEndBarNumberLabel   = RSID_STR_PPS_MIXIN_CONVERT_START_END_BAR_NUMBER_LABEL;
    static const ID MixinDoTrimingCheck                  = RSID_STR_PPS_MIXIN_DO_TRIMING_CHECK;
    static const ID MixinRemoveFrontSilenceCheck         = RSID_STR_PPS_MIXIN_REMOVE_FRONT_SILENCE_CHECK;
    static const ID MixinInsertFrontSilenceCheck         = RSID_STR_PPS_MIXIN_INSERT_FRONT_SILENCE_CHECK;
    static const ID MixinInsertFrontSilenceSecondLabel   = RSID_STR_PPS_MIXIN_INSERT_FRONT_SILENCE_SECOND_LABEL;
    static const ID MixinMBKillActiveCaption             = RSID_STR_PPS_MIXIN_MB_KILL_ACTIVE_CAP;
    static const ID MixinMBKillActiveMessageStart        = RSID_STR_PPS_MIXIN_MB_KILL_ACTIVE_MSG_START;
    static const ID MixinMBKillActiveMessageEnd          = RSID_STR_PPS_MIXIN_MB_KILL_ACTIVE_MSG_END;
    static const ID MixinMBKillActiveMessageOrder        = RSID_STR_PPS_MIXIN_MB_KILL_ACTIVE_MSG_ORDER;
    static const ID MixinMBKillActiveMessageTrimStart    = RSID_STR_PPS_MIXIN_MB_KILL_ACTIVE_MSG_TRIM_START;
    static const ID MixinMBKillActiveMessageTrimEnd      = RSID_STR_PPS_MIXIN_MB_KILL_ACTIVE_MSG_TRIM_END;
    static const ID MixinMBKillActiveMessageTrimOrder    = RSID_STR_PPS_MIXIN_MB_KILL_ACTIVE_MSG_TRIM_ORDER;
    static const ID MixinMBKillActiveMessageFrontSilence = RSID_STR_PPS_MIXIN_MB_KILL_ACTIVE_MSG_FRONT_SILENCE;

    static const ID Audio                           = RSID_STR_PPS_AUDIO;
    static const ID AudioNormalizeKindNoneRadio     = RSID_STR_PPS_AUDIO_NORMALIZE_KIND_NONE_RADIO;
    static const ID AudioNormalizeKindPeakRadio     = RSID_STR_PPS_AUDIO_NORMALIZE_KIND_PEAK_RADIO;
    static const ID AudioNormalizeKindAverageRadio  = RSID_STR_PPS_AUDIO_NORMALIZE_KIND_AVERAGE_RADIO;
    static const ID AudioNormalizeKindOverRadio     = RSID_STR_PPS_AUDIO_NORMALIZE_KIND_OVER_RADIO;
    static const ID AudioNormalizeKindGroup         = RSID_STR_PPS_AUDIO_NORMALIZE_KIND_GROUP;
    static const ID AudioNormalizeOverPPMLabel      = RSID_STR_PPS_AUDIO_NORMALIZE_OVER_PPM_LABEL;
    static const ID AudioVolumeLabel                = RSID_STR_PPS_AUDIO_VOLUME_LABEL;
    static const ID AudioMBKillActiveCaption        = RSID_STR_PPS_AUDIO_MB_KILL_ACTIVE_CAP;
    static const ID AudioMBKillActiveMessageOverPPM = RSID_STR_PPS_AUDIO_MB_KILL_ACTIVE_MSG_OVER_PPM;
    static const ID AudioMBKillActiveMessageVolume  = RSID_STR_PPS_AUDIO_MB_KILL_ACTIVE_MSG_VOLUME;

    static const ID AfterProcess                         = RSID_STR_PPS_AFTER_PROCESS;
    static const ID AfterProcessExecuteAfterProcessCheck = RSID_STR_PPS_AFTER_PROCESS_EXECUTE_AFTER_PROCESS_CHECK;
    static const ID AfterProcessExecuteFileLabel         = RSID_STR_PPS_AFTER_PROCESS_EXECUTE_FILE_LABEL;
    static const ID AfterProcessExecuteFileRefButton     = RSID_STR_PPS_AFTER_PROCESS_EXECUTE_FILE_REF_BUTTON;
    static const ID AfterProcessExecuteArgumentsLabel    = RSID_STR_PPS_AFTER_PROCESS_EXECUTE_ARGUMENTS_LABEL;
    static const ID AfterProcessExecuteArgumentsHelp     = RSID_STR_PPS_AFTER_PROCESS_EXECUTE_ARGUMENTS_HELP;
    static const ID AfterProcessStartOnBackgroundCheck   = RSID_STR_PPS_AFTER_PROCESS_START_ON_BACKGROUND_CHECK;
    static const ID AfterProcessWaitForProcessExitCheck  = RSID_STR_PPS_AFTER_PROCESS_WAIT_FOR_PROCESS_EXIT_CHECK;
    static const ID AfterProcessDeleteOutputFileCheck    = RSID_STR_PPS_AFTER_PROCESS_DELETE_OUTPUT_FILE_CHECK;
    static const ID AfterProcessFileDialogExeFile        = RSID_STR_PPS_AFTER_PROCESS_FILE_DIALOG_EXE_FILE;
    static const ID AfterProcessFileDialogAllFile        = RSID_STR_PPS_AFTER_PROCESS_FILE_DIALOG_ALL_FILE;
  }
  namespace PPS = ParameterPropertySheet;

  // ---------------------------------------------------------------------
  // convert dialog
  namespace Convert {
    static const ID Title                 = RSID_STR_CONVERT_TITLE;

    static const ID AbortButton           = RSID_STR_CONVERT_ABORT_BUTTON;
    static const ID InitializeTitleLabel  = RSID_STR_CONVERT_INITIALIZE_TITLE_LABEL;
    static const ID InitializeReportLabel = RSID_STR_CONVERT_INITIALIZE_REPORT_LABEL;
    static const ID ParseTitleLabel       = RSID_STR_CONVERT_PARSE_TITLE_LABEL;
    static const ID ParseReportLabel      = RSID_STR_CONVERT_PARSE_REPORT_LABEL;
    static const ID WavTitleLabel         = RSID_STR_CONVERT_WAV_TITLE_LABEL;
    static const ID WavReportLabel        = RSID_STR_CONVERT_WAV_REPORT_LABEL;
    static const ID MixinTitleLabel       = RSID_STR_CONVERT_MIXIN_TITLE_LABEL;
    static const ID MixinReportLabel      = RSID_STR_CONVERT_MIXIN_REPORT_LABEL;
    static const ID AfterTitleLabel       = RSID_STR_CONVERT_AFTER_TITLE_LABEL;
    static const ID AfterReportLabel      = RSID_STR_CONVERT_AFTER_REPORT_LABEL;
    static const ID PlayWavButton         = RSID_STR_CONVERT_PLAY_WAV_BUTTON;
    static const ID StopWavButton         = RSID_STR_CONVERT_STOP_WAV_BUTTON;
    static const ID SaveAsWavButton       = RSID_STR_CONVERT_SAVE_AS_WAV_BUTTON;
    static const ID SaveAsOggButton       = RSID_STR_CONVERT_SAVE_AS_OGG_BUTTON;
    static const ID OpenWavButton         = RSID_STR_CONVERT_OPEN_WAV_BUTTON;
    static const ID ShowOutputBoxButton   = RSID_STR_CONVERT_SHOW_OUTPUT_BOX_BUTTON ;

    static const ID FileDialogWavFile   = RSID_STR_CONVERT_FILE_DIALOG_WAV_FILE;
    static const ID FileDialogOggFile   = RSID_STR_CONVERT_FILE_DIALOG_OGG_FILE;
    static const ID FileDialogAllFile   = RSID_STR_CONVERT_FILE_DIALOG_ALL_FILE;
    static const ID MBOpenWavCaption    = RSID_STR_CONVERT_MB_OPEN_WAV_CAP;
    static const ID MBOpenWavMessage    = RSID_STR_CONVERT_MB_OPEN_WAV_MSG;
    static const ID OutputDialogTitle   = RSID_STR_CONVERT_OUTPUT_DIALOG_TITLE;
    static const ID OpenLogErrorMessage = RSID_STR_CONVERT_OPEN_LOG_ERROR_MSG;
    static const ID OutputToLogError    = RSID_STR_CONVERT_OUTPUT_TO_LOG_ERROR;

    static const ID MBErrorAbortCaption       = RSID_STR_CONVERT_MB_ERROR_ABORT_CAP;
    static const ID MBErrorAbortMessage       = RSID_STR_CONVERT_MB_ERROR_ABORT_MSG;
    static const ID MBErrorMemoryCaption      = RSID_STR_CONVERT_MB_ERROR_MEMORY_CAP;
    static const ID MBErrorMemoryMessage      = RSID_STR_CONVERT_MB_ERROR_MEMORY_MSG;
    static const ID MBErrorUnexpectedCaption  = RSID_STR_CONVERT_MB_ERROR_UNEXPECTED_CAP;
    static const ID MBErrorUnexpectedMessage  = RSID_STR_CONVERT_MB_ERROR_UNEXPECTED_MSG;
    static const ID MBErrorScriptCaption      = RSID_STR_CONVERT_MB_ERROR_SCRIPT_CAP;
    static const ID MBErrorScriptMessage      = RSID_STR_CONVERT_MB_ERROR_SCRIPT_MSG;
    static const ID MBInfoRandomDialogCaption = RSID_STR_CONVERT_MB_INFO_RANDOM_DIALOG_CAP;
    static const ID MBInfoRandomDialogMessage = RSID_STR_CONVERT_MB_INFO_RANDOM_DIALOG_MSG;

    static const ID ReportLabelCompleted                   = RSID_STR_CONVERT_REPORT_LABEL_COMPLETED;
    static const ID ReportLabelErrorExist                  = RSID_STR_CONVERT_REPORT_LABEL_ERROR_EXIST;
    static const ID ReportLabelAbort                       = RSID_STR_CONVERT_REPORT_LABEL_ABORT;
    static const ID ReportResultAbort                      = RSID_STR_CONVERT_REPORT_RESULT_ABORT;
    static const ID ReportLabelInitializeRunning           = RSID_STR_CONVERT_REPORT_LABEL_INITIALIZE_RUNNING;
    static const ID ReportResultInitializeRunning          = RSID_STR_CONVERT_REPORT_RESULT_INITIALIZE_RUNNING;
    static const ID ReportLabelParseRunning                = RSID_STR_CONVERT_REPORT_LABEL_PARSE_RUNNING;
    static const ID ReportResultParseRunning               = RSID_STR_CONVERT_REPORT_RESULT_PARSE_RUNNING;
    static const ID ReportLabelReadAudioFilesRunning       = RSID_STR_CONVERT_REPORT_LABEL_READ_AUDIO_FILES_RUNNING;
    static const ID ReportResultReadAudioFilesRunning      = RSID_STR_CONVERT_REPORT_RESULT_READ_AUDIO_FILES_RUNNING;
    static const ID ReportLabelOneAudioFileReadRunning     = RSID_STR_CONVERT_REPORT_LABEL_ONE_AUDIO_FILE_READ_RUNNING;
    static const ID ReportLabelMixinRunning                = RSID_STR_CONVERT_REPORT_LABEL_MIXIN_RUNNING;
    static const ID ReportResultMixinRunning               = RSID_STR_CONVERT_REPORT_RESULT_MIXIN_RUNNING;
    static const ID ReportLabelAffectWaveRunning           = RSID_STR_CONVERT_REPORT_LABEL_AFFECT_WAVE_RUNNING;
    static const ID ReportResultAffectWaveRunning          = RSID_STR_CONVERT_REPORT_RESULT_AFFECT_WAVE_RUNNING;
    static const ID ReportResultOutputToFileRunning        = RSID_STR_CONVERT_REPORT_RESULT_OUTPUT_TO_FILE_RUNNING;
    static const ID ReportLabelAfterOutputToFileNoOutput   = RSID_STR_CONVERT_REPORT_LABEL_AFTER_OUTPUT_TO_FILE_NO_OUTPUT;
    static const ID ReportResultAfterOutputToFileCompleted = RSID_STR_CONVERT_REPORT_RESULT_AFTER_OUTPUT_TO_FILE_COMPLETED;
  }

  // ---------------------------------------------------------------------
  // multiple convert dialog
  namespace Multiple {
    static const ID Title                        = RSID_STR_MULTIPLE_TITLE;
    static const ID MenuShowLog                  = RSID_STR_MULTIPLE_MENU_SHOW_LOG;
    static const ID AbortButton                  = RSID_STR_MULTIPLE_ABORT_BUTTON;
    static const ID ColumnBms                    = RSID_STR_MULTIPLE_COLUMN_BMS;
    static const ID ColumnResult                 = RSID_STR_MULTIPLE_COLUMN_RESULT;
    static const ID ColumnMostSeriousError       = RSID_STR_MULTIPLE_COLUMN_MOST_SERIOUS_ERROR;
    static const ID ColumnDefaultTextUnconverted = RSID_STR_MULTIPLE_COLUMN_DEFAULT_TEXT_UNCONVERTED;
    static const ID LogDioalogTitle              = RSID_STR_MULTIPLE_LOG_DIALOG_TITLE;
    static const ID StatusTextConverting         = RSID_STR_MULTIPLE_STATUS_TEXT_CONVERTING;
    static const ID StatusTextAborted            = RSID_STR_MULTIPLE_STATUS_TEXT_ABORTED;
    static const ID StatusTextCompleted          = RSID_STR_MULTIPLE_STATUS_TEXT_COMPLETED;
  }

  // ---------------------------------------------------------------------
  // entry dialog
  namespace Entry {
    static const ID Title                         = RSID_STR_EMTRY_TITLE;
    static const ID UsingParameterTitleLabel      = RSID_STR_EMTRY_USING_PARAMETER_TITLE_LABEL;
    static const ID UseCommonParameterButton      = RSID_STR_EMTRY_USE_COMMON_PARAMETER_BUTTON;
    static const ID UseIndividualParameterButton  = RSID_STR_EMTRY_USE_INDIVIDUAL_PARAMETER_BUTTON;
    static const ID ConvertButton                 = RSID_STR_EMTRY_CONVERT_BUTTON;
    static const ID ColumnName                    = RSID_STR_EMTRY_COLUMN_NAME;
    static const ID ColumnValue                   = RSID_STR_EMTRY_COLUMN_VALUE;
    static const ID UsingParameterLabelCommon     = RSID_STR_EMTRY_USING_PARAMETER_LABEL_COMMON;
    static const ID UsingParameterLabelIndividual = RSID_STR_EMTRY_USING_PARAMETER_LABEL_INDIVIDUAL;
    static const ID MBUseCommonCaption            = RSID_STR_EMTRY_MB_USE_COMMON_CAP;
    static const ID MBUseCommonMessage            = RSID_STR_EMTRY_MB_USE_COMMON_MSG;
    static const ID MBUseIndividualCaption        = RSID_STR_EMTRY_MB_USE_INDIVIDUAL_CAP;
    static const ID MBUseIndividualMessage        = RSID_STR_EMTRY_MB_USE_INDIVIDUAL_MSG;
  }

  // ---------------------------------------------------------------------
  // random statement dialog
  namespace Random {
    static const ID Title       = RSID_STR_RANDOM_TITLE;
    static const ID CheckButton = RSID_STR_RANDOM_CHECK_BUTTON;
    static const ID OKButton    = RSID_STR_RANDOM_OK_BUTTON;
  }

  // ---------------------------------------------------------------------
  // template path translate help dialog
  namespace TemplatePath {
    static const ID Title             = RSID_STR_TEMPLATE_PATH_TITLE;
    static const ID MenuCopy          = RSID_STR_TEMPLATE_PATH_MENU_COPY;
    static const ID ColumnName        = RSID_STR_TEMPLATE_PATH_COLUMN_NAME;
    static const ID ColumnDescription = RSID_STR_TEMPLATE_PATH_COLUMN_DESCRIPTION;
    static const ID Function          = RSID_STR_TEMPLATE_PATH_FUNCTION;

    static const ID TextOmitted             = RSID_STR_TEMPLATE_PATH_TEXT_OMITTED;
    static const ID TextInputBmsPath        = RSID_STR_TEMPLATE_PATH_TEXT_INPUT_BMS_PATH;
    static const ID TextAutoExtension       = RSID_STR_TEMPLATE_PATH_TEXT_AUTO_EXTENSION;
    static const ID TextBMX2WAVDirectory    = RSID_STR_TEMPLATE_PATH_TEXT_BMX2WAV_DIRECTORY;
    static const ID TextHeaderXXXXX         = RSID_STR_TEMPLATE_PATH_TEXT_HEADER_XXXXX;
    static const ID TextBaseName            = RSID_STR_TEMPLATE_PATH_TEXT_BASENAME;
    static const ID TextDirName             = RSID_STR_TEMPLATE_PATH_TEXT_DIRNAME;
    static const ID TextRemoveExtension     = RSID_STR_TEMPLATE_PATH_TEXT_REMOVE_EXTENSION;
    static const ID TextChangeAutoExtension = RSID_STR_TEMPLATE_PATH_TEXT_CHANGE_AUTO_EXTENSION;
  }


  // ---------------------------------------------------------------------
  // ===== searcher =====
  namespace Searcher {
    // -------------------------------------------------------------------
    // searcher main frame
    namespace Main {
      // ----------------
      // searcher main menu
      namespace Menu {
        static const ID File                     = RSID_STR_SRC_MAIN_FILE;
        static const ID FileClose                = RSID_STR_SRC_MAIN_FILE_CLOSE;
        static const ID FileExport               = RSID_STR_SRC_MAIN_FILE_EXPORT;
        static const ID FileExportCSVToFile      = RSID_STR_SRC_MAIN_FILE_EXPORT_CSV_TO_FILE;
        static const ID FileExportTSVToClipboard = RSID_STR_SRC_MAIN_FILE_EXPORT_TSV_TO_CLIPBOARD;
        static const ID Tool                     = RSID_STR_SRC_MAIN_TOOL;
        static const ID ToolSettings             = RSID_STR_SRC_MAIN_TOOL_SETTINGS;
      }

      // ----------------
      // searcher tree menu
      namespace PopupTree {
        static const ID OpenDirectory = RSID_STR_SRC_MAIN_PO_TREE_OPEN_DIRECTORY;
      }

      // ----------------
      // searcher entry menu
      namespace PopupEntry {
        static const ID HideEntry     = RSID_STR_SRC_MAIN_PO_ENTRY_HIDE_ENTRY;
        static const ID ProcessorMenu = RSID_STR_SRC_MAIN_PO_ENTRY_PROCESSOR_MENU;

        // BMX2WAV 本体兼用
        static const ID ProcessorMenuPlayPreview    = StringTable::Main::PopupEntry::ProcessorMenuPlayPreview;
        static const ID ProcessorMenuOpen           = StringTable::Main::PopupEntry::ProcessorMenuOpen;
        static const ID ProcessorMenuOpenByNotepad  = StringTable::Main::PopupEntry::ProcessorMenuOpenByNotepad;
        static const ID ProcessorMenuFileOperations = StringTable::Main::PopupEntry::ProcessorMenuFileOperations;
      }

      // ----------------
      // searcher tool bar
      namespace Toolbar {
        static const ID ReloadSquirrelScript     = RSID_STR_SRC_MAIN_TB_RELOAD_SQUIRREL_SCRIPT;
        static const ID ShowSquirrelOutputDialog = RSID_STR_SRC_MAIN_TB_SHOW_SQUIRREL_OUTPUT_DIALOG;
        static const ID Export                   = RSID_STR_SRC_MAIN_TB_EXPORT;
        static const ID HomeFolder               = RSID_STR_SRC_MAIN_TB_HOME_FOLDER;
        static const ID FilterLabel              = RSID_STR_SRC_MAIN_TB_FILTER_LABEL;
        static const ID AutoDisplayCells         = RSID_STR_SRC_MAIN_TB_AUTO_DISPLAY_CELLS;
        static const ID SelectColumnLabel        = RSID_STR_SRC_MAIN_TB_SELECT_COLUMN_LABEL;
        static const ID DisplayCells             = RSID_STR_SRC_MAIN_TB_DISPLAY_CELLS;
        static const ID ExecuteSearchLabel       = RSID_STR_SRC_MAIN_TB_EXECUTE_SEARCH_LABEL;
        static const ID ExecuteSearch            = RSID_STR_SRC_MAIN_TB_EXECUTE_SEARCH;

        // display filter (use at script)
        static const ID DisplayFilterAllPass    = RSID_STR_SRC_MAIN_TB_DISPLAY_FILTER_ALL_PASS;
        static const ID DisplayFilterBmsGeneral = RSID_STR_SRC_MAIN_TB_DISPLAY_FILTER_BMS_GENERAL;
        static const ID DisplayFilterOnlyBms    = RSID_STR_SRC_MAIN_TB_DISPLAY_FILTER_ONLY_BMS;
        static const ID DisplayFilterOnlyBme    = RSID_STR_SRC_MAIN_TB_DISPLAY_FILTER_ONLY_BME;

        // search method (use at script)
        static const ID SearchMethodBmsAll           = RSID_STR_SRC_MAIN_TB_SEARCH_METHOD_BMS_ALL;
        static const ID SearchMethodFilterBmsGeneral = RSID_STR_SRC_MAIN_TB_SEARCH_METHOD_FILTER_BMS_GENERAL;
        static const ID SearchMethodMostPlaylevel    = RSID_STR_SRC_MAIN_TB_SEARCH_METHOD_MOST_PLAYLEVEL;
      }

      // ----------------
      // column group name (use at script)
      // BMX2WAV 本体兼用
      namespace ColumnGroupName {
        static const ID Simple    = StringTable::Main::ColumnGroupName::Simple;
        static const ID Details   = StringTable::Main::ColumnGroupName::Details;
        static const ID Various   = StringTable::Main::ColumnGroupName::Various;
        static const ID ForSearch = StringTable::Main::ColumnGroupName::ForSearch;
      }

      // ----------------
      // searcher main frame
      static const ID MBScriptErrorCaption                      = RSID_STR_SRC_MAIN_MB_SCRIPT_ERROR_CAP;
      static const ID MBScriptErrorMessage                      = RSID_STR_SRC_MAIN_MB_SCRIPT_ERROR_MSG;
      static const ID SquirrelStandardOutputDialogTitle         = RSID_STR_SRC_MAIN_SQUIRREL_STANDARD_OUTPUT_DIALOG_TITLE;
      static const ID DefaultSortMethodName                     = RSID_STR_SRC_MAIN_DEFAULT_SORT_METHOD_NAME;
      static const ID StatusBarSortMethodTitle                  = RSID_STR_SRC_MAIN_STATUS_BAR_SORT_METHOD_TITLE;
      static const ID EntryProcessorMenuEmpty                   = RSID_STR_SRC_MAIN_ENTRY_PROCESSOR_MENU_EMPTY;
      static const ID StatusBarSortMethodDescending             = RSID_STR_SRC_MAIN_STATUS_BAR_SORT_METHOD_DESCENDING;
      static const ID DefaultFirstColumnName                    = RSID_STR_SRC_MAIN_DEFAULT_FIRST_COLUMN_NAME;
      static const ID MBExecuteSearchImpossibleDirectoryCaption = RSID_STR_SRC_MAIN_MB_EXECUTE_SEARCH_IMPOSSIBLE_DIRECTORY_CAP;
      static const ID MBExecuteSearchImpossibleDirectoryMessage = RSID_STR_SRC_MAIN_MB_EXECUTE_SEARCH_IMPOSSIBLE_DIRECTORY_MSG;
      static const ID MBExecuteSearchExecuteConfirmCaption      = RSID_STR_SRC_MAIN_MB_EXECUTE_SEARCH_EXECUTE_CONFIRM_CAP;
      static const ID MBExecuteSearchExecuteConfirmMessage      = RSID_STR_SRC_MAIN_MB_EXECUTE_SEARCH_EXECUTE_CONFIRM_MSG;
      static const ID MBSearchResultDestructionConfirmCaption   = RSID_STR_SRC_MAIN_MB_SEARCH_RESULT_DESTRUCTION_CONFIRM_CAP;
      static const ID MBSearchResultDestructionConfirmMessage   = RSID_STR_SRC_MAIN_MB_SEARCH_RESULT_DESTRUCTION_CONFIRM_MSG;
    }

    // -------------------------------------------------------------------
    // searcher settings property sheet
    // Abbreviation = SPS
    namespace SettingsPropertySheet {
      static const ID Title = RSID_STR_SRC_SPS_TITLE;

      static const ID Settings                               = RSID_STR_SRC_SPS_SETTINGS;
      static const ID SettingsHomeLabel                      = RSID_STR_SRC_SPS_SETTINGS_HOME_LABEL;
      static const ID SettingsHomeRefButton                  = RSID_STR_SRC_SPS_SETTINGS_HOME_REF_BUTTON;
      static const ID SettingsAutoDisplayCheck               = RSID_STR_SRC_SPS_SETTINGS_AUTO_DISPLAY_CHECK;
      static const ID SettingsAsynchronousCheck              = RSID_STR_SRC_SPS_SETTINGS_ASYNCHRONOUS_CHECK;
      static const ID SettingsHomeRefFolderDialogDescription = RSID_STR_SRC_SPS_SETTINGS_HOME_REF_FOLDER_DIALOG_DESCRIPTION;

      static const ID Language              = RSID_STR_SRC_SPS_LANGUAGE;
      static const ID LanguageLanguageGroup = RSID_STR_SRC_SPS_LANGUAGE_LANGUAGE_GROUP;
      static const ID LanguageNoticeLabel   = RSID_STR_SRC_SPS_LANGUAGE_NOTICE_LABEL;
      static const ID LanguageSelectButton  = RSID_STR_SRC_SPS_LANGUAGE_SELECT_BUTTON;
      static const ID LanguageAuto          = RSID_STR_SRC_SPS_LANGUAGE_AUTO;
    }
    namespace SPS = SettingsPropertySheet;

    // -------------------------------------------------------------------
    // searcher entry wait dialog
    namespace EntryWaitDialog {
      static const ID Title = RSID_STR_SRC_ENTRY_WAIT_DIALOG_TITLE;
      static const ID Text  = RSID_STR_SRC_ENTRY_WAIT_DIALOG_TEXT;
    }
  }



  // ---------------------------------------------------------------------
  namespace Message {
    // ----------------
    // main
    static const ID InternalError           = RSID_STR_MSG_INTERNAL_ERROR;
    static const ID UnexpectedError         = RSID_STR_MSG_UNEXPECTED_ERROR;
    static const ID UnexpectedErrorCaption  = RSID_STR_MSG_UNEXPECTED_ERROR_CAP;
    static const ID CommandLineError        = RSID_STR_MSG_COMMAND_LINE_ERROR;
    static const ID CommandLineErrorCaption = RSID_STR_MSG_COMMAND_LINE_ERROR_CAP;

    // ----------------
    // convert
    static const ID EntriedAudioFileNotFound    = RSID_STR_MSG_ENTRIED_AUDIO_FILE_NOT_FOUND;
    static const ID AudioFileOpenError          = RSID_STR_MSG_AUDIO_FILE_OPEN_ERROR;
    static const ID WavFileReadError            = RSID_STR_MSG_WAV_FILE_READ_ERROR;
    static const ID WavFileWriteError           = RSID_STR_MSG_WAV_FILE_WRITE_ERROR;
    static const ID OggFileOpenError            = RSID_STR_MSG_OGG_FILE_OPEN_ERROR;
    static const ID OggFileReadError            = RSID_STR_MSG_OGG_FILE_READ_ERROR;
    static const ID InvalidWavFileFormat        = RSID_STR_MSG_INVALID_WAV_FILE_FORMAT;
    static const ID UnsupportedWavFileFormat    = RSID_STR_MSG_UNSUPPORTED_WAV_FILE_FORMAT;
    static const ID BadAllocation               = RSID_STR_MSG_BAD_ALLOCATION;
    static const ID BmsFileAccessError          = RSID_STR_MSG_BMS_FILE_ACCESS_ERROR;
    static const ID OutputFileIsInputFilePath   = RSID_STR_MSG_OUTPUT_FILE_IS_INPUT_FILE_PATH;
    static const ID InvalidFormatAsBpmHeader    = RSID_STR_MSG_INVALID_FORMAT_AS_BPM_HEADER;
    static const ID InvalidFormatAsExtendedBpm  = RSID_STR_MSG_INVALID_FORMAT_AS_EXTENDED_BPM;
    static const ID InvalidFormatAsStopSequence = RSID_STR_MSG_INVALID_FORMAT_AS_STOP_SEQUENCE;
    static const ID InvalidFormatAsLNOBJHeader  = RSID_STR_MSG_INVALID_FORMAT_AS_LNOBJ_HEADER;
    static const ID LNObjectInvalidEnclose      = RSID_STR_MSG_LN_OBJECT_INVALID_ENCLOSE;
    static const ID LNObjectInvalidEncloseAdd   = RSID_STR_MSG_LN_OBJECT_INVALID_ENCLOSE_ADD;
    static const ID LNObjectNotEnclosed         = RSID_STR_MSG_LN_OBJECT_NOT_ENCLOSED;
    static const ID NotEntriedWavWasUsed        = RSID_STR_MSG_NOT_ENTRIED_WAV_WAS_USED;
    static const ID InvalidFormatAsBpmChange    = RSID_STR_MSG_INVALID_FORMAT_AS_BPM_CHANGE;
    static const ID ExBpmChangeEntryNotExist    = RSID_STR_MSG_EX_BPM_CHANGE_ENTRY_NOT_EXIST;
    static const ID StopSequenceEntryNotExist   = RSID_STR_MSG_STOP_SEQUENCE_ENTRY_NOT_EXIST;
    static const ID OutputFileAccessError       = RSID_STR_MSG_OUTPUT_FILE_ACCESS_ERROR;

    // ----------------
    // base
    static const ID InvalidCharUsedAsWord      = RSID_STR_MSG_INVALID_CHAR_USED_AS_WORD;
    static const ID InvalidWordValueUsed       = RSID_STR_MSG_INVALID_WORD_VALUE_USED;
    static const ID BufferOutOfRangeAccess     = RSID_STR_MSG_BUFFER_OUT_OF_RANGE_ACCESS;
    static const ID RequiredResolutionIsTooBig = RSID_STR_MSG_REQIORED_RESOLUTION_IS_TOO_BIG;
    // parser
    namespace Parser {
      static const ID FileAccessError              = RSID_STR_MSG_P_FILE_ACCESS_ERROR;
      static const ID NoBeginningEndRandom         = RSID_STR_MSG_P_NO_BEGINNING_END_RAMDOM;
      static const ID HeaderCollision              = RSID_STR_MSG_P_HEADER_COLLISION;
      static const ID InvalidBpm                   = RSID_STR_MSG_P_INVALID_BPM;
      static const ID InvalidLnobj                 = RSID_STR_MSG_P_INVALID_LNOBJ;
      static const ID ObjectCollision              = RSID_STR_MSG_P_OBJECT_COLLISION;
      static const ID BarLengthChangeCollision     = RSID_STR_MSG_P_BAR_LENGTH_CHANGE_COLLISION;
      static const ID InvalidChannelOrHeaderSyntax = RSID_STR_MSG_P_INVALID_CHANNEL_OR_HEADER_SYNTAX;
      static const ID CorrespondingEndIfIsNoting   = RSID_STR_MSG_P_CORRESPONDING_END_IF_IS_NOTING;
      static const ID InvalidRandomStatementValue  = RSID_STR_MSG_P_INVALID_RANDOM_STATEMENT_VALUE;
      static const ID CorrespondingIfIsNoting      = RSID_STR_MSG_P_CORRESPONDING_IF_IS_NOTIONG;
      static const ID InvalidBarLengthChangeValue  = RSID_STR_MSG_P_INVALID_BAR_LENGTH_CHANGE_VALUE;
      static const ID InvalidObjectExpression      = RSID_STR_MSG_P_INVALID_OBJECT_EXPRESSION;
      static const ID InvalidExtendedBPM           = RSID_STR_MSG_P_INVALID_EXTENDED_BPM;
      static const ID InvalidStopSequence          = RSID_STR_MSG_P_INVALID_STOP_SEQUENCE;
      static const ID UndefinedObjectUsed          = RSID_STR_MSG_P_UNDEFINED_OBJECT_USED;
      static const ID DefinedObjectNotUsed         = RSID_STR_MSG_P_DEFINED_OBJECT_NOT_USED;
      static const ID ResolutionExceededLimit      = RSID_STR_MSG_P_RESOLUTION_EXCEEDED_LIMIT;
    }
    // bmson parser
    namespace BmsonParser {
      static const ID UnexpectedToken                  = RSID_STR_MSG_BMSON_UNEXPECTED_TOKEN;
      static const ID NumberFormatError                = RSID_STR_MSG_BMSON_NUMBER_FORMAT_ERROR;
      static const ID UnicodeFormatError               = RSID_STR_MSG_BMSON_UNICODE_FORMAT_ERROR;
      static const ID DataTypeError                    = RSID_STR_MSG_BMSON_DATA_TYPE_ERROR;
      static const ID BmsonDataContradiction           = RSID_STR_MSG_BMSON_BMSON_DATA_CONTRADICTION;
      static const ID BarIsOutOfBmsRange               = RSID_STR_MSG_BMSON_BAR_IS_OUT_OF_BMS_RANGE;
      static const ID NumberOfObjectsIsOutOfBmsRange   = RSID_STR_MSG_BMSON_NUMBER_OF_OBJECTS_IS_OUT_OF_BMS_RANGE;
      static const ID RequiredKeyIsNothing             = RSID_STR_MSG_BMSON_REQUIRED_KEY_IS_NOTHING;
      static const ID BmsonObjectIsOutOfBmsonLineRange = RSID_STR_MSG_BMSON_BMSON_OBJECT_IS_OUT_OF_BMSON_LINE_RANGE;
    }

    // ----------------
    // core
    // converter
    namespace Converter {
      static const ID UsedMemoryExceededLimit              = RSID_STR_MSG_C_USED_MEMORY_EXCEEDED_LIMIT;
      static const ID UseDefaultBPM                        = RSID_STR_MSG_C_USE_DEFAULT_BPM;
      static const ID UseDefaultBPMNoSpecified             = RSID_STR_MSG_C_USE_DEFAULT_BPM_NO_SPECIFIED;
      static const ID TreatExtendedBPMAsNotSpecified       = RSID_STR_MSG_C_TREAT_EXTENDED_BPM_AS_NOT_SPECIFIED;
      static const ID TreatStopSequenceAsNotSpecified      = RSID_STR_MSG_C_TREAT_STOP_SEQUENCE_AS_NOT_SPECIFIED;
      static const ID DecimalIsSpecifiedAsStopSequence     = RSID_STR_MSG_C_DECIMAL_IS_SPECIFIED_AS_STOP_SEQUENCE;
      static const ID TreatLNOBJAsNotSpecified             = RSID_STR_MSG_C_TREAT_LNOBJ_AS_NOT_SPECIFIED;
      static const ID TreatWavAsNotSpecified               = RSID_STR_MSG_C_TREAT_WAV_AS_NOT_SPECIFIED;
      static const ID TreatEmptyFileAsSpecifiedAsWav       = RSID_STR_MSG_C_TREAT_EMPTY_FILE_AS_SPECIFIED_AS_WAV;
      static const ID TreatBPMAsNotChangedWithNumber       = RSID_STR_MSG_C_TREAT_BPM_AS_NOT_CHANGED_WITH_NUMBER;
      static const ID TreatBPMAsNotChangedWithNumberObject = RSID_STR_MSG_C_TREAT_BPM_AS_NOT_CHANGED_WITH_NUMBER_OBJECT;
      static const ID TreatStopSequenceAsNothing           = RSID_STR_MSG_C_TREAT_STOP_SEQUENCE_AS_NOTHING;

      // use at script
      static const ID Aborted                     = RSID_STR_MSG_C_ABORTED;
      static const ID RequestToAbortByUser        = RSID_STR_MSG_C_REQUEST_TO_ABORT_BY_USER;
      static const ID ExceptionOccurredErrorExist = RSID_STR_MSG_C_EXCEPTION_OCCURRED_ERROR_EXIST;
      static const ID ConvertStart                = RSID_STR_MSG_C_CONVERT_START;
      static const ID InitializeStart             = RSID_STR_MSG_C_INITIALIZE_START;
      static const ID InitializeEnd               = RSID_STR_MSG_C_INITIALIZE_END;
      static const ID ParseStart                  = RSID_STR_MSG_C_PARSE_START;
      static const ID ParseEnd                    = RSID_STR_MSG_C_PARSE_END;
      static const ID MaxResolutionAndBarNumber   = RSID_STR_MSG_C_MAX_RESOLUTION_AND_BAR_NUMBER;
      static const ID ReadAudioFilesStart         = RSID_STR_MSG_C_READ_AUDIO_FILES_START;
      static const ID ReadAudioFilesEnd           = RSID_STR_MSG_C_READ_AUDIO_FILES_END;
      static const ID MixinWavesStart             = RSID_STR_MSG_C_MIXIN_WAVES_START;
      static const ID MixinWavesEnd               = RSID_STR_MSG_C_MIXIN_WAVES_END;
      static const ID AffectWaveStart             = RSID_STR_MSG_C_AFFECT_WAVE_START;
      static const ID AffectWaveEnd               = RSID_STR_MSG_C_AFFECT_WAVE_END;
      static const ID NormalizeKind               = RSID_STR_MSG_C_NORMALIZE_KIND;
      static const ID NormalizeRatio              = RSID_STR_MSG_C_NORMALIZE_RATIO;
      static const ID OutputToFileStart           = RSID_STR_MSG_C_OUTPUT_TO_FILE_START;
      static const ID OutputToFileEnd             = RSID_STR_MSG_C_OUTPUT_TO_FILE_END;
      static const ID OutputFile                  = RSID_STR_MSG_C_OUTPUT_FILE;
      static const ID OutputToFileNoOutputNotice  = RSID_STR_MSG_C_OUTPUT_TO_FILE_NO_OUTPUT_NOTICE;
      static const ID OutputToFileNoOutputEnd     = RSID_STR_MSG_C_OUTPUT_TO_FILE_NO_OUTPUT_END;
      static const ID ProcessingTime              = RSID_STR_MSG_C_PROCESSING_TIME;
    }
  }
}

// 略称
namespace BMX2WAV {
  namespace StrT = StringTable;
}
