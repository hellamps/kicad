
#ifndef BITMAPS_H_
#define BITMAPS_H_

//FIXME: cannot include only this file in wxWidgets 2.9.3
// test if it works under stable release
// #include <wx/bitmap.h>   // only to define wxBitmap
class wxBitmap;     // only to define wxBitmap

#include <config.h>


#if defined(USE_PNG_BITMAPS)

#define VTOOLBAR_WIDTH  29
#define TOOL_SIZE       26


/// PNG memory record (file in memory).
struct BITMAP_OPAQUE
{
    const unsigned char* png;
    int         byteCount;
    const char* name;       // for debug, or future lazy dynamic linking
};

// declared as single element _array_, so its name assigns to pointer
#define EXTERN_BITMAP(x) extern const BITMAP_OPAQUE x[1];

#else   // XPM

#define VTOOLBAR_WIDTH  26
#define TOOL_SIZE       23

// temporary during migration to KiBitmap() and KiBitmapNew().
typedef const char*     BITMAP_OPAQUE;

#define EXTERN_BITMAP(x) extern const char* x[];

#endif

/// a BITMAP_DEF is really a const pointer to an opaque
/// structure.  So you should never need to use 'const' with it.
typedef const BITMAP_OPAQUE *BITMAP_DEF;


/**
 * Function KiBitmap
 * constructs a wxBitmap from a memory record, held in a
 * BITMAP_DEF.
 */
wxBitmap KiBitmap( BITMAP_DEF aBitmap );


/**
 * Function KiBitmapNew
 * allocates a wxBitmap on heap from a memory record, held in a
 * BITMAP_DEF.
 *
 * @return wxBitmap* - caller owns it.
 */
wxBitmap* KiBitmapNew( BITMAP_DEF aBitmap );


// may eventually generate this file automatically.

