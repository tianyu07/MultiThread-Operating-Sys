#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include "cs402.h"
#include "my402list.h"

char *ctime(const time_t *time);
void stderrOutput(char *errmsg);
void ReadInput(FILE *fp, My402List *list);
FILE *CheckParameters(int argc,char* argv[]);
char *TrimString(char *s);
char *ModifyTime(char *s);
int CheckAmount(char *token);
void SortInput(My402List *list);
void PrintStatement(My402List *list);
void ClearList(My402List *list);
char *NumberPrint(double num);

typedef struct tagBankRecord{
    char type;
    int time_stamp;
    char *time;
    double amount;
    char desc[1024];
}BankRecord;

int main(int argc, char* argv[]) {
    FILE *fp = CheckParameters(argc, argv);
    My402List *list = (My402List*)malloc(sizeof(My402List));
    memset(list, 0, sizeof(My402List));
    if (list == NULL) 
        stderrOutput("Initialization failed. Insufficient memory.");
    if (!My402ListInit(list)) 
        stderrOutput("My402List cannot be initialized.");
    ReadInput(fp, list);
    SortInput(list);
    PrintStatement(list);
    ClearList(list);

    fclose(fp);
    return 0;
}

//Check if the argument parameters are correct.
FILE *CheckParameters(int argc,char* argv[]) {
    FILE *fp = NULL;
    if (argc > 3) 
        stderrOutput("Illegal input commands\nUsage: warmup1 sort [filename]");
    else if (argc == 2) {
        if (strcmp(argv[1],"sort") != 0) 
            stderrOutput("Illegal input commands\nUsage: warmup1 sort [filename]");
        fp = stdin;
    }
    else if (argc == 3) {
        if (strcmp(argv[1],"sort") != 0) 
            stderrOutput("Illegal input commands\nUsage: warmup1 sort [filename]");
        fp = fopen(argv[2],"r");
        if (fp == NULL) 
            stderrOutput("Unable to open file. File cannot found."); 
        
        int status;
        struct stat st_buf;
        status = stat (argv[2], &st_buf);
        if (S_ISDIR (st_buf.st_mode)) 
            stderrOutput("Input file is a directory."); 
        if (errno == EACCES)
            stderrOutput("Input file cannot be opened - access denies");       
    }
    else stderrOutput("Illegal input commands\nUsage: warmup1 sort [filename]");
    return fp;
}

//Provide stderr output format and exit method.
void stderrOutput(char *errmsg) {
    fprintf(stderr, "%s\n", errmsg);
    exit(1);
}

