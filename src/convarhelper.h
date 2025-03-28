#ifndef _INCLUDE_SOURCEMOD_EXTENSION_HELPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_HELPER_H_

#include "smsdk_ext.h"
#undef max
#undef min
#include <list>
#include <string>
#include <iostream>
#include <fstream>

typedef std::list<ConVar *> ConVarList;

ConVarList g_ConVarList;

void ExecConfig(ICvar *icvar, const char *absolutePath, const char *name, ConVarList *myConVarList)
{
    std::ifstream input(absolutePath);
    std::string line;

    const char *pBuffer;
    char token[1024];
    while (std::getline(input, line)) {
        pBuffer = engine->ParseFile(line.c_str(), token, sizeof(token));
        if (strlen(token) <= 0)
            continue;

        ConVarList::iterator iter;
        for (iter = myConVarList->begin(); iter != myConVarList->end(); iter++)
        {
            ConVar *cvar = (*iter);
            if (strcmp(cvar->GetName(), token) == 0)
            {
                pBuffer = engine->ParseFile(pBuffer, token, sizeof(token));
                cvar->SetValue(token);
                break;
            }
        }
    }
}

void AutoExecConfig(ICvar *icvar, bool autoCreate, const char *name, ConVarList *myConVarList)
{
    #ifdef PLATFORM_WINDOWS
    char szSMPath[256] = "cfg\\sourcemod\\";
    #elif defined PLATFORM_LINUX || defined PLATFORM_APPLE
    char szSMPath[256] = "cfg/sourcemod/";
    #endif

    char szName[128] = "";

    if (name != NULL)
        strcat(szName, name);
    else
    {
        strcat(szName, "extension.");
        strcat(szName, SMEXT_CONF_NAME);
        strcat(szName, ".cfg");
    }

    strcat(szSMPath, szName);

    char szPath[256];
    g_pSM->BuildPath(Path_Game, szPath, sizeof(szPath), szSMPath);

    FILE *fp = fopen(szPath, "r");
    if (fp != NULL)
    {
        ExecConfig(icvar, szPath, szName, myConVarList);
        return;
    }

    fp = fopen(szPath, "w");
    if (fp == NULL)
    {
        smutils->LogError(myself, "Failed to create config file %s.", szPath);
        return;
    }

    fprintf(fp, "// This file was auto-generated\n");
    fprintf(fp, "// ConVars for extension \"%s\"\n", SMEXT_CONF_NAME);
    fprintf(fp, "\n\n");

    ConVarList::iterator iter;
    float x;
    for (iter = myConVarList->begin(); iter != myConVarList->end(); iter++)
    {
        const ConVar *cvar = (*iter);
        #if SOURCE_ENGINE >= SE_ORANGEBOX
        if (cvar->IsFlagSet(FCVAR_DONTRECORD))
        #else
        if (cvar->IsBitSet(FCVAR_DONTRECORD))
        #endif
        {
            continue;
        }

        char descr[255];
        char *dptr = descr;

        /* Print comments until there is no more */
        ke::SafeStrcpy(descr, sizeof(descr), cvar->GetHelpText());
        while (*dptr != '\0')
        {
            /* Find the next line */
            char *next_ptr = dptr;
            while (*next_ptr != '\0')
            {
                if (*next_ptr == '\n')
                {
                    *next_ptr = '\0';
                    next_ptr++;
                    break;
                }
                next_ptr++;
            }
            fprintf(fp, "// %s\n", dptr);
            dptr = next_ptr;
        }

        fprintf(fp, "// -\n");
        fprintf(fp, "// Default: \"%s\"\n", cvar->GetDefault());
        if (cvar->GetMin(x))
        {
            fprintf(fp, "// Minimum: \"%02f\"\n", x);
        }
        if (cvar->GetMax(x))
        {
            fprintf(fp, "// Maximum: \"%02f\"\n", x);
        }
        fprintf(fp, "%s \"%s\"\n", cvar->GetName(), cvar->GetDefault());
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");

    fclose(fp);

    ExecConfig(icvar, szPath, szName, myConVarList);
}

void AutoExecConfig(ICvar *icvar, bool autoCreate, const char *name = NULL)
{
    AutoExecConfig(icvar, autoCreate, name, &g_ConVarList);
}

void AddConVar(ConVar *conVar)
{
    g_ConVarList.push_back(conVar);
}

ConVar *CreateConVar(const char *name, const char *defaultVal, int flags = 0, const char *description = "", bool hasMin = false, float min = 0.0, bool hasMax = false, float max = 0.0)
{
    ConVar *myConVar = new ConVar(strdup(name), strdup(defaultVal), flags, strdup(description), hasMin, min, hasMax, max);
    AddConVar(myConVar);
    return myConVar;
}

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
