//GUI function declarations


void load_GLADE_build_file();


void set_up_window();


void set_up_player_buttons();


void set_up_track_labels();


void set_dialog_box();


void set_new_open_save_buttons();


void set_text_entry_button();


void  set_cancel_dialog_button();


void set_up_open_and_save_dialog();


void set_up_about_dialog();


void set_up_track_pos_slider();


void add_repeating_functions_to_main_loop();


void start_main_loop();


int change_track_position(GObject* tps);


int update_track_position(GObject* tps);


int update_state_label(GObject* state_label);


int update_pos_label(GObject* pos_label);


int update_end_label(GObject* end_label);


void make_dialog_open(GObject* f_dialog); // make the open / save dialog an open file dialog


void make_dialog_save(GObject* f_dialog); // make the open /save dialog a save file dialog


void label_to_open(GObject* b);


void label_to_save(GObject* b);


void run_file_dialog(GObject* f_dialog);


void push_open_response(GObject* f_dialog);


void push_cancel_response(GObject* f_dialog);


void push_entry_response(GObject* entry, GObject* f_dialog);


void set_entry_label(GObject* f_dialog, GObject* entry);


int set_track_name_label(GObject* track_label);


void make_new_file(GObject* track_label);


void save_wav(GObject* f_dialog);


void run_about_dialog(GObject* a_dialog);







