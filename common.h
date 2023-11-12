// common.h

#pragma once

#include <optional>

#include "tt_icon.h"
#include "tt_image_list.h"
#include "tt_exception.h"


namespace BMX2WAV {
  const char* const VERSION = "2.2.1";
  const char* const APPLICATION_NAME = "BMX2WAV";

  namespace Const {
    const unsigned int DEFAULT_BAR_RESOLUTION_COUNT = 1;
    const unsigned int BAR_RESOLUTION_MAX = 1048576;
    const int HEX36_MIN_VALUE = 0;
    const int HEX36_MAX_VALUE = 35;
    const int HEX36_MAX_COUNT = 36;

    const int WORD_MIN_VALUE = 0;
    const int WORD_MAX_VALUE = 1295;
    const int WORD_MAX_COUNT = 1296;

    const unsigned int BAR_MAX_VALUE = 999;
    const unsigned int BAR_MAX_COUNT = 1000;
  }

  // -- ErrorLevel -----
  enum class ErrorLevel {
    Internal = 0,
    ImmediatelyAbort,
    Fatal,
    NeedFix,
    Tiny,
    Warning,
    None,
  };

  // -- CommandID --------------------------------------------------------
  namespace CommandID {
    enum ID : int {
      // �t�@�C��
      Close = 2000,

      // �\��
      AutoDisplayCells,
      SelectColumnGroup,
      DisplayCells,
      Reload,

      // �ҏW
      EditCommonParameter,

      AddEntry,

      DeleteEntry,
      DeleteAllEntry,

      ShowPropertyOfEntry,
      EditIndividualParameter,

      // �ϊ�
      ConvertOneBms,
      ConvertAllBms,

      // �c�[��
      Settings,
      ExecuteSquirrelScript,
      ReloadSquirrelScript,
      ShowSquirrelOutputDialog,
      ConvertBmsonToBms,
      StartSearcher,

      // �w���v
      VersionInformation,

      // TestMode
      Test1,
      Test2,
      Test3,
    };

    namespace Control {
      enum ID : int {
        MainTree = 3000,
        MainToolBar,
        MainList,
      };
    }

    namespace MainToolBar {
      enum ID : int {
        // �\��
        AutoDisplayCells         = CommandID::AutoDisplayCells,
        SelectColumnGroup        = CommandID::SelectColumnGroup,
        DisplayCells             = CommandID::DisplayCells,
        Reload                   = CommandID::Reload,
        // �ҏW
        EditCommonParameter      = CommandID::EditCommonParameter,
        AddEntry                 = CommandID::AddEntry,
        DeleteEntry              = CommandID::DeleteEntry,
        DeleteAllEntry           = CommandID::DeleteAllEntry,
        ShowPropertyOfEntry      = CommandID::ShowPropertyOfEntry,
        EditIndividualParameter  = CommandID::EditIndividualParameter,
        // �ϊ�
        ConvertOneBms            = CommandID::ConvertOneBms,
        ConvertAllBms            = CommandID::ConvertAllBms,
        // �c�[��
        ExecuteSquirrelScript    = CommandID::ExecuteSquirrelScript,
        ReloadSquirrelScript     = CommandID::ReloadSquirrelScript,
        ShowSquirrelOutputDialog = CommandID::ShowSquirrelOutputDialog,
        StartSearcher            = CommandID::StartSearcher,

        // ��
        SelectColumnLabel = 4000,
      };
    }
  }

  // -- IniFileOperation -------------------------------------------------
  namespace IniFileOperation {
    void SavePlacement( const WINDOWPLACEMENT& placement );
    bool LoadPlacement( WINDOWPLACEMENT& placement );
    void SaveAutoDisplayCells( bool flag );
    bool LoadAutoDisplayCells( void );
    std::string LoadLanguage( void );

    void SaveErrorLogDump( TtException& e );

    // Secret
    bool LoadTestMode( void );
  }

  namespace Image {
    // -- Index ------------------------------------------------------------
    struct Index {
      enum Value : int {
        None = -1,
        Main,
        MainSmall,
        BmsFile,
        AutoDisplayCells,
        DisplayCells,
        Reload,
        EditCommonParameter,
        AddEntry,
        DeleteEntry,
        DeleteAllEntry,
        ShowPropertyOfEntry,
        EditIndividualParameter,
        ConvertOneBms,
        ConvertAllBms,
        ExecuteSquirrelScript,
        ReloadSquirrelScript,
        ShowSquirrelOutputDialog,
        SearcherMainSmall,
      };
    };

    // -- Objects
    extern std::vector<TtIcon>              ICONS;
    extern std::vector<TtBmpImage>          BMPS;
    extern std::optional<TtSystemImageList> LIST;

    extern int FOLDER_ICON_INDEX;

    void Initialize( void );
  }
}
