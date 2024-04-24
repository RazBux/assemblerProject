#include "util.h"
#include "printBinary.h"
#include "dataCodeTable.h"

int searchWordLL(char *LABLE, WordList *wl);
char* combineIntStr(const char *str, int num);
int startSecondProcess(WordList *DC_table, WordList *IC_table, SymbolTable *st, int *Flag);

int startSecondProcess(WordList *DC_table, WordList *IC_table, SymbolTable *st, int *Flag)
{
    /* creating linkedList for both entries and externes that will help with the ARE bytes */
    WordList externes = {NULL, 0};
    WordList enteries =  {NULL, 0};

    /* linked list for both files */
    WordList extF = {NULL, 0};
    WordList entF = {NULL, 0};
    
    Node *current;
    int trackIC = IC_table->count + 100 - 1;

    /* adding the entries and externs into thier lists */
    size_t i;
    for (i = 0; i < st->symbolCount; i++)
    {
        if (strcmp(st->symbols[i].prop, "external") == 0)
        {
            addWord(&externes, st->symbols[i].symbol);
        }
        else if (strcmp(st->symbols[i].prop, "entry") == 0)
        {
            addWord(&enteries, st->symbols[i].symbol);
        }
        
    }

    printf("\nExterns_WordList >> ");
    printWordListReverse(&externes);


    // printf("\nEnteries_WordList >> ");
    // printWordListReverse(&enteries);

    /*for each symbol in the IC_Code -> change the word to bin*/
    current = IC_table->head;
    while(current != NULL){
        /*cheack for symboles - if the starting char isn't 0 or 1 -> it's a LABLE*/
        if(!(*current->word == '0' || *current->word == '1')){
            Number labNum = {0};
            size_t i; 
            
            /* change the LABLE to bin & add the ARE. >> EXTERN: 01 {1} ENTRY:  10 {2} */
            /*if the value is extern*/
            if(searchWordLL(current->word, &externes)){
                /* add extern word for the .ext file */
                char *combinW = combineIntStr(current->word, trackIC);
                addWord(&extF, combinW);
                free(combinW);

                labNum.ARE = 1;
                printf("change the number from:: %s => %s\n", current->word, getNumberBinary(&labNum));
                current->word = getNumberBinary(&labNum); /*change the lable to bin*/
            }
            else if (getSymbolIndex_dc(st, current->word) != -1) {
                int index = getSymbolIndex_dc(st, current->word);

                /*if the word in the enteries list - add it for the .ent file */
                if(searchWordLL(current->word, &enteries)){
                    char *combinW = combineIntStr(st->symbols[index].symbol, st->symbols[index].val);
                    addWord(&entF, combinW);
                    free(combinW);
                }

                /* change the LABLE to bin numbers */
                labNum.ARE = 2;
                labNum.number = st->symbols[index].val;
                printf("LabNum.number = %d\n",labNum.number);
                printf("change the number from:: %s => %s\n", current->word, getNumberBinary(&labNum));
                current->word = getNumberBinary(&labNum);
            }
            else {
                printf("Error: there is no %s in the Symbol Table\n", current->word);
                /* raise a flag so the files won't be created */
                *Flag -= 1;
            }
        }
        current = current->next;
        trackIC -= 1;
    }

    printf("DC_WordList >> ");
    printWordListReverse(DC_table);

    printf("\nIC_WordList >> ");
    printWordListReverse(IC_table);

    printWordList(&extF);
    printWordList(&entF);

    return 0;
}

/**
 * Compares each word in the linked list with a given string.
 * If a match is found, returns 1. Otherwise, returns 0.
 *
 * @param list A pointer to the WordList to traverse.
 * @param compareWord The string to compare against each word in the list.
 * @return 1 if a match is found, 0 otherwise.
 */
int searchWordLL(char *LABLE, WordList *wl)
{
    Node *current = wl->head;
    while (current != NULL)
    {
        if (strcmp(current->word, LABLE) == 0) return 1;
        current = current->next;
    }
    return 0;
}


/**
 * Combines an integer and a string into a formatted string.
 * The integer is formatted to four digits with leading zeros and combined with the string with three spaces in between.
 *
 * @param str The string part of the input.
 * @param num The integer to format and combine with the string.
 * @return A dynamically allocated string containing the formatted output. 
 *         The caller is responsible for freeing this memory.
 */
char* combineIntStr(const char *str, int num) {
    /* Allocate memory for the resulting string */ 
    /* Integer (4 digits + '\0') + 2 spaces + original string + '\0' */ 
    char *result = malloc(5 + 2 + strlen(str) + 1);
    if (result == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    /* Use sprintf to format and concatenate the string directly into the allocated buffer */ 
    sprintf(result, "%04d  %s", num, str);

    return result;
}

