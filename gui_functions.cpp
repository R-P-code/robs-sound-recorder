#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "glob_vars.h"
#include "recorder_functions.h"
#include "gui_functions.h"
#include <exception>






//GTK object pointers

GtkBuilder* builder; //Opens the build file which contains information on how the gui window is layed out 
GObject* window; //Pointer to window 
GObject* button;//Pointer reused for various buttons
GObject* tps; //Pointer to track position slider
GObject* pos_label; //Pointer to label that tells the current position in minutes and seconds
GObject* end_label; //Pointer to end of track label
GObject* state_label; //Pointer to state label, example: recording, playing, stopped etc...
GObject* track_name_label; //Pointer to name of track label
GObject* open_save_file_dialog; //Pointer to open and save file dialog box
GObject* dialog_button; //Pointer to open and save buttons
GObject* about_dialog; //Pointer to about dialog
GError* error = NULL; //Pointer to deal with gtk runtime errors








bool track_change = false;








void load_GLADE_build_file() {

	builder = gtk_builder_new();

	if (gtk_builder_add_from_file (builder, "builder.ui", &error) == 0 ) {

			throw std::runtime_error("Error loading build file");

		}

}




















void set_up_window() {

	load_GLADE_build_file();

	window = gtk_builder_get_object (builder, "MainWindow");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	if (!window) throw std::runtime_error("Failed to load window");

	set_up_player_buttons();

	set_up_track_labels();

	set_up_open_and_save_dialog();

	set_up_about_dialog();

	set_up_track_pos_slider();
	

}















void set_up_player_buttons() {
	
	GtkSettings *default_settings = gtk_settings_get_default();
	g_object_set(default_settings, "gtk-button-images", TRUE, NULL);

	button = gtk_builder_get_object (builder, "PlayButton");
	g_signal_connect (button, "clicked", G_CALLBACK (play), NULL);

	button = gtk_builder_get_object (builder, "StopButton");
	g_signal_connect (button, "clicked", G_CALLBACK (stop), NULL);

	button = gtk_builder_get_object (builder, "RecordButton");
	g_signal_connect (button, "clicked", G_CALLBACK (record), NULL);

	button = gtk_builder_get_object (builder, "PauseButton");
	g_signal_connect (button, "clicked", G_CALLBACK (pause_audio), NULL);

}


















void set_up_track_labels(){

	state_label = gtk_builder_get_object (builder, "StateLabel");

	pos_label = gtk_builder_get_object (builder, "PositionLabel");

	end_label = gtk_builder_get_object (builder, "EndLabel");

	track_name_label = gtk_builder_get_object (builder, "TrackNameLabel");

}

















void set_dialog_box() {

	open_save_file_dialog = gtk_builder_get_object (builder, "FileOpener");
	g_signal_connect (open_save_file_dialog, "file-activated", G_CALLBACK(push_open_response), NULL);

	dialog_button = gtk_builder_get_object (builder, "DialogOpenSave");
	g_signal_connect_swapped (dialog_button, "clicked", G_CALLBACK(push_open_response), open_save_file_dialog);

}

















void set_new_open_save_buttons() {

	button = gtk_builder_get_object (builder, "NewButton");
	g_signal_connect_swapped (button, "activate", G_CALLBACK(make_new_file), track_name_label);

	button = gtk_builder_get_object (builder, "OpenButton");
	g_signal_connect_swapped (button, "activate", G_CALLBACK(make_dialog_open), open_save_file_dialog);
	g_signal_connect_swapped (button, "activate", G_CALLBACK(label_to_open), dialog_button);
	g_signal_connect_swapped (button, "activate", G_CALLBACK(run_file_dialog), open_save_file_dialog);

	button = gtk_builder_get_object (builder, "SaveasButton");
	g_signal_connect_swapped (button, "activate", G_CALLBACK(label_to_save), dialog_button);
	g_signal_connect_swapped (button, "activate", G_CALLBACK(make_dialog_save), open_save_file_dialog);
	g_signal_connect_swapped (button, "activate", G_CALLBACK (run_file_dialog), open_save_file_dialog);

	button = gtk_builder_get_object (builder, "SaveButton");
	g_signal_connect_swapped (button, "activate", G_CALLBACK(label_to_save), dialog_button);
	g_signal_connect_swapped (button, "activate", G_CALLBACK(make_dialog_save), open_save_file_dialog);
	g_signal_connect_swapped (button, "activate", G_CALLBACK (save_wav), open_save_file_dialog);

}


















