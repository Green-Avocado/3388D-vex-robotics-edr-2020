#include <stdio.h>
void replaceFile(char filename[])
{
    char version;
    int frames;
    int interval;
    int openingFrames;
    
    printf("starting\n");

    FILE* read = fopen(filename, "r");
    fscanf(read, "%c%d%d", &version, &frames, &interval);
    int data[6][frames];
    for(int j = 0; j < 6; j++)
    {
        fscanf(read, "\n");
        for(int i = 0; i < frames; i++)
        {
            fscanf(read, "%d", &data[j][i]);
        }
    }
    fclose(read);

    printf("read\n");

    openingFrames = 250 / interval;

    FILE* write = fopen(filename, "w");
    fprintf(write, "%c %d %d", version, frames, interval);
    for(int j = 0; j < 6; j++)
    {
        fprintf(write, "\n");
        for(int i = 0; i < frames; i++)
        {
            if(i < openingFrames)
            {
                if(j == 1)
                {
                    fprintf(write, "127 ");
                }
                else
                {
                    fprintf(write, "0 ");
                }
            }
            else
            {
                fprintf(write, "%d ", data[j][i - openingFrames]);
            }
        }
    }
    fclose(write);

    printf("written\n");
}

int main(int argc, char **argv)
{
    for(int i = 1; i < argc; i++)
    {
        replaceFile(argv[i]);
    }

    return 0;
}

