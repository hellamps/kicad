#ifndef __PCBNEW_ID_H__
#define __PCBNEW_ID_H__

#include <id.h>

/**
 * Command IDs for the printed circuit board editor.
 *
 * Please add IDs that are unique to the printed circuit board editor (Pcbnew)
 * here and not in the global id.h file.  This will prevent the entire project
 * from being rebuilt when adding new commands to the Pcbnew.
 */

enum pcbnew_ids
{
    ID_MAIN_MENUBAR = ID_END_LIST,
    ID_MICROWAVE_V_TOOLBAR,
    ID_OPEN_MODULE_EDITOR,
    ID_OPEN_MODULE_VIEWER,
    ID_READ_NETLIST,
    ID_SET_RELATIVE_OFFSET,

    // Right vertical tool bar command IDs.
    ID_PCB_HIGHLIGHT_BUTT,
    ID_PCB_SHOW_1_RATSNEST_BUTT,
    ID_PCB_MODULE_BUTT,
    ID_TRACK_BUTT,
    ID_PCB_ZONES_BUTT,
    ID_PCB_ADD_LINE_BUTT,
    ID_PCB_CIRCLE_BUTT,
    ID_PCB_ARC_BUTT,
    ID_PCB_ADD_TEXT_BUTT,
    ID_PCB_DIMENSION_BUTT,
    ID_PCB_MIRE_BUTT,
    ID_PCB_DELETE_ITEM_BUTT,
    ID_PCB_PLACE_OFFSET_COORD_BUTT,
    ID_PCB_PLACE_GRID_COORD_BUTT,

    ID_PCB_MASK_CLEARANCE,
    ID_PCB_LAYERS_SETUP,

    ID_POPUP_PCB_START_RANGE,
    ID_POPUP_PCB_MOVE_MODULE_REQUEST,
    ID_POPUP_PCB_DRAG_MODULE_REQUEST,
    ID_POPUP_PCB_EDIT_MODULE,
    ID_POPUP_PCB_CHANGE_SIDE_MODULE,
    ID_POPUP_PCB_DELETE_MODULE,
    ID_POPUP_PCB_ROTATE_MODULE_CLOCKWISE,
    ID_POPUP_PCB_ROTATE_MODULE_COUNTERCLOCKWISE,

    ID_POPUP_PCB_EDIT_PAD,
    ID_POPUP_PCB_EDIT_MORE_PAD,
    ID_POPUP_PCB_EDIT_PADS_ON_MODULE,
    ID_POPUP_PCB_DELETE_PAD,
    ID_POPUP_PCB_ADD_PAD,
    ID_POPUP_PCB_NEWSIZE_PAD,
    ID_POPUP_PCB_ROTATE_PAD,
    ID_POPUP_PCB_MOVE_PAD_REQUEST,
    ID_POPUP_PCB_DRAG_PAD_REQUEST,

    ID_POPUP_PCB_MOVE_TEXTMODULE_REQUEST,
    ID_POPUP_PCB_ROTATE_TEXTMODULE,
    ID_POPUP_PCB_EDIT_TEXTMODULE,
    ID_POPUP_PCB_DELETE_TEXTMODULE,
    ID_POPUP_PCB_RESET_TEXT_SIZE,

    ID_POPUP_PCB_MOVE_TEXTEPCB_REQUEST,
    ID_POPUP_PCB_ROTATE_TEXTEPCB,
    ID_POPUP_PCB_EDIT_TEXTEPCB,
    ID_POPUP_PCB_DELETE_TEXTEPCB,

    ID_POPUP_PCB_MOVE_DRAWING_REQUEST,
    ID_POPUP_PCB_EDIT_DRAWING,
    ID_POPUP_PCB_DELETE_DRAWING,
    ID_POPUP_PCB_DELETE_DRAWING_LAYER,
    ID_POPUP_PCB_END_LINE,

