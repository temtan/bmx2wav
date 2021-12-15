// searcher/searcher_common.h

#pragma once

#include "tt_image_list.h"
#include "tt_exception.h"


namespace BMX2WAV::Searcher {
  const char* const APPLICATION_NAME = "BMX2WAV Searcher";

  // -- Const ------------------------------------------------------------
  namespace Const {
    const unsigned int DefaultSplitterPosition = 240;
  }

  // -- CommandID --------------------------------------------------------
  namespace CommandID {
    enum ID : int {
      // ファイル
      Close = 2000,
      Export,
      ExportCSVToFile,
      ExportTSVToClipboard,

      Settings,

      ReloadSquirrelScript,
      ShowSquirrelOutputDialog,

      SelectFilter,

      AutoDisplayCells,
      SelectColumnGroup,
      DisplayCells,

      SelectSearch,
      ExecuteSearch,

      // ポップアップメニュー
      OpenTreeDirectory,
      HideEntry,

      Test1,
      Test2,
      Test3,
    };

    namespace Control {
      enum ID : int {
        MainToolBar = 3000,
        MainTree,
        MainList,
      };
    }

    namespace MainToolBar {
      enum ID : int {
        Export = CommandID::Export,

        ReloadSquirrelScript     = CommandID::ReloadSquirrelScript,
        ShowSquirrelOutputDialog = CommandID::ShowSquirrelOutputDialog,

        SelectFilter  = CommandID::SelectFilter,

        AutoDisplayCells  = CommandID::AutoDisplayCells,
        SelectColumnGroup = CommandID::SelectColumnGroup,
        DisplayCells      = CommandID::DisplayCells,

        SelectSearch  = CommandID::SelectSearch,
        ExecuteSearch = CommandID::ExecuteSearch,

        // 他
        FilterLabel = 5000,
        SelectColumnLabel,
        ExecuteSearchLabel,
      };
    }
  }

  // -- IniFileOperation -------------------------------------------------
  namespace IniFileOperation {
    void SavePlacement( const WINDOWPLACEMENT& placement );
    bool LoadPlacement( WINDOWPLACEMENT& placement );

    void SaveSplitterPosition( unsigned int width );
    unsigned int LoadSplitterPosition( void );

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
        Export,
        ReloadSquirrelScript,
        ShowSquirrelOutputDialog,
        AutoDisplayCells,
        DisplayCells,
        ExecuteSearch,
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
