/******************************************************
 * This file defines MIDI constants that will be used
 * throughout many programs.
******************************************************/


#define MAX_MIDI_MESSAGE_SIZE 8
#define MIDI_BUFFER_SIZE      12
#define MIDI_STATUS_BYTE      0b10000000
#define MIDI_DATA_BYTE        0b00000000
#define MIDI_SYSTEM_COMMON    0b1111

// MIDI Channel Messages

#define MIDI_NOTE_OFF         0b1000
#define MIDI_NOTE_ON          0b1001
#define MIDI_AFTERTOUCH       0b1010
#define MIDI_CONTROL_CHANGE   0b1011
#define MIDI_PROGRAM_CHANGE   0b1100
#define MIDI_AFTERTOUCH_MONO  0b1101
#define MIDI_PITCH_BEND       0b1110


// System Common and Realtime

#define MIDI_SYSTEM_EXCLUSIVE 0b11110000
#define MIDI_TIME_CODE        0b11110001
#define MIDI_SONG_POSITION    0b11110010
#define MIDI_SONG_SELECT      0b11110011
#define MIDI_TUNE_REQUEST     0b11110110
#define MIDI_END_OF_EXCLUSIVE 0b11110111
#define MIDI_TIMING_CLOCK     0b11111000
#define MIDI_START            0b11111010
#define MIDI_CONTINUE         0b11111011
#define MIDI_STOP             0b11111100
#define MIDI_ACTIVE_SENSING   0b11111110
#define MIDI_RESET            0b11111111


// Number of Data Bytes

#define MIDI_NOTE_OFF_NUM_DATA         2
#define MIDI_NOTE_ON_NUM_DATA          2
#define MIDI_AFTERTOUCH_NUM_DATA       2
#define MIDI_CONTROL_CHANGE_NUM_DATA   2
#define MIDI_PROGRAM_CHANGE_NUM_DATA   1
#define MIDI_AFTERTOUCH_MONO_NUM_DATA  1
#define MIDI_PITCH_BEND_NUM_DATA       2
#define MIDI_TIME_CODE_NUM_DATA        1
#define MIDI_SONG_POSITION_NUM_DATA    2
#define MIDI_SONG_SELECT_NUM_DATA      1
#define MIDI_TUNE_REQUEST_NUM_DATA     0
#define MIDI_END_OF_EXCLUSIVE_NUM_DATA 0
#define MIDI_TIMING_CLOCK_NUM_DATA     0
#define MIDI_START_NUM_DATA            0
#define MIDI_CONTINUE_NUM_DATA         0
#define MIDI_STOP_NUM_DATA             0
#define MIDI_ACTIVE_SENSING_NUM_DATA   0
#define MIDI_RESET_NUM_DATA            0


// MIDI Note Byte Values

#define MIDI_NOTE_G9  127
#define MIDI_NOTE_GB9 126
#define MIDI_NOTE_F9  125
#define MIDI_NOTE_E9  124
#define MIDI_NOTE_EB9 123
#define MIDI_NOTE_D9  122
#define MIDI_NOTE_DB9 121
#define MIDI_NOTE_C9  120
#define MIDI_NOTE_B8  119
#define MIDI_NOTE_BB8 118
#define MIDI_NOTE_A8  117
#define MIDI_NOTE_AB8 116
#define MIDI_NOTE_G8  115
#define MIDI_NOTE_GB8 114
#define MIDI_NOTE_F8  113
#define MIDI_NOTE_E8  112
#define MIDI_NOTE_EB8 111
#define MIDI_NOTE_D8  110
#define MIDI_NOTE_DB8 109
#define MIDI_NOTE_C8  108
#define MIDI_NOTE_B7  107
#define MIDI_NOTE_BB7 106
#define MIDI_NOTE_A7  105
#define MIDI_NOTE_AB7 104
#define MIDI_NOTE_G7  103
#define MIDI_NOTE_GB7 102
#define MIDI_NOTE_F7  101
#define MIDI_NOTE_E7  100
#define MIDI_NOTE_EB7 99
#define MIDI_NOTE_D7  98
#define MIDI_NOTE_DB7 97
#define MIDI_NOTE_C7  96
#define MIDI_NOTE_B6  95
#define MIDI_NOTE_BB6 94
#define MIDI_NOTE_A6  93
#define MIDI_NOTE_AB6 92
#define MIDI_NOTE_G6  91
#define MIDI_NOTE_GB6 90
#define MIDI_NOTE_F6  89
#define MIDI_NOTE_E6  88
#define MIDI_NOTE_EB6 87
#define MIDI_NOTE_D6  86
#define MIDI_NOTE_DB6 85
#define MIDI_NOTE_C6  84
#define MIDI_NOTE_B5  83
#define MIDI_NOTE_BB5 82
#define MIDI_NOTE_A5  81
#define MIDI_NOTE_AB5 80
#define MIDI_NOTE_G5  79
#define MIDI_NOTE_GB5 78
#define MIDI_NOTE_F5  77
#define MIDI_NOTE_E5  76
#define MIDI_NOTE_EB5 75
#define MIDI_NOTE_D5  74
#define MIDI_NOTE_DB5 73
#define MIDI_NOTE_C5  72
#define MIDI_NOTE_B4  71
#define MIDI_NOTE_BB4 70
#define MIDI_NOTE_A4  69
#define MIDI_NOTE_AB4 68
#define MIDI_NOTE_G4  67
#define MIDI_NOTE_GB4 66
#define MIDI_NOTE_F4  65
#define MIDI_NOTE_E4  64
#define MIDI_NOTE_EB4 63
#define MIDI_NOTE_D4  62
#define MIDI_NOTE_DB4 61
#define MIDI_NOTE_C4  60
#define MIDI_NOTE_B3  59
#define MIDI_NOTE_BB3 58
#define MIDI_NOTE_A3  57
#define MIDI_NOTE_AB3 56
#define MIDI_NOTE_G3  55
#define MIDI_NOTE_GB3 54
#define MIDI_NOTE_F3  53
#define MIDI_NOTE_E3  52
#define MIDI_NOTE_EB3 51
#define MIDI_NOTE_D3  50
#define MIDI_NOTE_DB3 49
#define MIDI_NOTE_C3  48
#define MIDI_NOTE_B2  47
#define MIDI_NOTE_BB2 46
#define MIDI_NOTE_A2  45
#define MIDI_NOTE_AB2 44
#define MIDI_NOTE_G2  43
#define MIDI_NOTE_GB2 42
#define MIDI_NOTE_F2  41
#define MIDI_NOTE_E2  40
#define MIDI_NOTE_EB2 39
#define MIDI_NOTE_D2  38
#define MIDI_NOTE_DB2 37
#define MIDI_NOTE_C2  36
#define MIDI_NOTE_B1  35
#define MIDI_NOTE_BB1 34
#define MIDI_NOTE_A1  33
#define MIDI_NOTE_AB1 32
#define MIDI_NOTE_G1  31
#define MIDI_NOTE_GB1 30
#define MIDI_NOTE_F1  29
#define MIDI_NOTE_E1  28
#define MIDI_NOTE_EB1 27
#define MIDI_NOTE_D1  26
#define MIDI_NOTE_DB1 25
#define MIDI_NOTE_C1  24
#define MIDI_NOTE_B0  23
#define MIDI_NOTE_BB0 22
#define MIDI_NOTE_A0  21
