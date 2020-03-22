
#ifndef String_h
#define String_h
#include<stdio.h>
#include<vector>

#endif

/**
 * Physical modelling synthesis of a plucked string with the Karplus Strong algorithm
 * Creates a circular buffer of random values
 * Returns each sample applying a basic filter consisting of the avergae of the current sample and the next
 * The gain of the filter is less than 1 (feedback value)
 */

class KarplusString {
    std::vector<float> buffer;
    size_t             index;
    float              feedback;
    
public:
    KarplusString(float frequency, float sampleRate, float fback)
    {
        //frequency of the note is determined by buffer size
        buffer.resize(uint32_t(sampleRate/frequency));
        
        for(size_t i=0;i<buffer.size();++i){
            //random values between -1 and 1
            buffer[i] = ((float)rand()) / ((float)RAND_MAX) * 2.0f - 1.0f;
        }
        
        index = 0;
        feedback = fback;
    }
    
    float getSample()
    {
        float val = buffer[index];
        
        float sample = (buffer[index] + buffer[(index+1)%buffer.size()]) * 0.5f * feedback;
        buffer[index] = sample;
        
        index = (index + 1) % buffer.size();
        
        return val;
        
    }
};
