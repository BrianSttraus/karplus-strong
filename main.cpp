
#include <iostream>
#include "wave.h"
#include "String.h"
#include<vector>
#include<sstream>
#include<math.h>


float midi_to_freq(int midi);
static void show_usage(std::string name);

int main(int argc, const char * argv[]) {
   
    //Variables for properties of audio file
    int srate;
    int channels = 1;
    int secs;
    int numSamples;
    int midiNote;                               //note number
    float root,third,fifth;                     //variables for frequencies of chord
    float interval1 = 4.0, interval2 = 7.0;     //semitone intervals
    float ratio = pow(2.0, 1.0/12.0);           //semitone ratio
    bool isStereo = false;                      //flag for stereo output
    int bitDpth;
    std::string fileName;
    
    //fill array of notes for chromatic scale
    std::string noteNames[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    std::string chordName;
    int octave;                                 //octave number
    
    //open log file
    std::ofstream log("output.txt",std::ios_base::app);
    if(!log)
        return 1;
    
    //check for required parameters from user in command line
    if(argc<6){
        show_usage(argv[0]);
        return 1;
    }

    //check for optional parameters
    while (argc>1) {
        if (std::string(argv[1]) == "-s") {
            isStereo = true;
            argc--;
            argv++;
        }
        else break;
    }
    
    //parse all parameters from command line, validate and return error messages
    std::stringstream arg1(argv[1]);
    if((!(arg1 >> midiNote)) || midiNote > 127 || midiNote < 0){
        std::cout << "Invalid MIDI Note" <<"\nRange: 0-127"<<std::endl;
        return 1;
    }
    //first interval is initialized to major, do nothing
    if (std::string(argv[2]) == "maj")
        ;
    else if (std::string(argv[2]) == "min")
        interval1 = 3.0;   //minor
    else{
        std::cout << "Invalid interval" <<std::endl;
        return 1;
    }
    std::stringstream arg3(argv[3]);
    if(!(arg3 >> bitDpth)){
        std::cout << "Invalid value for BitDepth" <<std::endl;
        return 1;
    }
    std::stringstream arg4(argv[4]);
    if((!(arg4 >> srate) || srate < 0)){
        std::cout << "Invalid value for Sample Rate" << "\nSR > 0" <<std::endl;
        return 1;
    }
    std::stringstream arg5(argv[5]);
    if(!(arg5 >> secs)){
        std::cout << "Invalid value for Duration" << "\nDUR > 0" <<std::endl;
        return 1;
    }
    
    if(isStereo)
        channels = 2;
    
    //calculate total of samples to write and create array
    numSamples = srate * secs * channels;
    std::vector<float> samples(numSamples);
    
    //calculate each frequency for a triad chord
    root = midi_to_freq(midiNote);
    third = root * (pow(ratio, interval1));
    fifth = root * (pow(ratio, interval2));
    
    //generate strings with desired notes
    KarplusString note1(root,(float)srate,0.995f);
    KarplusString note2(third,(float)srate,0.995f);
    KarplusString note3(fifth,(float)srate,0.995f);
    
    //fill output array with the sum of the strings
    for(size_t i = 0; i<numSamples;){
        samples[i] = note1.getSample() + note2.getSample() + note3.getSample();
        samples[i] *= 1.0/3.0;              //attenuate signal with value of 1/N notes
        if (isStereo) {
            samples[i+1] = samples[i];      //stereo output has 2 samples per frame. Interleaved
            i+=2;
        }
        else i++;
    }
    
    //get the chord name and octave number with the midi number
    chordName = noteNames[midiNote%12];
    octave = (midiNote/12);
    
    
    fileName = chordName + std::to_string(octave) + argv[2] + ".wav";
    
    //check bit depth selection and fill a new array with the desired sample precision
    //write array to output file
    if(bitDpth == 16){
        std::vector<int16_t> int_16Samples(samples.size());
        for(size_t i=0;i<int_16Samples.size();i++){
            int_16Samples[i] = (int16_t) (samples[i] * 32767);
        }
        write_header(fileName, int_16Samples, srate, channels,0);
    }
    
    else if(bitDpth == 8){
        std::vector<int8_t> int_8Samples(samples.size());
        for(size_t i=0;i<int_8Samples.size();i++){
            int_8Samples[i] = (int8_t) (samples[i] * 127);
        }
        write_header(fileName, int_8Samples, srate, channels,0);
    }
    
    //32 bit floats. Use same array
    else if (bitDpth == 32)
        write_header(fileName, samples, srate, channels, 1);
    
    //validate user input and return error message
    else{
        std::cout << "Program only supports 16, 8, and 32 float Bit Depths" << std::endl;
        return 1;
    }
    
    //write name of output chord to log file
    log << chordName << octave << " " << argv[2] << std::endl;
    return 0;
}



float midi_to_freq(int midi)

{
    /**
     * Calculate frequency wit MIDI note number.
     * f = 2^((MIDI-69)/12) * 440.0
     */
    float freq;
    freq = pow(2.0f, ((float)midi-69.0f)/12.0f) * 440.0f;
    return freq;
    
}


static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> MidiNote Interval BitDepth SRATE DUR\n"
              << "Where:\n"
              << "MIDINote: MIDI number between 0 and 127.\n"
              << "\tFor best results use notes above E2 (MIDI note 40)\n"
              << "Interval: Use 'maj' for Major, and 'min' for Minor chord.\n"
              << "Bit Depth: 8, 16, or 32 bits.\n"
              << "Sample Rate: Any common sampling frequency value.\n"
              << "Duration: Time in seconds for duration of output file.\n"
              << "Options:\n"
              << "\t-s\tConvert output to stereo. Default: Mono\n"
              << std::endl;
}
