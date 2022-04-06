#include <stdio.h>
#include <stdlib.h>

#define TOTAL_STEPS 80000

int main()
{
    FILE *fl;
    fl = fopen("outputs_xyz/output.bin", "rb");
    FILE *op;
    op = fopen("output_bin.txt", "w");

    for(int i=0;i<TOTAL_STEPS/100;i++)
    {
        for(int j=0;j<1000;j++)
        {
            double tmpx, tmpy, tmpz;
            fread(&tmpx, sizeof(double), 1, fl);
            fread(&tmpy, sizeof(double), 1, fl);
            fread(&tmpz, sizeof(double), 1, fl);
            fprintf(op, "%f ", tmpx);
            fprintf(op, "%f ", tmpy);
            fprintf(op, "%f ", tmpz);
            fprintf(op, "\n");
        }
        // char newline;
        // fread(&newline, sizeof(char), 1, fl);
        // fprintf(op, "\n");
    }
    fclose(fl);
    fclose(op);
    return 0;
}