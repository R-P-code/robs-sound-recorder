


//Function declarations

bool init_SDL_audio();


bool process_audio(); //Main function







void set_track_position(int new_pos);


uint32_t get_track_position();


double calculate_pos_seconds();


uint32_t get_end_position();


void set_end_position(Uint32 new_pos);


double calculate_end_seconds();


double calculate_pos_minutes();


double calculate_end_minutes();






void close_SDL();





void audioRecordingCallback( void* userdata, Uint8* stream, int len );


void audioPlaybackCallback( void* userdata, Uint8* stream, int len );






void set_end_of_recording();






void load_mic();


void load_speakers();




void update_if_recording();


void update_if_playing();





void stop();


void pause_audio();


void record();


void play();








int read_wav_file();


int write_wav_file();


void reset_track_positions();


void set_buffer_variables(uint32_t bytes_per_sec);


bool check_if_wav();


struct imp_wav_hdr;


bool input_file_into_buffer();


struct exp_wav_hdr;


void set_data_size(exp_wav_hdr& wav_hdr);


bool output_recording_into_file();








