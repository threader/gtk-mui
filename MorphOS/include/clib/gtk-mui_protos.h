#ifndef LIBRARIES_GTKMUIPPC_PROTOS_H
#define LIBRARIES_GTKMUIPPC_PROTOS_H

void *mgtk_getglobaldata(void);

/* gtk functions */
void       gtk_init(int *argc, char ***argv);
GtkWidget* gtk_window_new   (GtkWindowType   type);
void       gtk_window_set_title   (GtkWindow *window,  const gchar *title);
void       gtk_window_set_resizable        (GtkWindow *window,
                                             gboolean resizable);

gboolean   gtk_window_get_resizable(GtkWindow *window);
GtkWidget* gtk_button_new (void);
GtkWidget* gtk_button_new_with_label    (const char *label);
GtkWidget* gtk_button_new_from_stock       (const gchar *stock_id);
void       gtk_widget_show  (GtkWidget      *widget);
void       gtk_widget_hide  (GtkWidget *widget);
void       gtk_widget_show_all(GtkWidget *widget);
void       gtk_widget_hide_all(GtkWidget *widget);
void       gtk_widget_grab_default (GtkWidget *widget);
void       gtk_widget_set_sensitive(GtkWidget *widget, gboolean sensitive);
gint       gtk_widget_get_events           (GtkWidget *widget);
GdkVisual* gtk_widget_get_visual           (GtkWidget *widget);
GdkColormap* gtk_widget_get_colormap        (GtkWidget *widget);
GType      gtk_widget_get_type(void);
void       gtk_widget_queue_draw(GtkWidget *widget);
void       gtk_widget_style_get            (GtkWidget *widget,
                                             const gchar *first_property_name,
                                            ...);
void       gtk_widget_set_rc_style(GtkWidget *widget);
void       gtk_widget_set_style            (GtkWidget *widget,
                                             GtkStyle *style);
void       gtk_widget_realize              (GtkWidget *widget);
gpointer   gtk_type_class                  (GtkType type);
#define    GTK_TYPE_WIDGET (gtk_widget_get_type ())
void       gtk_grab_add                    (GtkWidget *widget);
void       gtk_grab_remove                 (GtkWidget *widget);
GtkWidget* gtk_grab_get_current            (void);
void       gtk_main         (void);
void       gtk_main_quit    (void);
void       gtk_widget_destroy( GtkWidget *widget/*, GtkWidget *param*/ );
void       gtk_widget_unref(GtkWidget *widget);
GtkWidget *gtk_widget_ref(GtkWidget *widget);
void       gtk_widget_set_size_request    (GtkWidget           *widget,
                                           gint                 width,
                                            gint                 height);
void       gtk_widget_set_usize           (GtkWidget *widget,
                                            gint width,
                                            gint height);
void       gtk_widget_set_state            (GtkWidget *widget,
                                             GtkStateType state);
void       gtk_container_add(GtkContainer  *container, GtkWidget *widget);
void       gtk_container_set_border_width (GtkContainer *container, guint border_width); 
GtkWidget *gtk_hbox_new     (gint homogeneous, gint spacing );
GtkWidget *gtk_vbox_new     (gint homogeneous, gint spacing );
GType      gtk_vbox_get_type (void);
void       gtk_box_pack_start( GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding );
void       gtk_box_pack_end (GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding );

GtkWidget *gtk_menu_new      (void);
GtkWidget *gtk_menu_bar_new      (void);
GtkWidget *gtk_option_menu_new             (void);
GtkWidget *gtk_menu_item_new_with_label (const char *label );
GtkWidget *gtk_menu_item_new ();
void       gtk_menu_shell_append        (GtkMenuShell *menu_shell, GtkWidget *child);
void       gtk_menu_item_set_submenu    (GtkMenuItem         *menu_item,
                                         GtkWidget           *submenu);
GtkWidget *gtk_menu_get_attach_widget   (GtkMenu *menu);
void       gtk_option_menu_set_menu(GtkOptionMenu *option_menu, GtkWidget *menu);


GtkWidget *gtk_table_new(guint rows, guint columns, gboolean homogeneous);
void       gtk_table_attach_defaults  (GtkTable        *table,
                                       GtkWidget       *widget,
				       guint            left_attach,
				       guint            right_attach,
				       guint            top_attach,
				       guint            bottom_attach);
