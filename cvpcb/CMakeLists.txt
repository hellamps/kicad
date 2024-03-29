add_definitions(-DCVPCB)

###
# Includes
###

include_directories(BEFORE ${INC_BEFORE})
include_directories(
    ./dialogs
    ${Boost_INCLUDE_DIR}
    ../3d-viewer
    ../pcbnew
    ../polygon
    ../common
    ${INC_AFTER}
    )

###
# Sources
###
set(CVPCB_DIALOGS
    dialogs/dialog_cvpcb_config.cpp
    dialogs/dialog_cvpcb_config_fbp.cpp
    dialogs/dialog_display_options.cpp
    dialogs/dialog_display_options_base.cpp
    )

set(CVPCB_SRCS
    ../pcbnew/netlist_reader_common.cpp
    ../pcbnew/netlist_reader_kicad.cpp
    ../pcbnew/netlist_reader_firstformat.cpp
    ../pcbnew/class_drc_item.cpp
    autosel.cpp
    cfg.cpp
    class_components_listbox.cpp
    class_DisplayFootprintsFrame.cpp
    class_footprints_listbox.cpp
    cvframe.cpp
    cvpcb.cpp
    listboxes.cpp
    loadcmp.cpp
    menubar.cpp
    readschematicnetlist.cpp
    read_write_cmpfile.cpp
    readwrite_dlgs.cpp
    setvisu.cpp
    tool_cvpcb.cpp
    )

###
# Windows resource file
###
if(WIN32)
    if(MINGW)
        # CVPCB_RESOURCES variable is set by the macro.
        mingw_resource_compiler(cvpcb)
    else(MINGW)
        set(CVPCB_RESOURCES cvpcb.rc)
    endif(MINGW)
endif(WIN32)

###
# Apple resource files
###
if(APPLE)
    set(CVPCB_RESOURCES cvpcb.icns cvpcb_doc.icns)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/cvpcb.icns"
                                PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/cvpcb_doc.icns"
                                PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(MACOSX_BUNDLE_ICON_FILE cvpcb.icns)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER org.kicad-eda.cvpcb)
endif(APPLE)

###
# Create the cvpcb executable
###
add_executable(cvpcb WIN32 MACOSX_BUNDLE
    ${CVPCB_SRCS}
    ${CVPCB_DIALOGS}
    ${CVPCB_RESOURCES}
    )

###
# Set properties for APPLE on cvpcb target
###
if(APPLE)
    set_target_properties(cvpcb PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
endif(APPLE)

###
# Link executable target cvpcb with correct libraries
###
target_link_libraries(cvpcb
    3d-viewer
    pcbcommon
    common
    bitmaps
    polygon
    kbool
    ${wxWidgets_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GDI_PLUS_LIBRARIES}
    )

###
# Add cvpcb as install target
###
install(TARGETS cvpcb
        DESTINATION ${KICAD_BIN}
        COMPONENT binary)