//Check from the input and store values in My402List
void ReadInput(FILE *fp, My402List *list) {
    char buf[1026];
    memset(buf, 0, 1026*sizeof(char));
    while (fgets(buf, sizeof(buf), fp) != NULL)  {
        BankRecord *transRecord = (BankRecord *)malloc(sizeof(BankRecord));
        transRecord->time = (char *)malloc(1024*sizeof(char));
        memset(transRecord, 0, sizeof(BankRecord));
        int i, count = 0;
        if (transRecord == NULL)
            stderrOutput("Unable to allocate space for transactions. Insufficient memory.");
        if (strlen(buf) > 1024)
            stderrOutput("Error input line: Characters overflow.");
        for (i = 0; i < strlen(buf); i++) 
            if (buf[i] == '\t')
                count++;
        if (count != 3)
            stderrOutput("Malformed tfile format: Should be four transaction fields.");
        
        char *s = strdup(buf);
        char delim[] = "\t";
        char *token = NULL;
        token = strtok(s, delim);
        i = 0;
        while (token != NULL) {
            if (i == 0) {
                if (strcmp(token, "-") == 0) 
                    transRecord->type = '-';
                else if (strcmp(token, "+") == 0)
                    transRecord->type = '+';
                else 
                    stderrOutput("Invalid input: Transaction type should be '+' or '-'");
            }
            
            if (i == 1) {
                char *token1 = strdup(token);
                int sys_time = (int)time(NULL);
                int time_stamp = atoi(token1);               
                if (strlen(token1) < 11 && time_stamp < sys_time) {            
                    time_t curtime = (time_t)time_stamp;                   
                    char *time = ctime(&curtime);                    
                    char *modTime = ModifyTime(time);                    
                    transRecord->time_stamp = time_stamp;
                    transRecord->time = modTime;
                }
                else 
                    stderrOutput("Invalid input: Transaction timestamp is bad.");
            }
            
            if (i == 2) {
                char *token1 = strdup(token);
                if (CheckAmount(token1)) 
                    transRecord->amount = atof(token1);  
                else 
                    stderrOutput("Invalid input: Transaction amount is not in the right format.");
            }
            
            if (i == 3) {
                char *token1 = strdup(token);
                token1 = TrimString(token1);
                if (strlen(token1) <= 24) {
                    if (token1[strlen(token1)-1] == '\n')
                        token1[strlen(token1)-1] = '\0';
                    strcpy(transRecord->desc, token1);
                }            
                else {
                    token1[24] = '\0';
                    strcpy(transRecord->desc, token1);
                }
            }
            
            token = strtok(NULL, delim);
            i++;        
        }
        
        My402ListAppend(list, (void *)transRecord);
    }
    
    if (My402ListEmpty(list))
        stderrOutput("Unable to read file. File is empty.");
    
}

//Trim string. Remove blanks before strings.
char *TrimString(char *s) {
    int i, count = 0;
    int len = strlen(s);
    for (i = 0; s[i] == ' ' || s[i] == '\t' || s[i] == '\n'; i++) {
        count++;
    }
    char *res = (char *)malloc((len-count+1)*sizeof(char));
    memset(res, 0, (len-count+1)*sizeof(char));
    strncpy(res, s+count, len-count);
    return res;
}

//Convert returned string of ctime() to correct format
char *ModifyTime(char *s) {
    char *res = (char *)malloc(16*sizeof(s));
    memset(res, 0, 16*sizeof(char));
    strncpy(res, s, 11);
    strcat(res, s+20); 
    res[strlen(res)-1] = '\0';
    return res;
}

//Check if amount data are in right format
int CheckAmount(char *token) {
    if (strlen(token) > 10 || strlen(token) < 4 || token[0] == '-' || token[0] == '.')
        return FALSE;
    if (token[0] == '0' && token[1] != '.') 
        return FALSE;
    
    char *amount = (char *)malloc((strlen(token)+1)*sizeof(char));
    memset(amount, 0, (strlen(token)+1)*sizeof(char));
    strcpy(amount, token);
    int i, flag = 0, index = 0;
    for (i = 0; i < strlen(token); i++) {
        if ((char)amount[i] == '.') {
            index = i;
            flag++;      
        }                 
        if (amount[i] != 46 && (amount[i] > 57 || amount[i] < 48)) 
            return FALSE;
    } 
    
    if (flag != 1) 
        return FALSE; 

    char *dollar = (char *)malloc(10*sizeof(char));
    char *cent = (char *)malloc(10*sizeof(char));
    memset(dollar, 0, 10*sizeof(char));
    memset(cent, 0, 10*sizeof(char));
    strncpy(dollar, amount, index);
    strcpy(cent, amount+index+1);

    if (strlen(dollar) > 7 || strlen(cent) != 2) 
        return FALSE;

    return TRUE;
}

//Sort transactions in the time order
void SortInput(My402List *list) {
    int len = My402ListLength(list);
    int i, j;
    My402ListElem *elem1, *elem2;
    BankRecord *trans1, *trans2;
    elem1 = list->anchor.next;
    for (i = 0; i < len; i++) {
        elem2 = elem1;
        for (j = i + 1; j < len; j++) {
            elem2 = elem2->next;
            trans1 = (BankRecord *)elem1->obj;
            trans2 = (BankRecord *)elem2->obj;
            if (trans1->time_stamp == trans2->time_stamp) 
                stderrOutput("Transaction time error: Two transactions are in same time.");                
            else if (trans1->time_stamp > trans2->time_stamp) {
                elem1->obj = trans2;
                elem2->obj = trans1;
            }
        }
        elem1 = elem1->next;
    }
}