    ID_POPUP_PCB_EDIT_TRACK,
    ID_POPUP_PCB_DELETE_TRACK,
    ID_POPUP_PCB_DELETE_TRACKNET,
    ID_POPUP_PCB_DELETE_TRACK_MNU,

    ID_POPUP_PCB_MOVE_ZONE_CORNER,
    ID_POPUP_PCB_ADD_ZONE_CORNER,
    ID_POPUP_PCB_DELETE_ZONE_CORNER,
    ID_POPUP_PCB_PLACE_ZONE_CORNER,
    ID_POPUP_PCB_DELETE_ZONE_LAST_CREATED_CORNER,
    ID_POPUP_PCB_EDIT_ZONE_PARAMS,
    ID_POPUP_PCB_DELETE_ZONE,
    ID_POPUP_PCB_STOP_CURRENT_EDGE_ZONE,
    ID_POPUP_PCB_FILL_ALL_ZONES,
    ID_POPUP_PCB_FILL_ZONE,
    ID_POPUP_PCB_DELETE_ZONE_CONTAINER,
    ID_POPUP_PCB_ZONE_ADD_SIMILAR_ZONE,
    ID_POPUP_PCB_ZONE_ADD_CUTOUT_ZONE,
    ID_POPUP_PCB_DELETE_ZONE_CUTOUT,
    ID_POPUP_PCB_MOVE_ZONE_OUTLINES,
    ID_POPUP_PCB_PLACE_ZONE_OUTLINES,
    ID_POPUP_PCB_DRAG_ZONE_OUTLINE_SEGMENT,
    ID_POPUP_PCB_PLACE_DRAGGED_ZONE_OUTLINE_SEGMENT,
    ID_POPUP_PCB_REMOVE_FILLED_AREAS_IN_ALL_ZONES,
    ID_POPUP_PCB_REMOVE_FILLED_AREAS_IN_CURRENT_ZONE,

    ID_POPUP_PCB_DELETE_MARKER,

    ID_POPUP_PCB_DELETE_DIMENSION,
    ID_POPUP_PCB_MOVE_TEXT_DIMENSION_REQUEST,

    ID_POPUP_PCB_MOVE_MIRE_REQUEST,
    ID_POPUP_PCB_DELETE_MIRE,
    ID_POPUP_PCB_EDIT_MIRE,

    ID_POPUP_PCB_STOP_CURRENT_DRAWING,

    ID_POPUP_PCB_EDIT_DIMENSION,
    ID_POPUP_PCB_END_TRACK,
    ID_POPUP_PCB_PLACE_VIA,
    ID_POPUP_PCB_PLACE_MICROVIA,
    ID_POPUP_PCB_SWITCH_TRACK_POSTURE,

    ID_POPUP_PCB_IMPORT_PAD_SETTINGS,
    ID_POPUP_PCB_EXPORT_PAD_SETTINGS,

    ID_POPUP_PCB_EDIT_TRACKSEG,
    ID_POPUP_PCB_EDIT_TRACK_MNU,
    ID_POPUP_PCB_EDIT_NET,
    ID_POPUP_PCB_SELECT_LAYER,
    ID_POPUP_PCB_SELECT_CU_LAYER,
    ID_POPUP_PCB_SELECT_NO_CU_LAYER,
    ID_POPUP_PCB_SELECT_LAYER_PAIR,
    ID_POPUP_PCB_DELETE_EDGE,
    ID_POPUP_PCB_MOVE_EDGE,
    ID_POPUP_PCB_PLACE_EDGE,
    ID_POPUP_PCB_END_EDGE,
    ID_POPUP_PCB_LOCK_ON_TRACKSEG,
    ID_POPUP_PCB_LOCK_OFF_TRACKSEG,
    ID_POPUP_PCB_LOCK_ON_TRACK,
    ID_POPUP_PCB_LOCK_OFF_TRACK,
    ID_POPUP_PCB_LOCK_ON_NET,
    ID_POPUP_PCB_LOCK_OFF_NET,
    ID_POPUP_PCB_SETFLAGS_TRACK_MNU,