void      gtk_table_attach( GtkTable         *table,
				      GtkWidget        *child,
				      guint            left_attach,
				      guint            right_attach,
				      guint            top_attach,
				      guint            bottom_attach,
				      GtkAttachOptions xoptions,
				      GtkAttachOptions yoptions,
				      guint            xpadding,
				      guint            ypadding );
void       gtk_table_set_row_spacings      (GtkTable *table,
                                              guint spacing);
void       gtk_table_set_col_spacings      (GtkTable *table,
                                              guint spacing);
GtkWidget *gtk_entry_new( void );
void       gtk_entry_set_text( GtkEntry    *entry,
                                              const gchar *text );
const gchar *gtk_entry_get_text( GtkEntry *entry );
void gtk_entry_set_visibility( GtkEntry *entry,
                                              gboolean  visible );
void        gtk_entry_set_max_length        (GtkEntry *entry,
                                              gint max);
void gtk_editable_select_region( GtkEditable *entry,
                                              gint         start,
                                              gint         end );
void        gtk_editable_insert_text        (GtkEditable *editable,
                                              const gchar *new_text,
                                              gint new_text_length,
                                              gint *position);

GtkWidget *gtk_toggle_button_new( void );
GtkWidget *gtk_toggle_button_new_with_label( const gchar *label );
GtkWidget *gtk_toggle_button_new_with_mnemonic( const gchar *label );
gboolean   gtk_toggle_button_get_active	(GtkToggleButton *toggle_button);
void       gtk_toggle_button_set_active( GtkToggleButton *toggle_button,
                                         gboolean        is_active );

GtkWidget* gtk_check_button_new               (void);
GtkWidget* gtk_check_button_new_with_label    (const gchar *label);
GtkWidget* gtk_check_button_new_with_mnemonic (const gchar *label);
void       gtk_editable_set_editable         (GtkEditable *editable,
                                            gboolean     is_editable);
gboolean    gtk_editable_get_editable         (GtkEditable *editable);
gchar*      gtk_editable_get_chars (GtkEditable *editable,
																		gint start_pos,
																		gint end_pos);

GtkWidget*  gtk_image_new                   (void);
GtkWidget*  gtk_image_new_from_file         (const gchar *filename);
GSList     *gtk_radio_button_group(GtkWidget *widget);
GSList     *gtk_radio_button_get_group(GtkWidget *widget);
GtkWidget  *gtk_radio_button_new_with_label_from_widget( GtkRadioButton *group, const gchar    *label );
GtkWidget  *gtk_radio_button_new_with_label( GSList *group, const gchar  *label );
GtkWidget  *gtk_arrow_new(GtkArrowType arrow_type, GtkShadowType shadow_type);
GtkWidget  *gtk_hseparator_new(void);
GtkWidget  *gtk_hscrollbar_new(GtkAdjustment *adjustment);
GtkWidget  *gtk_hscale_new( GtkAdjustment *adjustment );
GtkWidget  *gtk_vscale_new( GtkAdjustment *adjustment );
void        gtk_scale_set_digits            (GtkScale *scale,
                                             gint digits);
void        gtk_scale_set_value_pos(GtkScale *scale, GtkPositionType pos);
GtkAdjustment *gtk_adjustment_new(gdouble value, 
																	gdouble lower, 
																	gdouble upper, 
																	gdouble step_increment, 
																	gdouble page_increment, 
																	gdouble page_size );
void        gtk_adjustment_set_value(GtkAdjustment *adjustment, gdouble value);
void        gtk_adjustment_changed          (GtkAdjustment *adjustment);
void        gtk_range_set_update_policy     (GtkRange *range, GtkUpdateType policy);
GtkTooltips* gtk_tooltips_new(void);
void        gtk_tooltips_set_tip(GtkTooltips *tooltips, 
																	GtkWidget *widget, 
																	const gchar *tip_text, 
																	const gchar *tip_private);
void        gtk_tooltips_disable(GtkTooltips *tooltips);
void        gtk_tooltips_enable(GtkTooltips *tooltips);
void        gtk_tooltips_force_window(GtkTooltips *tooltips);
GtkWidget*  gtk_file_selection_new          (const gchar *title);
void        gtk_file_selection_set_filename (GtkFileSelection *filesel,
                                             const gchar *filename);
const gchar* gtk_file_selection_get_filename (GtkFileSelection *filesel);

GtkWidget*  gtk_frame_new                   (const gchar *label);
void        gtk_frame_set_label             (GtkFrame *frame,
                                             const gchar *label);
