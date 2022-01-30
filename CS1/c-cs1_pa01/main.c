#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define MAX 51

typedef struct monster {
    char *name;
    char *element;
    int population;
} monster;

typedef struct region {
    char *name;
    int nmonsters;
    int total_population;
    monster **monsters;
} region;

typedef struct itinerary {
    int nregions;
    region **regions;
    int captures;
} itinerary;

typedef struct trainer {
    char *name;
    itinerary *visits;
} trainer;

monster* createMonster(char *name, char *element, int population);
monster** readMonsters(FILE* infile, int *monsterCount);
region** readRegions(FILE* infile, int *countRegions, monster** monsterList, int monsterCount);
trainer* readTrainers(FILE* infile, int *trainerCount, region** regionList, int countRegions);
void process_inputs(monster** monsterList, int monsterCount, region** regionList,int regionCount, trainer* trainerList, int trainerCount);
void release_memory(monster** monsterList, int monsterCount, region** regionList,int regionCount, trainer* trainerList, int trainerCount);

int main(void){
    atexit(report_mem_leak);

    FILE* infile = fopen("in.txt", "r");

    // used to store specific file region amount values
    int* num = (int*) malloc(sizeof(int) * 3);
    char* temp = (char*) malloc(sizeof(char) * MAX);

    int* monsterCount = &num[0];
    int* regionCount = &num[1];
    int* trainerCount = &num[2];

    // checking to see if there's an issue opening the file
    if(infile == NULL){
        printf("Error.\n");
        exit(-1);
    }

    fscanf(infile, "%d %s\n", monsterCount, temp);
    monster** monsterData = readMonsters(infile, monsterCount);

    fscanf(infile, "%d %s\n", regionCount, temp);
    region** regionData = readRegions(infile, regionCount, monsterData, *monsterCount);

    fscanf(infile, "%d %s", trainerCount, temp);
    trainer* trainerData = readTrainers(infile, trainerCount, regionData, *regionCount);

    process_inputs(monsterData, *monsterCount, regionData, *regionCount, trainerData, *trainerCount);

    release_memory(monsterData, *monsterCount, regionData , *regionCount, trainerData, *trainerCount);

    //closing infile to avoid corruption
    fclose(infile);

    // freeing DMA variables specific to this function
    free(temp);
    free(num);
}

void release_memory(monster** monsterList, int monsterCount, region** regionList,int regionCount, trainer* trainerList, int trainerCount){

    for(int i = 0; i < trainerCount; i++){
        free(trainerList[i].visits->regions);
        free(trainerList[i].visits);
        free(trainerList[i].name);
    } free(trainerList);

    for(int i = 0; i < regionCount; i++){
        free(regionList[i]->name);
        free(regionList[i]->monsters);
        free(regionList[i]);
    } free (regionList);

    for(int i = 0; i < monsterCount; i++){
        free(monsterList[i]->name);
        free(monsterList[i]->element);
        free(monsterList[i]);
    } free(monsterList);

}

monster* createMonster(char* name, char* element, int population){
    monster* mdata = (monster*) malloc(sizeof(monster));// making space for monster data
    mdata->name = (char*) malloc(sizeof(char) * (strlen(name) + 1));// making space for the monsters name
    mdata->element = (char*) malloc(sizeof(char) * (strlen(element) + 1));// making space for the monsters element

    // ADDING VALUES TO THE INDEXED MONSTER
    strcpy(mdata->name, name);
    strcpy(mdata->element, element);
    mdata->population = population;

    return mdata;
}

monster** readMonsters(FILE* infile, int* monsterCount){
    monster** monsterStore = (monster**) malloc(sizeof(monster*) * *monsterCount);

    char* name = (char*) malloc(sizeof(char) * MAX);// monster name
    char* element = (char*) malloc(sizeof(char) * MAX);// monster element

    int* pop = (int*) malloc(sizeof(int));// monster gen. population

    // scanning every monster UNTIL the monsterCount limit has been reached
    for(int i = 0; i < *monsterCount; i++){
        fscanf(infile, "%s %s %d\n", name, element, pop);
        monsterStore[i] = createMonster(name, element, *pop);
    }

    // freeing DMA variables specific to this function
    free(pop);
    free(element);
    free(name);

    return monsterStore;
}

