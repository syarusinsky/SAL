#ifndef AUDIOCONSTANTS_HPP
#define AUDIOCONSTANTS_HPP

/*******************************************************
 * This class contains the audio constants that the
 * target system will be running at.
*******************************************************/

#ifdef TARGET_BUILD
#define SAMPLE_RATE 40000
#else
#define SAMPLE_RATE 96000
#endif
#define SAMPLE_PERIOD (1.0f / static_cast<float>(SAMPLE_RATE))
#define NYQUIST_FREQ (SAMPLE_RATE / 2)
#ifdef TARGET_BUILD
#define ABUFFER_SIZE 256
#else
#define ABUFFER_SIZE 512
#endif

// Musical Frequencies
#define MUSIC_G9  12543.85
#define MUSIC_GB9 11839.82
#define MUSIC_F9  11175.30
#define MUSIC_E9  10548.08
#define MUSIC_EB9 9956.06
#define MUSIC_D9  9397.27
#define MUSIC_DB9 8869.84
#define MUSIC_C9  8372.02
#define MUSIC_B8  7902.13
#define MUSIC_BB8 7458.62
#define MUSIC_A8  7040.00
#define MUSIC_AB8 6644.88
#define MUSIC_G8  6271.93
#define MUSIC_GB8 5919.91
#define MUSIC_F8  5587.65
#define MUSIC_E8  5274.04
#define MUSIC_EB8 4978.03
#define MUSIC_D8  4698.64
#define MUSIC_DB8 4434.92
#define MUSIC_C8  4186.01
#define MUSIC_B7  3951.07
#define MUSIC_BB7 3729.31
#define MUSIC_A7  3520.00
#define MUSIC_AB7 3322.44
#define MUSIC_G7  3135.96
#define MUSIC_GB7 2959.96
#define MUSIC_F7  2793.83
#define MUSIC_E7  2637.02
#define MUSIC_EB7 2489.02
#define MUSIC_D7  2349.32
#define MUSIC_DB7 2217.46
#define MUSIC_C7  2093.00
#define MUSIC_B6  1975.53
#define MUSIC_BB6 1864.66
#define MUSIC_A6  1760.00
#define MUSIC_AB6 1661.22
#define MUSIC_G6  1567.98
#define MUSIC_GB6 1479.98
#define MUSIC_F6  1396.91
#define MUSIC_E6  1318.51
#define MUSIC_EB6 1244.51
#define MUSIC_D6  1174.66
#define MUSIC_DB6 1108.73
#define MUSIC_C6  1046.50
#define MUSIC_B5  987.77
#define MUSIC_BB5 932.33
#define MUSIC_A5  880.00
#define MUSIC_AB5 830.61
#define MUSIC_G5  783.99
#define MUSIC_GB5 739.99
#define MUSIC_F5  698.46
#define MUSIC_E5  659.26
#define MUSIC_EB5 622.25
#define MUSIC_D5  587.33
#define MUSIC_DB5 554.37
#define MUSIC_C5  523.25
#define MUSIC_B4  493.88
#define MUSIC_BB4 466.16
#define MUSIC_A4  440.00
#define MUSIC_AB4 415.30
#define MUSIC_G4  392.00
#define MUSIC_GB4 369.99
#define MUSIC_F4  349.23
#define MUSIC_E4  329.63
#define MUSIC_EB4 311.13
#define MUSIC_D4  293.66
#define MUSIC_DB4 277.18
#define MUSIC_C4  261.63
#define MUSIC_B3  246.94
#define MUSIC_BB3 233.08
#define MUSIC_A3  220.00
#define MUSIC_AB3 207.65
#define MUSIC_G3  196.00
#define MUSIC_GB3 185.00
#define MUSIC_F3  174.61
#define MUSIC_E3  164.81
#define MUSIC_EB3 155.56
#define MUSIC_D3  146.83
#define MUSIC_DB3 138.59
#define MUSIC_C3  130.81
#define MUSIC_B2  123.47
#define MUSIC_BB2 116.54
#define MUSIC_A2  110.00
#define MUSIC_AB2 103.83
#define MUSIC_G2  98.00
#define MUSIC_GB2 92.50
#define MUSIC_F2  87.31
#define MUSIC_E2  82.41
#define MUSIC_EB2 77.78
#define MUSIC_D2  73.42
#define MUSIC_DB2 69.30
#define MUSIC_C2  65.41
#define MUSIC_B1  61.74
#define MUSIC_BB1 58.27
#define MUSIC_A1  55.00
#define MUSIC_AB1 51.91
#define MUSIC_G1  49.00
#define MUSIC_GB1 46.25
#define MUSIC_F1  43.65
#define MUSIC_E1  41.20
#define MUSIC_EB1 38.89
#define MUSIC_D1  36.71
#define MUSIC_DB1 34.65
#define MUSIC_C1  32.70
#define MUSIC_B0  30.87
#define MUSIC_BB0 29.14
#define MUSIC_A0  27.50

#endif // AUDIOCONSTANTS_HPP