EXTERN_BITMAP( shape_3d_xpm )
EXTERN_BITMAP( add_arc_xpm )
EXTERN_BITMAP( add_bus2bus_xpm )
EXTERN_BITMAP( add_bus_xpm )
EXTERN_BITMAP( add_circle_xpm )
EXTERN_BITMAP( add_component_xpm )
EXTERN_BITMAP( add_corner_xpm )
EXTERN_BITMAP( add_dashed_line_xpm )
EXTERN_BITMAP( add_dimension_xpm )
EXTERN_BITMAP( add_entry_xpm )
EXTERN_BITMAP( add_glabel_xpm )
EXTERN_BITMAP( add_hierarchical_label_xpm )
EXTERN_BITMAP( add_hierarchical_subsheet_xpm )
EXTERN_BITMAP( add_hierar_pin_xpm )
EXTERN_BITMAP( add_junction_xpm )
EXTERN_BITMAP( add_line2bus_xpm )
EXTERN_BITMAP( add_line_label_xpm )
EXTERN_BITMAP( add_line_xpm )
EXTERN_BITMAP( add_mires_xpm )
EXTERN_BITMAP( add_polygon_xpm )
EXTERN_BITMAP( add_power_xpm )
EXTERN_BITMAP( add_rectangle_xpm )
EXTERN_BITMAP( add_text_xpm )
EXTERN_BITMAP( add_tracks_xpm )
EXTERN_BITMAP( add_zone_cutout_xpm )
EXTERN_BITMAP( add_zone_xpm )
EXTERN_BITMAP( icon_pcbnew_xpm )
EXTERN_BITMAP( anchor_xpm )
EXTERN_BITMAP( annotate_down_right_xpm )
EXTERN_BITMAP( annotate_right_down_xpm )
EXTERN_BITMAP( annotate_xpm )
EXTERN_BITMAP( apply_xpm )
EXTERN_BITMAP( auto_associe_xpm )
EXTERN_BITMAP( auto_delete_track_xpm )
EXTERN_BITMAP( auto_track_width_xpm )
EXTERN_BITMAP( axis3d_back_xpm )
EXTERN_BITMAP( axis3d_bottom_xpm )
EXTERN_BITMAP( axis3d_front_xpm )
EXTERN_BITMAP( axis3d_left_xpm )
EXTERN_BITMAP( axis3d_right_xpm )
EXTERN_BITMAP( axis3d_top_xpm )
EXTERN_BITMAP( axis3d_xpm )
EXTERN_BITMAP( bom_xpm )
EXTERN_BITMAP( book_xpm )
EXTERN_BITMAP( break_bus_xpm )
EXTERN_BITMAP( break_line_xpm )
EXTERN_BITMAP( browse_files_xpm )
EXTERN_BITMAP( cancel_xpm )
EXTERN_BITMAP( change_entry_orient_xpm )
EXTERN_BITMAP( checked_ok_xpm )
EXTERN_BITMAP( component_select_alternate_shape_xpm )
EXTERN_BITMAP( component_select_unit_xpm )
EXTERN_BITMAP( config_xpm )
EXTERN_BITMAP( copper_layers_setup_xpm )
EXTERN_BITMAP( copyblock_xpm )
EXTERN_BITMAP( copycomponent_xpm )
EXTERN_BITMAP( copy_button_xpm )
EXTERN_BITMAP( create_cmp_file_xpm )
EXTERN_BITMAP( cursor_shape_xpm )
EXTERN_BITMAP( cursor_xpm )
EXTERN_BITMAP( cut_button_xpm )
EXTERN_BITMAP( cvpcb_xpm )
EXTERN_BITMAP( dashedline_xpm )
EXTERN_BITMAP( datasheet_xpm )
EXTERN_BITMAP( delete_arc_xpm )
EXTERN_BITMAP( delete_association_xpm )
EXTERN_BITMAP( delete_bus_xpm )
EXTERN_BITMAP( delete_circle_xpm )
EXTERN_BITMAP( delete_connection_xpm )
EXTERN_BITMAP( delete_cotation_xpm )
EXTERN_BITMAP( delete_field_xpm )
EXTERN_BITMAP( delete_glabel_xpm )
EXTERN_BITMAP( delete_line_xpm )
EXTERN_BITMAP( delete_module_xpm )
EXTERN_BITMAP( delete_net_xpm )
EXTERN_BITMAP( delete_node_xpm )
EXTERN_BITMAP( delete_pad_xpm )
EXTERN_BITMAP( delete_pinsheet_xpm )
EXTERN_BITMAP( delete_pin_xpm )
EXTERN_BITMAP( delete_polygon_xpm )
EXTERN_BITMAP( delete_rectangle_xpm )
EXTERN_BITMAP( delete_segment_xpm )
EXTERN_BITMAP( delete_sheet_xpm )
EXTERN_BITMAP( delete_text_xpm )
EXTERN_BITMAP( delete_track_xpm )
EXTERN_BITMAP( delete_xpm )
EXTERN_BITMAP( dialog_warning_xpm )
EXTERN_BITMAP( directory_xpm )
EXTERN_BITMAP( display_options_xpm )
EXTERN_BITMAP( down_xpm )
EXTERN_BITMAP( ortho_xpm )
EXTERN_BITMAP( drag_module_xpm )
EXTERN_BITMAP( drag_outline_segment_xpm )
EXTERN_BITMAP( drag_pad_xpm )
EXTERN_BITMAP( drag_segment_withslope_xpm )
EXTERN_BITMAP( drag_track_segment_xpm )
EXTERN_BITMAP( drc_off_xpm )
EXTERN_BITMAP( drc_xpm )
EXTERN_BITMAP( edges_sketch_xpm )
EXTERN_BITMAP( edit_comp_footprint_xpm )
EXTERN_BITMAP( edit_component_xpm )
EXTERN_BITMAP( edit_comp_ref_xpm )
EXTERN_BITMAP( edit_comp_value_xpm )
EXTERN_BITMAP( edit_module_xpm )
EXTERN_BITMAP( editor_xpm )
EXTERN_BITMAP( edit_part_xpm )
EXTERN_BITMAP( edit_sheet_xpm )
EXTERN_BITMAP( edit_text_xpm )
EXTERN_BITMAP( edit_xpm )
EXTERN_BITMAP( eeschema_xpm )
EXTERN_BITMAP( enter_sheet_xpm )
EXTERN_BITMAP( erc_xpm )
EXTERN_BITMAP( ercerr_xpm )
EXTERN_BITMAP( ercwarn_xpm )
EXTERN_BITMAP( erc_green_xpm )
EXTERN_BITMAP( exit_xpm )
EXTERN_BITMAP( export_footprint_names_xpm )
EXTERN_BITMAP( export_module_xpm )
EXTERN_BITMAP( export_options_pad_xpm )
EXTERN_BITMAP( export_xpm )
EXTERN_BITMAP( fabrication_xpm )
EXTERN_BITMAP( file_footprint_xpm )
EXTERN_BITMAP( fill_zone_xpm )
EXTERN_BITMAP( find_xpm )
EXTERN_BITMAP( find_replace_xpm )
EXTERN_BITMAP( flag_xpm )
EXTERN_BITMAP( fonts_xpm )
EXTERN_BITMAP( footprint_text_xpm )
EXTERN_BITMAP( gbr_select_mode0_xpm )
EXTERN_BITMAP( gbr_select_mode1_xpm )
EXTERN_BITMAP( gbr_select_mode2_xpm )
EXTERN_BITMAP( gerbview_drill_file_xpm )
EXTERN_BITMAP( gerber_file_xpm )
EXTERN_BITMAP( gerber_recent_files_xpm )
EXTERN_BITMAP( gerbview_clear_layers_xpm )
EXTERN_BITMAP( gerber_open_dcode_file_xpm )
EXTERN_BITMAP( gerbview_open_recent_drill_files_xpm )
EXTERN_BITMAP( general_deletions_xpm )
EXTERN_BITMAP( general_ratsnest_xpm )
EXTERN_BITMAP( glabel2label_xpm )
EXTERN_BITMAP( glabel2text_xpm )
EXTERN_BITMAP( gl_change_xpm )
EXTERN_BITMAP( global_options_pad_xpm )
EXTERN_BITMAP( green_xpm )
EXTERN_BITMAP( grid_select_axis_xpm )
EXTERN_BITMAP( grid_select_xpm )
EXTERN_BITMAP( grid_xpm )
EXTERN_BITMAP( hammer_xpm )
EXTERN_BITMAP( help_xpm )
EXTERN_BITMAP( hidden_pin_xpm )
EXTERN_BITMAP( hierarchy_cursor_xpm )
EXTERN_BITMAP( hierarchy_nav_xpm )
EXTERN_BITMAP( hotkeys_xpm )
EXTERN_BITMAP( icon_cvpcb_small_xpm )
EXTERN_BITMAP( icon_cvpcb_xpm )
EXTERN_BITMAP( icon_eeschema_xpm )
EXTERN_BITMAP( icon_gerbview_small_xpm )
EXTERN_BITMAP( icon_gerbview_xpm )
EXTERN_BITMAP( icon_kicad_xpm )
EXTERN_BITMAP( icon_modedit_xpm )
EXTERN_BITMAP( icon_txt_xpm )
EXTERN_BITMAP( icon_3d_xpm )
EXTERN_BITMAP( icon_bitmap2component_xpm )
EXTERN_BITMAP( icon_pcbcalculator_xpm )
EXTERN_BITMAP( image_xpm )
EXTERN_BITMAP( import_cmp_from_lib_xpm )
EXTERN_BITMAP( import_footprint_names_xpm )
EXTERN_BITMAP( import_hierarchical_label_xpm )
EXTERN_BITMAP( import_module_xpm )
EXTERN_BITMAP( import_xpm )
EXTERN_BITMAP( import3d_xpm )
EXTERN_BITMAP( info_xpm )
EXTERN_BITMAP( insert_module_board_xpm )
EXTERN_BITMAP( invert_module_xpm )
EXTERN_BITMAP( invisible_text_xpm )
EXTERN_BITMAP( jigsaw_xpm )
EXTERN_BITMAP( kicad_icon_small_xpm )
EXTERN_BITMAP( label2glabel_xpm )
EXTERN_BITMAP( label2text_xpm )
EXTERN_BITMAP( label_xpm )
EXTERN_BITMAP( lang_bg_xpm )
EXTERN_BITMAP( lang_catalan_xpm )
EXTERN_BITMAP( lang_chinese_xpm )
EXTERN_BITMAP( lang_cs_xpm )
EXTERN_BITMAP( lang_def_xpm )
EXTERN_BITMAP( lang_de_xpm )
EXTERN_BITMAP( lang_en_xpm )
EXTERN_BITMAP( lang_es_xpm )
EXTERN_BITMAP( lang_fr_xpm )
EXTERN_BITMAP( lang_fi_xpm )
EXTERN_BITMAP( lang_gr_xpm )
EXTERN_BITMAP( lang_hu_xpm )
EXTERN_BITMAP( lang_it_xpm )
EXTERN_BITMAP( lang_jp_xpm )
EXTERN_BITMAP( lang_ko_xpm )
EXTERN_BITMAP( lang_nl_xpm )
EXTERN_BITMAP( lang_pl_xpm )
EXTERN_BITMAP( lang_pt_xpm )
EXTERN_BITMAP( lang_ru_xpm )
EXTERN_BITMAP( lang_sl_xpm )
EXTERN_BITMAP( language_xpm )
EXTERN_BITMAP( layers_manager_xpm )
EXTERN_BITMAP( leave_sheet_xpm )
EXTERN_BITMAP( left_xpm )
EXTERN_BITMAP( libedit_icon_xpm )
EXTERN_BITMAP( libedit_xpm )
EXTERN_BITMAP( libedit_part_xpm )
EXTERN_BITMAP( lib_next_xpm )
EXTERN_BITMAP( lib_previous_xpm )
EXTERN_BITMAP( library_browse_xpm )
EXTERN_BITMAP( library_update_xpm )
EXTERN_BITMAP( library_xpm )
EXTERN_BITMAP( libview_xpm )
EXTERN_BITMAP( lines90_xpm )
EXTERN_BITMAP( load_module_board_xpm )
EXTERN_BITMAP( load_module_lib_xpm )
EXTERN_BITMAP( local_ratsnest_xpm )
EXTERN_BITMAP( locked_xpm )
EXTERN_BITMAP( macros_record_xpm )
EXTERN_BITMAP( mirepcb_xpm )
EXTERN_BITMAP( mirror_h_xpm )
EXTERN_BITMAP( mirror_v_xpm )
EXTERN_BITMAP( mode_module_xpm )
EXTERN_BITMAP( mode_track_xpm )
EXTERN_BITMAP( mod_ratsnest_xpm )
EXTERN_BITMAP( module_check_xpm )
EXTERN_BITMAP( module_edit_xpm )
EXTERN_BITMAP( module_filtered_list_xpm )
EXTERN_BITMAP( module_pin_filtered_list_xpm )
EXTERN_BITMAP( module_full_list_xpm )
EXTERN_BITMAP( module_options_xpm )
EXTERN_BITMAP( module_ratsnest_xpm )
EXTERN_BITMAP( module_xpm )
EXTERN_BITMAP( module_edit_xpm )
EXTERN_BITMAP( modview_icon_xpm )
EXTERN_BITMAP( morgan1_xpm )
EXTERN_BITMAP( morgan2_xpm )
EXTERN_BITMAP( move_arc_xpm )
EXTERN_BITMAP( move_circle_xpm )
EXTERN_BITMAP( move_field_xpm )
EXTERN_BITMAP( move_glabel_xpm )
EXTERN_BITMAP( move_line_xpm )
EXTERN_BITMAP( move_module_xpm )
EXTERN_BITMAP( move_pad_xpm )
EXTERN_BITMAP( move_pinsheet_xpm )
EXTERN_BITMAP( move_pin_xpm )
EXTERN_BITMAP( move_polygon_xpm )
EXTERN_BITMAP( move_rectangle_xpm )
EXTERN_BITMAP( move_sheet_xpm )
EXTERN_BITMAP( move_text_xpm )
EXTERN_BITMAP( move_track_segment_xpm )
EXTERN_BITMAP( move_track_xpm )
EXTERN_BITMAP( move_xpm )
EXTERN_BITMAP( mw_add_gap_xpm )
EXTERN_BITMAP( mw_add_line_xpm )
EXTERN_BITMAP( mw_add_shape_xpm )
EXTERN_BITMAP( mw_add_stub_arc_xpm )
EXTERN_BITMAP( mw_add_stub_xpm )
EXTERN_BITMAP( mw_toolbar_xpm )
EXTERN_BITMAP( net_highlight_xpm )
EXTERN_BITMAP( netlist_xpm )
EXTERN_BITMAP( net_locked_xpm )
EXTERN_BITMAP( net_unlocked_xpm )
EXTERN_BITMAP( new_component_xpm )
EXTERN_BITMAP( new_cvpcb_xpm )
EXTERN_BITMAP( new_footprint_xpm )
EXTERN_BITMAP( new_gerb_xpm )
EXTERN_BITMAP( new_library_xpm )
EXTERN_BITMAP( new_pcb_xpm )
EXTERN_BITMAP( new_project_xpm )
EXTERN_BITMAP( new_python_xpm )
EXTERN_BITMAP( new_sch_xpm )
EXTERN_BITMAP( new_txt_xpm )
EXTERN_BITMAP( new_xpm )
EXTERN_BITMAP( noconn_xpm )
EXTERN_BITMAP( normal_xpm )
EXTERN_BITMAP( online_help_xpm )
EXTERN_BITMAP( open_brd_file_xpm )
EXTERN_BITMAP( open_document_xpm )
EXTERN_BITMAP( open_library_xpm )
EXTERN_BITMAP( open_project_xpm )
EXTERN_BITMAP( options_all_tracks_and_vias_xpm )
EXTERN_BITMAP( options_all_tracks_xpm )
EXTERN_BITMAP( options_all_vias_xpm )
EXTERN_BITMAP( options_arc_xpm )
EXTERN_BITMAP( options_circle_xpm )
EXTERN_BITMAP( options_module_xpm )
EXTERN_BITMAP( options_new_pad_xpm )
EXTERN_BITMAP( options_pad_xpm )
EXTERN_BITMAP( options_pinsheet_xpm )
EXTERN_BITMAP( options_pin_xpm )
EXTERN_BITMAP( options_rectangle_xpm )
EXTERN_BITMAP( options_segment_xpm )
EXTERN_BITMAP( options_text_xpm )
EXTERN_BITMAP( options_tracks_xpm )
EXTERN_BITMAP( options_track_xpm )
EXTERN_BITMAP( options_vias_xpm )
EXTERN_BITMAP( opt_show_polygon_xpm )
EXTERN_BITMAP( orient_xpm )
EXTERN_BITMAP( pad_sketch_xpm )
EXTERN_BITMAP( pad_xpm )
EXTERN_BITMAP( pads_mask_layers_xpm )
EXTERN_BITMAP( palette_xpm )
EXTERN_BITMAP( part_properties_xpm )
EXTERN_BITMAP( paste_xpm )
EXTERN_BITMAP( pcbnew_xpm )
EXTERN_BITMAP( pcb_offset_xpm )
EXTERN_BITMAP( pin2pin_xpm )
EXTERN_BITMAP( pin_name_to_xpm )
EXTERN_BITMAP( pin_number_to_xpm )
EXTERN_BITMAP( pin_size_to_xpm )
EXTERN_BITMAP( pinorient_right_xpm )
EXTERN_BITMAP( pinorient_left_xpm )
EXTERN_BITMAP( pinorient_up_xpm )
EXTERN_BITMAP( pinorient_down_xpm )
EXTERN_BITMAP( pinshape_nonlogic_xpm )
EXTERN_BITMAP( pinshape_normal_xpm )
EXTERN_BITMAP( pinshape_invert_xpm )
EXTERN_BITMAP( pinshape_clock_fall_xpm )
EXTERN_BITMAP( pinshape_clock_normal_xpm )
EXTERN_BITMAP( pinshape_clock_invert_xpm )
EXTERN_BITMAP( pinshape_active_low_input_xpm )
EXTERN_BITMAP( pinshape_clock_active_low_xpm )
EXTERN_BITMAP( pinshape_active_low_output_xpm )
EXTERN_BITMAP( pintype_input_xpm )
EXTERN_BITMAP( pintype_output_xpm )
EXTERN_BITMAP( pintype_bidi_xpm )
EXTERN_BITMAP( pintype_3states_xpm )
EXTERN_BITMAP( pintype_passive_xpm )
EXTERN_BITMAP( pintype_notspecif_xpm )
EXTERN_BITMAP( pintype_powerinput_xpm )
EXTERN_BITMAP( pintype_poweroutput_xpm )
EXTERN_BITMAP( pintype_opencoll_xpm )
EXTERN_BITMAP( pintype_openemit_xpm )
EXTERN_BITMAP( pintype_noconnect_xpm )
EXTERN_BITMAP( pin_to_xpm )
EXTERN_BITMAP( pin_xpm )
EXTERN_BITMAP( plot_hpg_xpm )
EXTERN_BITMAP( plot_ps_xpm )
EXTERN_BITMAP( plot_xpm )
EXTERN_BITMAP( polar_coord_xpm )
EXTERN_BITMAP( post_compo_xpm )
EXTERN_BITMAP( post_drill_xpm )
EXTERN_BITMAP( post_module_xpm )
EXTERN_BITMAP( preference_xpm )
EXTERN_BITMAP( print_button_xpm )
EXTERN_BITMAP( ratsnest_xpm )
EXTERN_BITMAP( read_setup_xpm )
EXTERN_BITMAP( redo_xpm )
EXTERN_BITMAP( red_xpm )
EXTERN_BITMAP( reload2_xpm )
EXTERN_BITMAP( reload_xpm )
EXTERN_BITMAP( repaint_xpm )
EXTERN_BITMAP( reset_text_xpm )
EXTERN_BITMAP( resize_sheet_xpm )
EXTERN_BITMAP( right_xpm )
EXTERN_BITMAP( rotate_field_xpm )
EXTERN_BITMAP( rotate_glabel_xpm )
EXTERN_BITMAP( rotate_module_neg_xpm )
EXTERN_BITMAP( rotate_module_pos_xpm )
EXTERN_BITMAP( rotate_pin_xpm )
EXTERN_BITMAP( rotate_cw_xpm )
EXTERN_BITMAP( rotate_ccw_xpm )
EXTERN_BITMAP( rotate_neg_x_xpm )
EXTERN_BITMAP( rotate_pos_x_xpm )
EXTERN_BITMAP( rotate_neg_y_xpm )
EXTERN_BITMAP( rotate_pos_y_xpm )
EXTERN_BITMAP( rotate_neg_z_xpm )
EXTERN_BITMAP( rotate_pos_z_xpm )
EXTERN_BITMAP( save_as_xpm )
EXTERN_BITMAP( save_library_xpm )
EXTERN_BITMAP( save_netlist_xpm )
EXTERN_BITMAP( save_part_in_mem_xpm )
EXTERN_BITMAP( save_project_xpm )
EXTERN_BITMAP( save_setup_xpm )
EXTERN_BITMAP( save_xpm )
EXTERN_BITMAP( schematic_xpm )
EXTERN_BITMAP( select_grid_xpm )
EXTERN_BITMAP( select_layer_pair_xpm )
EXTERN_BITMAP( select_w_layer_xpm )
EXTERN_BITMAP( sheetset_xpm )
EXTERN_BITMAP( show_dcodenumber_xpm )
EXTERN_BITMAP( show_footprint_xpm )
EXTERN_BITMAP( show_mod_edge_xpm )
EXTERN_BITMAP( showtrack_xpm )
EXTERN_BITMAP( show_zone_xpm )
EXTERN_BITMAP( show_zone_disable_xpm )
EXTERN_BITMAP( show_zone_outline_only_xpm )
EXTERN_BITMAP( swap_layer_xpm )
EXTERN_BITMAP( text_sketch_xpm )
EXTERN_BITMAP( three_d_xpm )
EXTERN_BITMAP( tool_ratsnest_xpm )
EXTERN_BITMAP( tools_xpm )
EXTERN_BITMAP( track_locked_xpm )
EXTERN_BITMAP( track_sketch_xpm )
EXTERN_BITMAP( track_unlocked_xpm )
EXTERN_BITMAP( transistor_xpm )
EXTERN_BITMAP( tree_nosel_xpm )
EXTERN_BITMAP( tree_sel_xpm )
EXTERN_BITMAP( undelete_xpm )
EXTERN_BITMAP( undo_xpm )
EXTERN_BITMAP( unit_inch_xpm )
EXTERN_BITMAP( unit_mm_xpm )
EXTERN_BITMAP( unknown_xpm )
EXTERN_BITMAP( unlocked_xpm )
EXTERN_BITMAP( unzip_xpm )
EXTERN_BITMAP( update_module_board_xpm )
EXTERN_BITMAP( up_xpm )
EXTERN_BITMAP( via_xpm )
EXTERN_BITMAP( via_sketch_xpm )
EXTERN_BITMAP( viewlibs_icon_xpm )
EXTERN_BITMAP( warning_xpm )
EXTERN_BITMAP( web_support_xpm )
EXTERN_BITMAP( width_net_xpm )
EXTERN_BITMAP( width_segment_xpm )
EXTERN_BITMAP( width_track_via_xpm )
EXTERN_BITMAP( width_track_xpm )
EXTERN_BITMAP( width_vias_xpm )
EXTERN_BITMAP( window_close_xpm )
EXTERN_BITMAP( zip_tool_xpm )
EXTERN_BITMAP( zip_xpm )
EXTERN_BITMAP( zone_unfill_xpm )
EXTERN_BITMAP( zoom_area_xpm )
EXTERN_BITMAP( zoom_center_on_screen_xpm )
EXTERN_BITMAP( zoom_redraw_xpm )
EXTERN_BITMAP( zoom_fit_in_page_xpm )
EXTERN_BITMAP( zoom_in_xpm )
EXTERN_BITMAP( zoom_out_xpm )
EXTERN_BITMAP( zoom_page_xpm )
EXTERN_BITMAP( zoom_selection_xpm )
EXTERN_BITMAP( zoom_xpm )

#endif  // BITMAPS_H_
