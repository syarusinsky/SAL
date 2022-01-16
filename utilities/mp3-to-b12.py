# A simple script to turn an mp3 file into a b12 compressed audio file
# to use, run "python mp3-to-b12.py -FILENAME_WITH_EXTENSION -WRITE_TO_MONO"

import sys
import os
import librosa
import math

def b12_compression (samples) :
    # buffer to hold bytes
    data_to_write = bytearray()
    data_to_write.append( 0x00 )

    # convert each float sample into a 12-bit value and pack the data
    compressed_buf_index = 0
    bits_left_in_compress_buf_byte = 8
    for sample_val in samples :
        sample_val = math.floor( ((sample_val / 2) + 0.5) * 4095 )

        bits_left_in_value_to_write = 12
        value_to_write = sample_val

        while bits_left_in_value_to_write > 0 :
            nibble = value_to_write & 0xF
            value_to_write = value_to_write >> 4
            data_to_write[compressed_buf_index] |= nibble << ( bits_left_in_compress_buf_byte - 4 )

            bits_left_in_compress_buf_byte -= 4
            bits_left_in_value_to_write -= 4
            if bits_left_in_compress_buf_byte == 0 :
                compressed_buf_index += 1
                bits_left_in_compress_buf_byte = 8
                data_to_write.append( 0x00 )

    return data_to_write

this_dir = os.path.dirname( os.path.realpath("__file__") )

if len(sys.argv) == 1:
    print( "ERROR: you need to specify the name of the mp3 file to convert" )
    quit()
else:
    file_name = sys.argv[1]

write_mono = False
if len(sys.argv) == 3: # requesting stereo or mono
    if sys.argv[2] == "true" :
        write_mono = True
    elif sys.argv[2] == "false" :
        write_mono = False
    else :
        print( "ERROR: write to mono argument should be 'true' or 'false'" )
        quit()

# open files
try:
    mp3_file, sample_rate = librosa.load( os.path.join(this_dir, file_name), sr=40000, mono=write_mono )
    output_file_name_no_ext = file_name.replace( ".mp3", "" );

    if ( write_mono and len(output_file_name_no_ext) > 8 ) or ( not write_mono and len(output_file_name_no_ext) > 7 ) :
        print( "ERROR: the filename must be 8 (or 7 for stereo) or less characters for FAT16 filename size limitations" )
        quit()

    output_file_name = file_name.replace( ".mp3", ".b12" )

    if write_mono or len(mp3_file) > 2 :
        # start writing file
        output_file = open( output_file_name, 'wb' )

        # do b12 compression on the samples
        data_to_write = b12_compression( mp3_file )

        # write the buffer to the file
        output_file.write( data_to_write )

        output_file.close()
    else :
        for channel_num, channel_values in enumerate(mp3_file):
            if channel_num == 0 :
                output_file_name = file_name.replace( ".mp3", "L.b12" )
            else :
                output_file_name = file_name.replace( ".mp3", "R.b12" )

            # start writing file
            output_file = open( output_file_name, 'wb' )

            # do b12 compression on the samples
            data_to_write = b12_compression( channel_values )

            # write the buffer to the file
            output_file.write( data_to_write )

            output_file.close()

except IOError:
    print( "ERROR: font file could not be found, did you not include the .mp3 extension?" )
    quit()