void set_text_entry_button() {

	button = gtk_builder_get_object (builder, "DialogEntry");//Text entry
	g_signal_connect (button, "activate", G_CALLBACK(push_entry_response), open_save_file_dialog);
	g_signal_connect (open_save_file_dialog, "selection-changed", G_CALLBACK(set_entry_label), button);

}

















void	set_cancel_dialog_button(){

	button = gtk_builder_get_object (builder, "DialogCancel");
	g_signal_connect_swapped (button, "clicked", G_CALLBACK(push_cancel_response), open_save_file_dialog);

}
















void set_up_open_and_save_dialog() {

	set_dialog_box();

	set_new_open_save_buttons();

	set_text_entry_button();

	set_cancel_dialog_button();

}


















void set_up_about_dialog() {

	about_dialog = gtk_builder_get_object (builder, "AboutDialog");

	button = gtk_builder_get_object (builder, "AboutButton");
	g_signal_connect_swapped (button, "activate", G_CALLBACK(run_about_dialog), about_dialog);

	button = gtk_builder_get_object (builder, "CloseAboutButton");
	g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_hide), about_dialog);

}


















void set_up_track_pos_slider() {

	tps = gtk_builder_get_object (builder, "TrackPositionSlider");
	g_signal_connect (tps, "value_changed", G_CALLBACK (change_track_position), tps);

}

















void add_repeating_functions_to_main_loop() {

	g_idle_add(GSourceFunc(process_audio), NULL );

	g_idle_add(GSourceFunc( update_state_label ), state_label );

	g_idle_add(GSourceFunc( update_track_position ), tps );

	g_idle_add(GSourceFunc( update_pos_label ), pos_label );

	g_idle_add(GSourceFunc( update_end_label ), end_label );

	g_idle_add(GSourceFunc( set_track_name_label ), track_name_label);

}


















void start_main_loop() {

	if (!init_SDL_audio()) throw std::runtime_error("Failed to initialize SDL");
	else gtk_main();
	
}
















int change_track_position(GObject* tps) {


	if (length > 0) {
		

		int temp_1 = gtk_range_get_value (GTK_RANGE(tps)); 

		int temp_2 = temp_1 / length;

		int temp_3 = temp_2 * length;


		if (temp_3 > get_end_position()) temp_3 = get_end_position();

		set_track_position(temp_3);


	}


	return 1;


};
















int update_track_position(GObject* tps) {


	gtk_range_set_range (GTK_RANGE(tps), 0.0, get_end_position());

	gtk_range_set_value (GTK_RANGE(tps), get_track_position());

	return 1;


}

















int update_state_label(GObject* state_label) {


	if (current_state == STOPPED) gtk_label_set_text (GTK_LABEL(state_label), "Stopped.");

	else if (current_state == PLAYING) gtk_label_set_text (GTK_LABEL(state_label), "Playing...");

	else if (current_state == RECORDING) gtk_label_set_text (GTK_LABEL(state_label), "Recording...");

	else if (current_state == PAUSED) gtk_label_set_text (GTK_LABEL(state_label), "Paused.");

	return 1;


}
















int update_pos_label(GObject* pos_label) {


	double curr_time_min = calculate_pos_minutes();

	double curr_time_sec = calculate_pos_seconds();


	std::ostringstream ss;


	if (curr_time_sec < 10) ss << "0" << curr_time_min << ":" << "0" << curr_time_sec;

	else ss << "0" << curr_time_min << ":" << curr_time_sec;



	gtk_label_set_text (GTK_LABEL(pos_label), ss.str().c_str());



	return 1;


}














	