    ID_POPUP_PCB_GLOBAL_IMPORT_PAD_SETTINGS,
    ID_POPUP_PCB_EDIT_ALL_VIAS_AND_TRACK_SIZE,
    ID_POPUP_PCB_EDIT_ALL_VIAS_SIZE,
    ID_POPUP_PCB_EDIT_ALL_TRACK_SIZE,
    ID_POPUP_PCB_DISPLAY_FOOTPRINT_DOC,
    ID_POPUP_PCB_MOVE_TRACK_NODE,
    ID_POPUP_PCB_BREAK_TRACK,
    ID_POPUP_PCB_PLACE_MOVED_TRACK_NODE,
    ID_POPUP_PCB_DRAG_TRACK_SEGMENT_KEEP_SLOPE,
    ID_POPUP_PCB_DRAG_TRACK_SEGMENT,
    ID_POPUP_PCB_MOVE_TRACK_SEGMENT,
    ID_POPUP_PCB_GET_AND_MOVE_MODULE_REQUEST,
    ID_POPUP_PCB_GETINFO_MARKER,
    ID_POPUP_PCB_END_RANGE,

    // Tracks and vias sizes general options
    ID_AUX_TOOLBAR_PCB_VIA_SIZE,
    ID_AUX_TOOLBAR_PCB_TRACK_WIDTH,
    ID_AUX_TOOLBAR_PCB_SELECT_AUTO_WIDTH,
    ID_POPUP_PCB_SELECT_WIDTH_START_RANGE,
    ID_POPUP_PCB_SELECT_WIDTH,
    ID_POPUP_PCB_SELECT_AUTO_WIDTH,
    ID_POPUP_PCB_SELECT_USE_NETCLASS_VALUES,
    ID_POPUP_PCB_SELECT_WIDTH1,
    ID_POPUP_PCB_SELECT_WIDTH2,
    ID_POPUP_PCB_SELECT_WIDTH3,
    ID_POPUP_PCB_SELECT_WIDTH4,
    ID_POPUP_PCB_SELECT_WIDTH5,
    ID_POPUP_PCB_SELECT_WIDTH6,
    ID_POPUP_PCB_SELECT_WIDTH7,
    ID_POPUP_PCB_SELECT_WIDTH8,
    ID_POPUP_PCB_SELECT_VIASIZE,
    ID_POPUP_PCB_SELECT_VIASIZE1,
    ID_POPUP_PCB_SELECT_VIASIZE2,
    ID_POPUP_PCB_SELECT_VIASIZE3,
    ID_POPUP_PCB_SELECT_VIASIZE4,
    ID_POPUP_PCB_SELECT_VIASIZE5,
    ID_POPUP_PCB_SELECT_VIASIZE6,
    ID_POPUP_PCB_SELECT_VIASIZE7,
    ID_POPUP_PCB_SELECT_VIASIZE8,
    ID_POPUP_PCB_SELECT_WIDTH_END_RANGE,

    // reserve a block of MAX_ITEMS_IN_PICKER ids for the item selection popup
    ID_POPUP_PCB_ITEM_SELECTION_START,
    ID_POPUP_PCB_ITEM_SELECTION_END = MAX_ITEMS_IN_PICKER + ID_POPUP_PCB_ITEM_SELECTION_START,
    ID_POPUP_PCB_AUTOPLACE_START_RANGE,
    ID_POPUP_PCB_AUTOPLACE_FIXE_MODULE,
    ID_POPUP_PCB_AUTOPLACE_FREE_MODULE,
    ID_POPUP_PCB_AUTOPLACE_FREE_ALL_MODULES,
    ID_POPUP_PCB_AUTOPLACE_FIXE_ALL_MODULES,
    ID_POPUP_PCB_AUTOPLACE_CURRENT_MODULE,
    ID_POPUP_PCB_AUTOMOVE_ALL_MODULES,
    ID_POPUP_PCB_AUTOMOVE_NEW_MODULES,
    ID_POPUP_PCB_AUTOPLACE_COMMANDS,
    ID_POPUP_PCB_AUTOPLACE_ALL_MODULES,
    ID_POPUP_PCB_AUTOPLACE_NEW_MODULES,
    ID_POPUP_PCB_AUTOPLACE_NEXT_MODULE,

