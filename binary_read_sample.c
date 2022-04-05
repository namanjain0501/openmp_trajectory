#include <stdio.h>
#include <stdlib.h>

#define TOTAL_STEPS 500

int main()
{
    FILE *fl;
    fl = fopen("output.bin", "rb");
    FILE *op;
    op = fopen("output_bin.txt", "w");

    for(int i=0;i<TOTAL_STEPS/100;i++)
    {
        for(int j=0;j<1000;j++)
        {
            double tmp;
            fread(&tmp, sizeof(double), 1, fl);
            fprintf(op, "%f ", tmp);
            fread(&tmp, sizeof(double), 1, fl);
            fprintf(op, "%f ", tmp);
            fread(&tmp, sizeof(double), 1, fl);
            fprintf(op, "%f ", tmp);
        }
        char newline;
        fread(&newline, sizeof(char), 1, fl);
        fprintf(op, "%c", newline);
    }
    fclose(fl);
    fclose(op);
    return 0;
}