//Print transactions
void PrintStatement(My402List *list) {
    int i;
    int len = My402ListLength(list);
    char *time = (char *)malloc(1024*sizeof(char));
    char *desc = (char *)malloc(1024*sizeof(char));
    memset(time, 0, 1024*sizeof(char));
    memset(desc, 0, 1024*sizeof(char));
    
    double amount = 0.0;
    double balance = 0.0;
    char *amount_str = (char *)malloc(100*sizeof(char));
    char *balance_str = (char *)malloc(100*sizeof(char));
    memset(amount_str, 0, 100*sizeof(char));
    memset(balance_str, 0, 100*sizeof(char));
    char *big_banlance = "?,???,???.??";

    My402ListElem *elem = &list->anchor;
    BankRecord *transRecord;
    
    fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");
    fprintf(stdout, "|       Date      | Description              |         Amount |        Balance |\n");
    fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");

    for (i = 0; i < len; i++) {
        elem = elem->next;
        transRecord = (BankRecord *)elem->obj;
        time = transRecord->time;
        desc = transRecord->desc;
        fprintf(stdout, "| %s | %-24s ", time, desc);

        amount = transRecord->amount;       
        if (transRecord->type == '+') {
            balance = balance + amount;
            amount_str = NumberPrint(amount);
            fprintf(stdout, "| %13s  ", amount_str);
        }
        else {
            balance = balance - amount;
            amount_str = NumberPrint(amount);
            fprintf(stdout, "| (%12s) ", amount_str);    
        }

        if (balance >= 0) {
            if (balance >= 10000000) {
                fprintf(stdout, "| %13s  |\n", big_banlance);
            }
            else {
                balance_str = NumberPrint(balance);
                fprintf(stdout, "| %13s  |\n", balance_str);
            }
        }
        else {
            if (balance <= -10000000) {
                fprintf(stdout, "| (%12s) |\n", big_banlance);
            }
            else {
                balance_str = NumberPrint(balance*(-1));
                fprintf(stdout, "| (%12s) |\n", balance_str);
            }
        }
    }
    fprintf(stdout,"+-----------------+--------------------------+----------------+----------------+\n");
    
}

//Destroy My402List and free memory
void ClearList(My402List *list) {
    My402ListUnlinkAll(list);
    free(list);
}

//Insert comma to separate double numbers
char *NumberPrint(double num) {
    int i;
    char *res = (char *)malloc(100*sizeof(char));
    memset(res, 0, 100*sizeof(char));
    snprintf(res, 100, "%f", num);
    for (i = 0; i < strlen(res); i++) 
        if (*(res+i) == '.') 
            break;
    res[i+3] = '\0';
    if (num < 1000.00)       
        return res;
    char *dec = (char *)malloc(100*sizeof(char));
    memset(dec, 0, 100*sizeof(char));
    int len = strlen(res);
    strcpy(dec, res + len - 3);
    res[i] = '\0';
    char *res1 = (char *)malloc(100*sizeof(char));
    char *res2 = (char *)malloc(100*sizeof(char));
    memset(res1, 0, 100*sizeof(char));
    memset(res2, 0, 100*sizeof(char));
    if (strlen(res) <= 6) {
        int x = strlen(res) - 3;
        strncpy(res1, res, x);
        strcat(res1, ",");
        strcat(res1, res + x);
    }
    else {
        strncpy(res1, res, 1);
        strcat(res1, ",");
        strncpy(res2, res+1, 3);
        strcat(res1, res2);
        strcat(res1, ",");
        strcat(res1, res + 4);
    }
    strcat(res1, dec);
    return res1;
}
