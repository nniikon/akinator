#include "../include/akinator_promts.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <wctype.h>


#define OUTPUT_BUFFER_SIZE 512
#define OUTPUT_COMMAND_SIZE 50
#define  INPUT_BUFFER_SIZE 32


void akinatorPrintAndSay(const wchar_t input[])
{
	akinatorSay(input);
	wprintf(input);
}


void akinatorSay(const wchar_t input[])
{
    char cmdBuffer[OUTPUT_BUFFER_SIZE] = {};
    char inputBuffer[OUTPUT_BUFFER_SIZE - OUTPUT_COMMAND_SIZE] = {};

    wcstombs(inputBuffer, input, sizeof(inputBuffer));
    sprintf(cmdBuffer, "echo \"%s\"|festival --tts", inputBuffer);
    system(cmdBuffer);
}

inline static AkinatorOptions convertIntToOption(const int input)
{
    if (input == -1)
        return AKIN_OPT_ERROR;

    return ((AkinatorOptions) (input - (int)'0'));
}


AkinatorOptions akinatorGetMode(AkinatorError* err)
{
    akinatorPrintAndSay(L"Что вы от меня хотите?\n");
    int input = -1;
    while (input == -1)
    {
        wprintf(L"[1] Отгадывать\n"
                L"[2] Дать определение\n"
                L"[3] Сравнить объекты\n"
                L"[4] Показать дерево\n"
                L"[5] Выйти и сохранить\n"
                L"[6] Выйти без сохранения\n");
        input = akinatorGetOption(L"123456", err);

        if (*err != AKINATOR_ERR_NO)
            return AKIN_OPT_ERROR;

        if (input != -1)
            return convertIntToOption(input);
        akinatorPrintAndSay(L"Неверный ввод");
        wprintf            (L", попробуйте еще раз.\n");
    }
    return AKIN_OPT_ERROR;
}


static wchar_t* cutUnnecessarySpace(wchar_t* input)
{
    while (iswspace(input[0]))
    {
        input++;
    }

	wchar_t* inputDup = input;
    while (!iswspace(inputDup[0]))
    {
        inputDup++;
    }

	inputDup[0] = L'\n';
	return input;
}


int akinatorGetOption(const wchar_t cAllowed[], AkinatorError* err)
{
    wchar_t inputBuffer[INPUT_BUFFER_SIZE] = {};
    if (fgetws(inputBuffer, sizeof(inputBuffer), stdin) == NULL)
    {
        fwprintf(stderr, L"FUCK YOU FGETS!\n");
        *err = AKINATOR_ERR_BAD_FGETS;
        return -1;
    }

    wchar_t* inputBufferDup = cutUnnecessarySpace(inputBuffer);

    int returnValue = inputBufferDup[0];

    if (inputBufferDup[1] != L'\n')
        return -1;

    if (wcsrchr(cAllowed, inputBufferDup[0]) == NULL)
        return -1;

    *err = AKINATOR_ERR_NO;
    return returnValue;
}


void akinatorGoodbye()
{
    akinatorPrintAndSay(L"Хорошего дня!\n");
}


void akinatorError()
{
    akinatorPrintAndSay(L"Пизда рулю, неожиданная ошибка\n");
}
