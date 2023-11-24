#include "../include/akinator_promts.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define OUTPUT_BUFFER_SIZE 512
#define  INPUT_BUFFER_SIZE 32

void akinatorPrintAndSay(const wchar_t input[])
{
	//akinatorSay(input);
	wprintf(input);
}


void akinatorSay(const wchar_t input[])
{
    char cmdBuffer[OUTPUT_BUFFER_SIZE] = {};
    char inputBuffer[OUTPUT_BUFFER_SIZE / 2] = {};

    wcstombs(inputBuffer, input, sizeof(inputBuffer));
    sprintf(cmdBuffer, "echo \"%s\"|festival --tts", inputBuffer);
    system(cmdBuffer);
}


AkinatorOptions akinatorGetMode()
{
    akinatorPrintAndSay(L"Привет пользователь!\n"
                        L"Что вы от меня хотите?\n");
    int input = -1;
    while (input == -1)
    {
        wprintf(L"[1] Отгадывать\n"
                L"[2] Дать определение\n"
                L"[3] Сравнить объекты\n"
                L"[4] Показать дерево\n"
                L"[5] Выйти и сохранить\n"
                L"[6] Выйти без сохранения\n");
        input = akinatorGetOption(L"123456");
        if (input != -1)
            return (AkinatorOptions) input;
        akinatorPrintAndSay(L"Неверный ввод");
        wprintf            (L", попробуйте еще раз.\n");
    }
    return AKIN_OPT_ERROR;
}


int akinatorGetOption(const wchar_t cAllowed[])
{
    wchar_t inputBuffer[INPUT_BUFFER_SIZE] = {};
    if (fgetws(inputBuffer, sizeof(inputBuffer), stdin) == NULL)
    {
        fprintf(stderr, "FUCK YOU FGETS!\n");
        return -1;
    }
    int returnValue = inputBuffer[0];

    if (inputBuffer[1] != L'\n')
        return -1;
    if (wcsrchr(cAllowed, inputBuffer[0]) == NULL)
        return -1;
    return returnValue;
}