    ID_POPUP_PCB_AUTOROUTE_COMMANDS,
    ID_POPUP_PCB_AUTOROUTE_ALL_MODULES,
    ID_POPUP_PCB_AUTOROUTE_MODULE,
    ID_POPUP_PCB_AUTOROUTE_PAD,
    ID_POPUP_PCB_AUTOROUTE_NET,

    ID_POPUP_PCB_AUTOROUTE_RESET_UNROUTED,
    ID_POPUP_PCB_AUTOROUTE_SELECT_LAYERS,
    ID_POPUP_PCB_AUTOPLACE_END_RANGE,

    ID_POPUP_PCB_REORIENT_ALL_MODULES,

    ID_MENU_RECOVER_BOARD,
    ID_MENU_READ_LAST_SAVED_VERSION_BOARD,
    ID_MENU_ARCHIVE_MODULES,
    ID_MENU_ARCHIVE_NEW_MODULES,
    ID_MENU_ARCHIVE_ALL_MODULES,
    ID_MENU_MICELLANOUS,
    ID_MENU_LIST_NETS,
    ID_MENU_PCB_CLEAN,
    ID_MENU_PCB_SWAP_LAYERS,
    ID_MENU_PCB_RESET_TEXTMODULE_REFERENCE_SIZES,
    ID_MENU_PCB_RESET_TEXTMODULE_VALUE_SIZES,

    ID_GEN_EXPORT_FILE_VRML,
    ID_GEN_EXPORT_SPECCTRA,
    ID_GEN_EXPORT_FILE_GENCADFORMAT,
    ID_GEN_EXPORT_FILE_MODULE_REPORT,
    ID_GEN_IMPORT_SPECCTRA_SESSION,
    ID_GEN_IMPORT_SPECCTRA_DESIGN,

    ID_TOOLBARH_PCB_MODE_MODULE,
    ID_TOOLBARH_PCB_MODE_TRACKS,
    ID_TOOLBARH_PCB_FREEROUTE_ACCESS,

    ID_AUX_TOOLBAR_PCB_SELECT_LAYER_PAIR,

    ID_PCB_GEN_POS_MODULES_FILE,
    ID_PCB_GEN_DRILL_FILE,

    ID_PCB_PAD_SETUP,

    ID_PCB_DRAWINGS_WIDTHS_SETUP,

    ID_PCB_GEN_CMP_FILE,
    ID_MENU_PCB_SHOW_3D_FRAME,
    ID_PCB_USER_GRID_SETUP,
    ID_PCB_GEN_BOM_FILE_FROM_BOARD,
    ID_MENU_PCB_SHOW_DESIGN_RULES_DIALOG,
    ID_MENU_PCB_SHOW_HIDE_LAYERS_MANAGER_DIALOG,

    ID_TB_OPTIONS_SHOW_MANAGE_LAYERS_VERTICAL_TOOLBAR,
    ID_TB_OPTIONS_SHOW_ZONES,
    ID_TB_OPTIONS_SHOW_ZONES_DISABLE,
    ID_TB_OPTIONS_SHOW_ZONES_OUTLINES_ONLY,
    ID_TB_OPTIONS_DRC_OFF,
    ID_TB_OPTIONS_SHOW_RATSNEST,
    ID_TB_OPTIONS_SHOW_MODULE_RATSNEST,
    ID_TB_OPTIONS_AUTO_DEL_TRACK,
    ID_TB_OPTIONS_SHOW_VIAS_SKETCH,
    ID_TB_OPTIONS_SHOW_TRACKS_SKETCH,
    ID_TB_OPTIONS_SHOW_HIGH_CONTRAST_MODE,
    ID_TB_OPTIONS_SHOW_EXTRA_VERTICAL_TOOLBAR_MICROWAVE,

