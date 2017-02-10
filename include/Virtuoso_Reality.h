#ifndef VIRTUOSO_REALITY_H
#define VIRTUOSO_REALITY_H

#include "Virtuoso_Reality_Util.h"
#include "Windowing_System.h"
#include <ctype.h>
    //-----------------------------------------------------------------------------
    // global variables and #defines
    //-----------------------------------------------------------------------------
#define SAMPLE                  float
#define RTAUDIO_FORMAT          RTAUDIO_FLOAT32
//#define NUM_CHANNELS            2
#define SND_BUFFER_SIZE         512
#define SND_FFT_SIZE            ( SND_BUFFER_SIZE * 2 )
#define SND_MARSYAS_SIZE        ( 512 )
#define INC_VAL_MOUSE           1.0f
#define INC_VAL_KB              .025f

class VirtuosoReality{
    //-----------------------------------------------------------------------------
    // function prototypes
    //-----------------------------------------------------------------------------
public:
    VirtuosoReality(int argc, char ** argv);
    static WindowingSystem * ws;
    static const int NUM_CHANNELS = 2;
    static int cb( const void * inputBuffer, void * outputBuffer,  unsigned long numFrames,
                  const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags statusFlags, void * userData );
    static void reshapeFunc( int width, int height );
    static void keyboardFunc(GLFWwindow * window, int key, int scancode, int action);
    static GLFWcursorposfun mouseFunc(double x, double y );
    static double compute_log_spacing( int fft_size, double factor );
    static double map_log_spacing( double ratio, double power );
    static void doMovement(int key);
    static SAMPLE g_audio_buffer[SND_BUFFER_SIZE]; // latest mono buffer (possibly preview)
    static SAMPLE g_stereo_buffer[SND_BUFFER_SIZE*2]; // current stereo buffer (now
    static GLint g_buffer_size; // = SND_BUFFER_SIZE;
    static int samplesRead;
    void showUI();
    void init(int argc, char ** argv);
    void run();
    void avrLoop();
    
#define MAX_PATH_BYTES 2048
    char musicPath[MAX_PATH_BYTES];
    bool browseButtonPressed = false;
    ImGuiFs::Dialog  * dlg;
    bool show_demo_window = false;
    bool show_fchooser_window = false;
    bool show_help_window = false;
    bool show_about_window = false;
    static bool g_play;// = false;
    ImVec4 clear_color = ImColor(114, 144, 154);
    static bool showui;// = false;
    // real-time audio
    static GLboolean g_ready;// = FALSE;
    static Mutex g_mutex;
    // file reading
    static AudioDecoder * g_sf; // = NULL;
    // the index associated with the waterfall
    static GLint g_wf;// = 0;
    // starting a file
    static GLint g_starting; // = 0;
    // input filename
    static string g_filename;// = NULL;
    // draw analysis features
    static GLboolean g_draw_features;// = TRUE;
    // freeze display
    static GLboolean g_freeze;
    // mute audio
    static GLboolean g_mute;// = FALSE;
    // for waterfall
    struct Pt2D { float x; float y; };
    
    static Pt2D ** g_spectrums;// = NULL;
    static GLuint g_depth;// = 48; // for john: 64
    
    // not sure what these do anymore...
    static GLint g_buffer_count_a;// = 0;
    static GLint g_buffer_count_b;// = 0;
    
    // thing running
    static GLboolean g_running;// = TRUE;
    // file input running
    static GLboolean g_file_running;
    // restart the file
    static GLboolean g_restart;// = TRUE;
    // pause audio playback
    static GLboolean g_pause; // = FALSE;
    // array of booleans for waterfall
    static GLboolean * g_draw ;//= NULL;
    
    static GLfloat g_light0_pos[4];// = { 2.0f, 1.2f, 4.0f, 1.0f };
    static GLfloat g_light1_pos[4];// = { -2.0f, 0.0f, -4.0f, 1.0f };
    
