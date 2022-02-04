#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include "helpers.h"

int main(int argc, char* argv[])
{
    printf("AIFF Parser\n");
    char* file_path = "clarinet.aif";
    FILE* filePtr = fopen(file_path, "rb");
    FILE* file_ptr = filePtr;
    int result = 0;

    if (file_ptr == NULL) {
        printf("Error: Null file pointer!\n");
        result = -1;
        exit(0);
    }
    else {
        int chunkSize = 0;
        char chunkID[5];
        unsigned char sizeBuffer[4];
        char formType[5];
        unsigned int numSampleFrames = 0;
        unsigned char frameBuffer[4];
        char numChannels[2];
        int channels = 0;
        char sampleSize[2];
        int bitDepth = 0;
        char sampleRateBuffer[10];
        char** audio_start;

        chunkID[0] = fgetc(file_ptr);
        chunkID[1] = fgetc(file_ptr);
        chunkID[2] = fgetc(file_ptr);
        chunkID[3] = fgetc(file_ptr);
        chunkID[4] = '\0';

        if (0 == strcmp(chunkID, "FORM")) {
            //printf("\nFORM Chunk\n");
            sizeBuffer[0] = fgetc(file_ptr);
            sizeBuffer[1] = fgetc(file_ptr);
            sizeBuffer[2] = fgetc(file_ptr);
            sizeBuffer[3] = fgetc(file_ptr);
            chunkSize = swap4Bytes(sizeBuffer);
            //printf("Chunk Size: %d bytes\n", chunkSize);

            formType[0] = fgetc(file_ptr);
            formType[1] = fgetc(file_ptr);
            formType[2] = fgetc(file_ptr);
            formType[3] = fgetc(file_ptr);
            formType[4] = '\0';
            //printf("Form Type: %c%c%c%c\n", formType[0], formType[1], formType[2], formType[3]);
            if (0 != strcmp(formType, "AIFF") && 0 != strcmp(formType, "AIFC")) {
                printf("Error: File is not AIFF\n");
                result = -1;
                exit(0);
            }
        }
        else {
            printf("Cannot find FORM chunk!\n");
            result = -1;
            exit(0);
        }

        int foundCOMM = 0;
        int foundSSND = 0;

        while (!foundCOMM || !foundSSND) {
            chunkID[0] = fgetc(file_ptr);
            chunkID[1] = fgetc(file_ptr);
            chunkID[2] = fgetc(file_ptr);
            chunkID[3] = fgetc(file_ptr);
            chunkID[4] = '\0';

            if (0 == strcmp(chunkID, "COMM")) {
                foundCOMM = 1;
                //printf("\nCOMM Chunk\n");
                sizeBuffer[0] = fgetc(file_ptr);
                sizeBuffer[1] = fgetc(file_ptr);
                sizeBuffer[2] = fgetc(file_ptr);
                sizeBuffer[3] = fgetc(file_ptr);
                chunkSize = swap4Bytes(sizeBuffer);
                //printf("Chunk Size: %d bytes\n", chunkSize);

                numChannels[0] = fgetc(file_ptr);
                numChannels[1] = fgetc(file_ptr);
                channels = swap2Bytes(numChannels);
                printf("Channels: %d\n", channels);

                frameBuffer[0] = fgetc(file_ptr);
                frameBuffer[1] = fgetc(file_ptr);
                frameBuffer[2] = fgetc(file_ptr);
                frameBuffer[3] = fgetc(file_ptr);
                numSampleFrames = swap4Bytes(frameBuffer);
                //printf("Sample Frames: %d\n", numSampleFrames);

                printf("Number of Samples: %d\n", channels * numSampleFrames);

                sampleSize[0] = fgetc(file_ptr);
                sampleSize[1] = fgetc(file_ptr);
                bitDepth = swap2Bytes(sampleSize);

                printf("Bit Depth: %d\n", bitDepth);

                sampleRateBuffer[0] = fgetc(file_ptr);
                sampleRateBuffer[1] = fgetc(file_ptr);
                sampleRateBuffer[2] = fgetc(file_ptr);
                sampleRateBuffer[3] = fgetc(file_ptr);
                sampleRateBuffer[4] = fgetc(file_ptr);
                sampleRateBuffer[5] = fgetc(file_ptr);
                sampleRateBuffer[6] = fgetc(file_ptr);
                sampleRateBuffer[7] = fgetc(file_ptr);
                sampleRateBuffer[8] = fgetc(file_ptr);
                sampleRateBuffer[9] = fgetc(file_ptr);
                double sampleRateValue = convertFromIeeeExtended(sampleRateBuffer);
                printf("Sample Rate: %f Hz\n", sampleRateValue);
            }

            else if (0 == strcmp(chunkID, "SSND")) {
                foundSSND = 1;
                //printf("\nSSND Chunk\n");
                sizeBuffer[0] = fgetc(file_ptr);
                sizeBuffer[1] = fgetc(file_ptr);
                sizeBuffer[2] = fgetc(file_ptr);
                sizeBuffer[3] = fgetc(file_ptr);
                chunkSize = swap4Bytes(sizeBuffer);
                //printf("Chunk Size: %d bytes\n", chunkSize);

                fseek(file_ptr, 8, SEEK_CUR);

                audio_start = &file_ptr;
                printf("Start of Audio Address: %p\n", audio_start);

                int sample_data = 0;

                switch (bitDepth) {
                case 8:
                    for (int i = 0; i < 100; i++) {
                        char samplePointBuffer;
                        samplePointBuffer = fgetc(file_ptr);
                        printf("Sample %d: %d\n", i + 1, samplePointBuffer);
                    }
                    break;
                case 16:
                    for (int i = 0; i < 100; i++) {
                        char samplePointBuffer[2];
                        for (int i = 0; i < 2; i++) {
                            samplePointBuffer[i] = fgetc(file_ptr);
                        }
                        sample_data = swap2Bytes(samplePointBuffer);
                        printf("Sample %d: %d\n", i + 1, sample_data);
                    }
                    break;
                case 32:
                    for (int i = 0; i < 100; i++) {
                        char samplePointBuffer[4];
                        for (int i = 0; i < 4; i++) {
                            samplePointBuffer[i] = fgetc(file_ptr);
                        }
                        sample_data = swap4Bytes(samplePointBuffer);
                        printf("Sample %d: %d\n", i + 1, sample_data);
                    }
                    break;
                default:
                    printf("Error: Invalid bit depth!");
                    result = -1;
                    exit(0);
                }
            }

            else {
                sizeBuffer[0] = fgetc(file_ptr);
                sizeBuffer[1] = fgetc(file_ptr);
                sizeBuffer[2] = fgetc(file_ptr);
                sizeBuffer[3] = fgetc(file_ptr);
                chunkSize = swap4Bytes(sizeBuffer);
                fseek(file_ptr, chunkSize, SEEK_CUR);
            }
        }
    }
    printf("Result: %d", result);
    return 0;
}