void        gtk_frame_set_label_align       (GtkFrame *frame,
                                             gfloat xalign,
																						 gfloat yalign);
void        gtk_frame_set_shadow_type       (GtkFrame *frame,
                                             GtkShadowType type);
GtkWidget*  gtk_spin_button_new             (GtkAdjustment *adjustment,
                                              gdouble climb_rate,
                                              guint digits);
gint        gtk_spin_button_get_value_as_int
                                            (GtkSpinButton *spin_button);
void        gtk_spin_button_set_digits      (GtkSpinButton *spin_button,
                                              guint digits);
gdouble     gtk_spin_button_get_value       (GtkSpinButton *spin_button);
gdouble     gtk_spin_button_get_value_as_float(GtkSpinButton *spin_button);
void        gtk_spin_button_set_wrap        (GtkSpinButton *spin_button,
                                             gboolean wrap);
void        gtk_spin_button_set_numeric     (GtkSpinButton *spin_button,
                                             gboolean numeric);
GtkWidget*  gtk_label_new                   (const char *str);
void        gtk_label_set_text              (GtkLabel *label,
                                             const char *str);
GtkWidget*  gtk_notebook_new                (void);
gint        gtk_notebook_append_page        (GtkNotebook *notebook,
                                             GtkWidget *child,
                                             GtkWidget *tab_label);
gint        gtk_notebook_prepend_page       (GtkNotebook *notebook,
                                             GtkWidget *child,
                                             GtkWidget *tab_label);
void        gtk_notebook_next_page          (GtkNotebook *notebook);
void        gtk_notebook_prev_page          (GtkNotebook *notebook);
GtkWidget*  gtk_progress_bar_new            (void);
const gchar* gtk_progress_bar_get_text      (GtkProgressBar *pbar);
void        gtk_progress_bar_set_text       (GtkProgressBar *pbar,
                                             const gchar *text);
gdouble     gtk_progress_bar_get_fraction   (GtkProgressBar *pbar);
GtkProgressBarOrientation gtk_progress_bar_get_orientation
                                            (GtkProgressBar *pbar);
void        gtk_progress_bar_pulse          (GtkProgressBar *pbar);
void        gtk_progress_bar_set_fraction   (GtkProgressBar *pbar,
                                             gdouble fraction);
void        gtk_progress_bar_set_orientation
                                            (GtkProgressBar *pbar,
                                             GtkProgressBarOrientation orientation);

guint       gtk_timeout_add                 (guint32 interval, GtkFunction function, gpointer data);
void        gtk_timeout_remove              (guint timeout_handler_id);
GtkWidget*  gtk_fixed_new                   (void);
void        gtk_fixed_put                   (GtkFixed *fixed,
                                             GtkWidget *widget,
                                              gint x,
                                              gint y);
void        gtk_fixed_move                  (GtkFixed *fixed,
                                             GtkWidget *widget,
                                              gint x,
                                              gint y);

GtkStyle*   gtk_style_new                   (void);
GtkStyle*   gtk_style_attach                (GtkStyle *style,
                                             GdkWindow *window);
void        gtk_style_set_background        (GtkStyle *style,
                                             GdkWindow *window,
                                              GtkStateType state_type);
GtkStyle*   gtk_rc_get_style_by_paths       (GtkSettings *settings,
                                              const char *widget_path,
                                              const char *class_path,
                                              GType type);
void        gtk_paint_polygon               (GtkStyle *style,
                                              GdkWindow *window,
                                              GtkStateType state_type,
                                              GtkShadowType shadow_type,
                                              GdkRectangle *area,
                                              GtkWidget *widget,
                                              const gchar *detail,
                                              GdkPoint *points,
                                              gint npoints,
                                              gboolean fill);
void        gtk_paint_box                   (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_arrow                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             GtkArrowType arrow_type,
                                             gboolean fill,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_shadow                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_check                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_extension             (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height,
                                             GtkPositionType gap_side);
void        gtk_paint_flat_box              (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_focus                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_option                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_slider                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height,
                                             GtkOrientation orientation);
void        gtk_paint_vline                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint y1_,
                                             gint y2_,
                                             gint x);
void        gtk_paint_hline                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x1,
                                             gint x2,
                                             gint y);
GtkWidget*  gtk_handle_box_new              (void);

#endif /* LIBRARIES_GTKMUIPPC_H */
