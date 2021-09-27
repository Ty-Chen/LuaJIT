#ifndef _CALL_INFO_BUFFER_H_
#define _CALL_INFO_BUFFER_H_
#include <stdio.h>
#include <string.h>
#include <fstream>

class CallInfoBuffer
{
public:
    CallInfoBuffer();
    ~CallInfoBuffer();

    BOOL            Init(const char* pszPath, int nSize);
    void            UnInit();

    BOOL            Add(const char* pszSourse, int nSize, bool bNewLine);
    BOOL            WriteFile();
    void            PopLastLine();
    const char*     GetBuffer();
    void            GetTopText(const char* &pszOut);

private:
    int             m_nBufferSize;
    int             m_nSize;
    char*           m_pszBuffer;

    int             m_LastLineIndex;
    std::ofstream   m_pFile;
};

#endif
