#include <stdio.h>
int multiplier = 80;

int fix(char filename[])
{
    char version;
    int frames;
    int interval;

    printf("starting %s\n", filename);

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
    printf("read finished\n");

    for(int i = 0; i < frames; i++)
    {
        data[4][i] = data[4][i] * multiplier / 100;
    }

    FILE* write = fopen(filename, "w");
    fprintf(write, "%c %d %d", version, frames, interval);
    for(int j = 0; j < 6; j++)
    {
        fprintf(write, "\n");
        for(int i = 0; i < frames; i++)
        {
            fprintf(write, "%d ", data[j][i]);
        }
    }
    fclose(write);
    printf("write finished\n\n");
}

int main(int argc, char **argv)
{
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            if(argv[i][1] == 'c' && argv[i][2] == '\0')
            {
                sscanf(argv[i+1], "%d", &multiplier);
                i++;
            }
            else
            {
                printf("Option not recognised. Exiting...\n");
                return 1;
            }
        }
        else
        {
            fix(argv[i]);
        }
    }
    return 0;
}