    // for time domain waterfall
    static SAMPLE ** g_waveforms;// = NULL;
    static GLfloat g_wf_delay_ratio;// = 1.0f / 3.0f;
    static GLuint g_wf_index;// = 0;
    static GLint g_wf_delay;
    static GLfloat g_dz;
    static GLfloat g_z ;
    static GLfloat g_space ; // for john: .1f
    static GLint g_ds;
    // waveform
    static GLboolean g_waveform;
    static GLboolean g_wutrfall;
    static GLboolean g_usedb;
    static GLfloat g_time_scale;
    static GLfloat g_freq_scale;
    static GLint g_time_view;
    static GLfloat g_inc_val_kb;
    static GLfloat g_inc_val_mouse;
    
    static GLboolean g_fullscreen;
    static GLboolean g_lissajous;
    static GLfloat g_lissajous_scale;
    static GLint g_delay;
    static GLfloat g_log_factor;
    static GLfloat g_log_space;
    static GLint g_fft_size;
    static GLboolean g_rainbow;
    static GLboolean g_show_time;
    static GLboolean g_backwards;
    static GLfloat g_begintime;
    static GLint g_freq_view;
    
    static int g_position;
    static int b_seek;
    
    
    static void help( );
    
    void probe( );
    void usage( );
    void idleFunc( );
    void draw_string( GLfloat x, GLfloat y, GLfloat z, const char * str, GLfloat scale );
    void draw_Lissajous(SAMPLE * stereobuffer, int len, int channels);
    
    void displayFunc( );
    
    void initialize_graphics( );
    bool initialize_audio( );
    void initialize_analysis( );
    void extract_buffer( );
    
    
    // global audio buffer
    SAMPLE g_fft_buffer[SND_FFT_SIZE];
    SAMPLE g_back_buffer[SND_BUFFER_SIZE]; // for lissajous
    SAMPLE g_cur_buffer[SND_BUFFER_SIZE];  // current mono buffer (now playing), for lissajous
    GLfloat g_window[SND_BUFFER_SIZE]; // DFT transform window
    static GLfloat g_log_positions[SND_FFT_SIZE/2]; // precompute positions for log spacing
    

    SF_INFO g_sf_info;
    
    // default sample rate
    //#if defined(__LINUX_ALSA__) || defined(__LINUX_OSS__) || defined(__LINUX_JACK__)
    //GLuint g_srate = 48000;
    //#else
    GLuint g_srate = 44100; //sample rate
    //#endif
    
    // gain
    GLfloat g_gain = 1.0f;
    
    
    // number of real-time audio channels
    GLint g_sndout = 0;
    GLint g_sndin = 2;

    
    // marsyas analysis modules
    Centroid * g_centroid = NULL;
    DownSampler * g_down_sampler = NULL;
    Flux * g_flux = NULL;
    LPC * g_lpc = NULL;
    MFCC * g_mfcc = NULL;
    RMS * g_rms = NULL;
    Rolloff * g_rolloff = NULL;
    Rolloff * g_rolloff2 = NULL;
    
    // global flags with default...
    // ---
    // audio input device
    long g_audioInputDevice = -1;
    // audio output device
    long g_audioOutputDevice = -1;
    // print features to stdout
    GLboolean g_stdout = FALSE;
    // opengl dislpay
    GLboolean g_display = TRUE;
    
    
    // draw play pointer
    GLboolean g_draw_play = FALSE;
    
    GLboolean g_z_set = FALSE;
    
    GLboolean g_downsample = FALSE;
    
    // fill mode
    GLenum g_fillmode = GL_FILL;
    // light 0 position
    // light 1 parameters
    GLfloat g_light1_ambient[4] = { .2f, .2f, .2f, 1.0f };
    GLfloat g_light1_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat g_light1_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    // modelview stuff
    GLfloat g_angle_y = 0.0f;
    GLfloat g_inc = 0.0f;
    
    static float t;
    

};

#endif