region** readRegions(FILE* infile, int *countRegions, monster** monsterList, int monsterCount){
    region** regionStore = (region**) malloc(sizeof(region*) * *countRegions);

    char* name = (char*) malloc(sizeof(char) * MAX);// region name
    char* temp = (char*) malloc(sizeof(char) * MAX);// garbage collector

    int* numMon = (int*) malloc(sizeof(int));// number of monsters in a region
    int* pop = (int*) malloc(sizeof(int));// population

    // allocating space for regionStore[i] and taking in region-header information
    for(int i = 0; i < *countRegions; i++){
        *pop=0;
        regionStore[i] = (region*) malloc(sizeof(region));

        fscanf(infile, "%s %d %s\n", name, numMon, temp);// temp used to clear unused string
        regionStore[i]->monsters = (monster**) malloc(sizeof(monster*) * *numMon);

        for(int j = 0; j < *numMon; j++){
            fscanf(infile, "%s", temp);// monster name mentioned in region-section

            // PURPOSE: compare monster name found w/ what was stored previously
            for(int search = 0; search < monsterCount; search++){
                if(!strcmp(temp, monsterList[search]->name)){
                    *pop += monsterList[search]->population;// collecting all values into the population var
                    regionStore[i]->monsters[j] = monsterList[search];// linking regionList[j] w/ monsterList["correct"]
                    break;
                }
            }// search-Loop
        }// j-Loop
        // taking all the useful information collected and storing it in the appropriate locations
        regionStore[i]->name = (char*) malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(regionStore[i]->name, name);
        regionStore[i]->nmonsters = *numMon;
        regionStore[i]->total_population = *pop;
    }// i-Loop

    // freeing DMA variables specific to this function
    free(pop);
    free(name);
    free(temp);
    free(numMon);

    return regionStore;
}

trainer* readTrainers(FILE* infile, int *trainerCount, region** regionList, int countRegions){
    trainer* trainerStore = (trainer*) malloc(sizeof(trainer) * *trainerCount);

    char* names = (char*) malloc(sizeof(char) * MAX);// trainer name
    char* area = (char*) malloc(sizeof(char) * MAX);// temp holder for location names
    char* temp = (char*) malloc(sizeof(char) * MAX);// garbage collector

    int* captureAmount = (int*) malloc(sizeof(int));// trainer total captures
    int* regionAmount = (int*) malloc(sizeof(int));// trainer's total locations

    for(int i = 0; i < *trainerCount; i++){
        fscanf(infile, "%s %d %s %d %s", names, captureAmount, temp, regionAmount, temp);

        // dynamically allocating all the requirted space for information we wish to store
        trainerStore[i].name = (char*) malloc(sizeof(char) * (strlen(names) + 1));
        trainerStore[i].visits = (itinerary*) malloc(sizeof(itinerary));
        trainerStore[i].visits->regions = (region**) malloc(sizeof(region*) * countRegions);

        // putting useful data into the proper DMA positions
        strcpy(trainerStore[i].name, names);
        trainerStore[i].visits->captures = *captureAmount;
        trainerStore[i].visits->nregions = *regionAmount;

        for(int j = 0; j < *regionAmount; j++){
            fscanf(infile, "%s", area);// collecting trainer's trevalled region loction

            // comparing previously gathered name w/ what's already stored
            for(int search = 0; search < countRegions; search++){
                if(!strcmp(area, regionList[search]->name)){
                    trainerStore[i].visits->regions[j] = regionList[search];
                    break;
                }
            }// search-Loop
        }// j-Loop
    }// i-Loop

    // freeing DMA variables specific to this function
    free(names);
    free(area);
    free(temp);
    free(captureAmount);
    free(regionAmount);

    return trainerStore;
}

void process_inputs(monster** monsterList, int monsterCount, region** regionList,int regionCount, trainer* trainerList, int trainerCount){
    FILE* outfile = fopen("out.txt", "w");

    double* M = (double*) malloc(sizeof(double));
    double* R = (double*) malloc(sizeof(double));
    double* temp = (double*) malloc(sizeof(double));

    int* x = (int*) malloc(sizeof(int));// used for int conversion in the calculations

    for(int person = 0; person < trainerCount; person++){// <---PURPOSE: trainerList[i]
        fprintf(outfile,"%s\n", trainerList[person].name);
        printf("%s\n", trainerList[person].name);

        for(int place = 0; place < trainerList[person].visits->nregions; place++){// <---PURPOSE: regionList[j]
            fprintf(outfile,"%s\n", trainerList[person].visits->regions[place]->name);
            printf("%s\n", trainerList[person].visits->regions[place]->name);

            for(int mon = 0; mon < trainerList[person].visits->regions[place]->nmonsters; mon++){// <---PURPOSE: monsterList[k]
                *M = trainerList[person].visits->regions[place]->monsters[mon]->population;// monsters relative population
                *R = trainerList[person].visits->regions[place]->total_population;// regions total population

                // completeing the specified math and converting the value into an integer
                *temp = *M / *R;
                *temp *= trainerList[person].visits->captures;
                *x = (int)(*temp >= 0 ? (*temp + .5) : 0);// condensed rounding statement, NOTE: (int) casting

                if(*x > 0){//only prints if there's a value 1+
                    fprintf(outfile,"%d %s\n",*x,trainerList[person].visits->regions[place]->monsters[mon]->name);// printing monster amount and name
                    printf("%d %s\n",*x,trainerList[person].visits->regions[place]->monsters[mon]->name);
                }
            }// mon-Loop
        }// place-Loop
        if(person != trainerCount-1){
            fprintf(outfile,"\n");
            printf("\n");
        }
    }// person-Loop

    // closing the outfile to make sure data is saved
    fclose(outfile);

    // freeing DMA variables specific to this function
    free(x);
    free(M);
    free(R);
    free(temp);

}

