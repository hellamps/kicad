
include_directories(BEFORE ${INC_BEFORE})
include_directories(
    ./dialogs
    ./dialog_about
    ${Boost_INCLUDE_DIR}
    ../3d-viewer
    ../pcbnew
    ../polygon
    ${INC_AFTER}
    )

set(COMMON_ABOUT_DLG_SRCS
    dialog_about/AboutDialog_main.cpp
    dialog_about/dialog_about.cpp
    dialog_about/dialog_about_base.cpp
    dialogs/dialog_display_info_HTML_base.cpp
    dialogs/dialog_exit_base.cpp
    dialogs/dialog_image_editor.cpp
    dialogs/dialog_image_editor_base.cpp
    dialogs/dialog_get_component.cpp
    dialogs/dialog_get_component_base.cpp
    dialogs/dialog_hotkeys_editor.cpp
    dialogs/dialog_hotkeys_editor_base.cpp
    dialogs/dialog_page_settings_base.cpp
    )

set(COMMON_SRCS
    ${COMMON_ABOUT_DLG_SRCS}
    base_struct.cpp
    basicframe.cpp
    bezier_curves.cpp
    bitmap.cpp
    block_commande.cpp
    build_version.cpp
    class_bitmap_base.cpp
    class_colors_design_settings.cpp
    class_marker_base.cpp
    class_page_info.cpp
    class_plotter.cpp
    class_undoredo_container.cpp
    common.cpp
    common_plot_functions.cpp
    common_plotHPGL_functions.cpp
    common_plotPS_functions.cpp
    common_plotGERBER_functions.cpp
    common_plotDXF_functions.cpp
    confirm.cpp
    copy_to_clipboard.cpp
    dcsvg.cpp
    displlst.cpp
    dlist.cpp
    drawframe.cpp
    drawpanel.cpp
    drawtxt.cpp
    dsnlexer.cpp
    edaappl.cpp
    eda_dde.cpp
    eda_doc.cpp
    filter_reader.cpp
    gestfich.cpp
    getrunningmicrosecs.cpp
    gr_basic.cpp
    hotkeys_basic.cpp
    hotkey_grid_table.cpp
    html_messagebox.cpp
    msgpanel.cpp
    netlist_keywords.cpp
    newstroke_font.cpp
    projet_config.cpp
    richio.cpp
    selcolor.cpp
    string.cpp
    trigo.cpp
    wildcards_and_files_ext.cpp
    worksheet.cpp
    wxwineda.cpp
    xnode.cpp
    zoom.cpp
    )

add_library(common STATIC ${COMMON_SRCS})

set(PCB_COMMON_SRCS
    base_screen.cpp
    pcbcommon.cpp
    footprint_info.cpp
    class_layer_box_selector.cpp
    ../pcbnew/basepcbframe.cpp
    ../pcbnew/class_board.cpp
    ../pcbnew/class_board_connected_item.cpp
    ../pcbnew/class_board_design_settings.cpp
    ../pcbnew/class_board_item.cpp
    ../pcbnew/class_dimension.cpp
    ../pcbnew/class_drawsegment.cpp
    ../pcbnew/class_drc_item.cpp
    ../pcbnew/class_edge_mod.cpp
    ../pcbnew/class_netclass.cpp
    ../pcbnew/class_netinfo_item.cpp
    ../pcbnew/class_netinfolist.cpp
    ../pcbnew/class_marker_pcb.cpp
    ../pcbnew/class_mire.cpp
    ../pcbnew/class_module.cpp
    ../pcbnew/class_module_transform_functions.cpp
    ../pcbnew/class_pad.cpp
    ../pcbnew/class_pad_draw_functions.cpp
    ../pcbnew/class_pcb_text.cpp
    ../pcbnew/class_text_mod.cpp
    ../pcbnew/class_track.cpp
    ../pcbnew/class_zone.cpp
    ../pcbnew/class_zone_settings.cpp
    ../pcbnew/classpcb.cpp
    ../pcbnew/collectors.cpp
    ../pcbnew/sel_layer.cpp
    pcb_plot_params_keywords.cpp
    dialogs/dialog_page_settings.cpp
)

if ( USE_NEW_PCBNEW_LOAD OR USE_NEW_PCBNEW_SAVE )
    set( PCB_COMMON_SRCS ${PCB_COMMON_SRCS} ../pcbnew/item_io.cpp ../pcbnew/io_mgr.cpp ../pcbnew/kicad_plugin.cpp )
else()
    set( PCB_COMMON_SRCS ${PCB_COMMON_SRCS} ../pcbnew/item_io.cpp )
endif()

# add -DPCBNEW to compilation of these PCBNEW sources
set_source_files_properties( ${PCB_COMMON_SRCS} PROPERTIES
    COMPILE_DEFINITIONS "PCBNEW"
    )

add_library(pcbcommon STATIC ${PCB_COMMON_SRCS})


# auto-generate netlist_lexer.h and netlist_keywords.cpp
make_lexer(
    ${CMAKE_CURRENT_SOURCE_DIR}/netlist.keywords
    ${PROJECT_SOURCE_DIR}/include/netlist_lexer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/netlist_keywords.cpp
    NL_T
    )

# auto-generate pcb_plot_params_lexer.h and pcb_plot_params_keywords.cpp
make_lexer(
    ${CMAKE_CURRENT_SOURCE_DIR}/pcb_plot_params.keywords
    ${CMAKE_CURRENT_SOURCE_DIR}/pcb_plot_params_lexer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pcb_plot_params_keywords.cpp
    PCBPLOTPARAMS_T
    )

# The dsntest may not build properly using MS Visual Studio.
if(NOT MSVC)
    # This one gets made only when testing.
    # to build it, first enable #define STAND_ALONE at top of dsnlexer.cpp
    add_executable( dsntest EXCLUDE_FROM_ALL dsnlexer.cpp )
    target_link_libraries( dsntest common ${wxWidgets_LIBRARIES} )

endif( NOT MSVC )