    ID_PCB_MUWAVE_START_CMD,
    ID_PCB_MUWAVE_TOOL_DISPLAY_TOOLS,
    ID_PCB_MUWAVE_TOOL_SELF_CMD,
    ID_PCB_MUWAVE_TOOL_GAP_CMD,
    ID_PCB_MUWAVE_TOOL_STUB_CMD,
    ID_PCB_MUWAVE_TOOL_STUB_ARC_CMD,
    ID_PCB_MUWAVE_TOOL_FUNCTION_SHAPE_CMD,
    ID_PCB_MUWAVE_END_CMD,

    ID_DRC_CONTROL,
    ID_PCB_GLOBAL_DELETE,
    ID_POPUP_PCB_DELETE_TRACKSEG,
    ID_TOOLBARH_PCB_SELECT_LAYER,
    ID_PCB_DISPLAY_OPTIONS_SETUP,

    // Module editor right vertical tool bar commands.
    ID_MODEDIT_PAD_TOOL,
    ID_MODEDIT_LINE_TOOL,
    ID_MODEDIT_CIRCLE_TOOL,
    ID_MODEDIT_ARC_TOOL,
    ID_MODEDIT_TEXT_TOOL,
    ID_MODEDIT_ANCHOR_TOOL,
    ID_MODEDIT_DELETE_TOOL,
    ID_MODEDIT_PLACE_GRID_COORD,

    // ID used in module editor:
    ID_POPUP_MODEDIT_EDIT_WIDTH_CURRENT_EDGE,
    ID_POPUP_MODEDIT_EDIT_WIDTH_ALL_EDGE,
    ID_POPUP_MODEDIT_EDIT_LAYER_CURRENT_EDGE,
    ID_POPUP_MODEDIT_EDIT_LAYER_ALL_EDGE,
    ID_POPUP_MODEDIT_ENTER_EDGE_WIDTH,
    ID_POPUP_MODEDIT_EDIT_EDGE,
    ID_MODEDIT_CHECK,
    ID_MODEDIT_SELECT_CURRENT_LIB,
    ID_MODEDIT_SAVE_LIBMODULE,
    ID_MODEDIT_DELETE_PART,
    ID_MODEDIT_NEW_MODULE,
    ID_MODEDIT_SHEET_SET,
    ID_MODEDIT_LOAD_MODULE,
    ID_MODEDIT_PAD_SETTINGS,
    ID_MODEDIT_LOAD_MODULE_FROM_BOARD,
    ID_MODEDIT_INSERT_MODULE_IN_BOARD,
    ID_MODEDIT_UPDATE_MODULE_IN_BOARD,
    ID_MODEDIT_EDIT_MODULE_PROPERTIES,
    ID_MODEDIT_TRANSFORM_MODULE,
    ID_MODEDIT_MODULE_ROTATE,
    ID_MODEDIT_MODULE_MIRROR,
    ID_MODEDIT_IMPORT_PART,
    ID_MODEDIT_EXPORT_PART,
    ID_MODEDIT_CREATE_NEW_LIB_AND_SAVE_CURRENT_PART,
    ID_POPUP_MODEDIT_EDIT_BODY_ITEM,

    ID_MODVIEW_LIBWINDOW,
    ID_MODVIEW_FOOTPRINT_WINDOW,
    ID_MODVIEW_LIB_LIST,
    ID_MODVIEW_FOOTPRINT_LIST,
    ID_MODVIEW_SELECT_LIB,
    ID_MODVIEW_SELECT_PART,
    ID_MODVIEW_PREVIOUS,
    ID_MODVIEW_NEXT,
    ID_MODVIEW_SHOW_3D_VIEW,
    ID_MODVIEW_FOOTPRINT_EXPORT_TO_BOARD

};

#endif  /* __PCBNEW_IDS_H__  */