int update_end_label(GObject* end_label) {


	double end_pos_min = calculate_end_minutes();

	double end_pos_sec = calculate_end_seconds();


	std::ostringstream ss;


	if (end_pos_sec < 10)	ss << "0" << end_pos_min << ":" << "0" << end_pos_sec;

	else ss << "0" << end_pos_min << ":" << end_pos_sec;


	gtk_label_set_text (GTK_LABEL(end_label), ss.str().c_str());


	return 1;


}


















void make_dialog_open(GObject* f_dialog) {

	gtk_file_chooser_set_action (GTK_FILE_CHOOSER(f_dialog), GTK_FILE_CHOOSER_ACTION_OPEN);

}


















//change dialog button label to open
void label_to_open(GObject* b) {

		gtk_button_set_label (GTK_BUTTON(b), "Open");

}
















void make_dialog_save(GObject* f_dialog) {

	gtk_file_chooser_set_action (GTK_FILE_CHOOSER(f_dialog), GTK_FILE_CHOOSER_ACTION_SAVE);

}

















//change dialog button label to save
void label_to_save(GObject* b) {

	gtk_button_set_label (GTK_BUTTON(b), "Save");

}

















void run_file_dialog(GObject* f_dialog) {

	//Stop audio player
	stop();

	int success = 0;

	int response = gtk_dialog_run(GTK_DIALOG(f_dialog));

	if (response == GTK_RESPONSE_OK) {

		import_export_filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(f_dialog));

		if (gtk_file_chooser_get_action (GTK_FILE_CHOOSER(f_dialog)) == GTK_FILE_CHOOSER_ACTION_OPEN ) {
	
			success = read_wav_file();

		}

		else if (gtk_file_chooser_get_action (GTK_FILE_CHOOSER(f_dialog)) == GTK_FILE_CHOOSER_ACTION_SAVE ) {

			success = write_wav_file();

		}

	}

	gtk_widget_hide(GTK_WIDGET(f_dialog));

	if (success) track_change = true;

}
















void push_open_response(GObject* f_dialog) {

	gtk_dialog_response(GTK_DIALOG(f_dialog), GTK_RESPONSE_OK);

}

















void push_cancel_response(GObject* f_dialog) {

	gtk_dialog_response(GTK_DIALOG(f_dialog), GTK_RESPONSE_CANCEL);

}















void push_entry_response(GObject* entry, GObject* f_dialog) {


	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(f_dialog), gtk_entry_get_text(GTK_ENTRY(entry)));

	gtk_entry_set_text (GTK_ENTRY(entry), "");

	gtk_dialog_response(GTK_DIALOG(f_dialog), GTK_RESPONSE_OK);


}
















void set_entry_label(GObject* f_dialog, GObject* entry) {


	if (gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(f_dialog))) {

		gtk_entry_set_text (GTK_ENTRY(entry), gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(f_dialog)));	

	}


}
















int set_track_name_label(GObject* track_label) {

	if (track_change) { 

		if (import_export_filepath == "") gtk_label_set_text (GTK_LABEL(track_label), "New recording");

		else gtk_label_set_text (GTK_LABEL(track_label), import_export_filepath.c_str());

		track_change = false;

	}

	return 1;

}
















void make_new_file(GObject* track_label) {

	reset_track_positions();

	gtk_label_set_text (GTK_LABEL(track_label), "New recording");

	import_export_filepath = "";

}
















void save_wav(GObject* f_dialog){

	if (import_export_filepath == "") run_file_dialog(f_dialog);

	else write_wav_file();

}















void run_about_dialog(GObject* a_dialog){

	int response = gtk_dialog_run(GTK_DIALOG(a_dialog));

	gtk_widget_hide(GTK_WIDGET(a_dialog));


}






















