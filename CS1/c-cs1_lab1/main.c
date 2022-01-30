#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define NAMEMAX 51

typedef struct student{
   char *lname;//this will require DMA to store a string
   int assignment;
   int finalExam;
   int total;
   int *quizzes;//this will require DMA to store;
}student;

student** readCourses(FILE *in, int *C, int *N, int *M){
   student** courses;
   char lname[NAMEMAX];
   fscanf(in, "%d %d %d", C, N, M);

   //Allocates the memory for C number of courses
   //Array of arrays of students aka 2D array of students
   courses = (student**) malloc(*C *(sizeof(student*))); //so courses is an array of students pointers

   //Now read C number of courses
   int cn, sn, qn;

   //allocate memory for the list of students for each courses
   for(cn = 0; cn < *C; cn++){
      courses[cn] = (student*) malloc(*N *sizeof(student));//Array of students for cnth course
      //now access each student of course[cn]
      for(sn = 0; sn < *N; sn++){
         int total = 0;//total for the student

         //read name and get the length of the name
         fscanf(in, "%s", lname);
         int nameLength = strlen(lname)+1;//+1 for NULL
         courses[cn][sn].lname = (char *) malloc(nameLength *sizeof(char));
         strcpy(courses[cn][sn].lname, lname);

         //take assignment score
         fscanf(in, "%d", &courses[cn][sn].assignment);
         total += courses[cn][sn].assignment;

         //allocatre memory for M number of quizzes for that particular student
         courses[cn][sn].quizzes = (int *) malloc (*M *sizeof(int));
         //take all quiz scores
         for(qn = 0; qn < *M; qn++){
            fscanf(in, "%d ", &courses[cn][sn].quizzes[qn]);
            total += courses[cn][sn].quizzes[qn];
         }

         //input the final exam score
         fscanf(in, "%d", &courses[cn][sn].finalExam);
         total += courses[cn][sn].finalExam;
         courses[cn][sn].total = total;
      }
   }return courses;
}

void printHighestTotal(FILE *outFile, student** courses, int C, int N, int M){
   int maxTotal = -1;
   int maxCourseIdx = 0;
   int maxStudentIdx = 0;
   int maxCourseNum = 0;

   for(int cn = 0; cn<C; cn++){
      for(int sn = 0; sn<N; sn++){
         if(courses[cn][sn].total>maxTotal){
            maxTotal = courses[cn][sn].total;
            maxCourseIdx = cn;
            maxStudentIdx = sn;
         }
      }
      if (maxCourseIdx==cn)
         maxCourseNum=cn+1;
   }

   printf("Name: %s\n", courses[maxCourseIdx][maxStudentIdx].lname);
   fprintf(outFile,"Name: %s\n", courses[maxCourseIdx][maxStudentIdx].lname);

   printf("Assignment: %d\n", courses[maxCourseIdx][maxStudentIdx].assignment);
   fprintf(outFile,"Assignment: %d\n", courses[maxCourseIdx][maxStudentIdx].assignment);

   printf("Quizzes: ");
   fprintf(outFile, "Quizzes: ");
   for(int i=0; i < M; i++){
      printf("%d ",courses[maxCourseIdx][maxStudentIdx].quizzes[i]);
      fprintf(outFile,"%d ",courses[maxCourseIdx][maxStudentIdx].quizzes[i]);
   }
   printf("\n");
   fprintf(outFile,"\n");

   printf("Final exam: %d\n",courses[maxCourseIdx][maxStudentIdx].finalExam);
   fprintf(outFile,"Final exam: %d\n",courses[maxCourseIdx][maxStudentIdx].finalExam);

   printf("Total: %d\n",courses[maxCourseIdx][maxStudentIdx].total);
   fprintf(outFile,"Total: %d\n",courses[maxCourseIdx][maxStudentIdx].total);

   printf("Course Number: %d\n", maxCourseNum);
   fprintf(outFile,"Course Number: %d\n", maxCourseNum);
}

void release_memroy(student** st, int C, int N, int M){
   //we need to release each quizzes array for each student, their name and then release the students and then courses
   for(int cn = 0; cn<C; cn++){
      for(int sn = 0; sn<N; sn++){
         free(st[cn][sn].lname);
         free(st[cn][sn].quizzes);
      }
      free(st[cn]);
   }free(st);
   //TODO MEMORY LEAK TEST
}

int main()
{
   atexit(report_mem_leak); //for memory leak detector.
   student** courseList;
   FILE *inFile, *outFile;
   int C, N, M;
   inFile = fopen("in.txt", "r");

   if(inFile!=NULL)
   {
      // printf("Reading data from input.txt...\n"); 
       
      //passing reference of C, N, and M so that we get to know what we have in the file
      courseList = readCourses(inFile, &C, &N, &M);

      outFile = fopen("out.txt", "w");


      printHighestTotal(outFile, courseList, C, N, M);

      release_memroy(courseList, C, N, M);

      fclose(inFile);
      fclose(outFile);
   }
   else
   {
      printf("Please provide correct input file\n");
      exit(-1);
   }
   return 0;
}

