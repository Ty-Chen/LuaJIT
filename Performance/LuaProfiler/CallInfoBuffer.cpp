#include "stdafx.h"
#include "CallInfoBuffer.h"

CallInfoBuffer::CallInfoBuffer()
{
    m_pszBuffer = NULL;
    m_nBufferSize = 0;
    m_nSize = 0;
    m_LastLineIndex = 0;
}

CallInfoBuffer::~CallInfoBuffer()
{
    UnInit();
}

BOOL CallInfoBuffer::Init(const char* pszPath, int nSize)
{
    BOOL bResult = false;

    if (m_pszBuffer)
    {
        free(m_pszBuffer);
        m_LastLineIndex = 0;
    }

    ASSERT_ERROR(nSize);
    m_nBufferSize = nSize;

    m_pszBuffer = (char*)malloc(nSize);
    ASSERT_ERROR(m_pszBuffer);

    memset(m_pszBuffer, 0, nSize);
    m_pszBuffer[0] = '\0';

    ASSERT_ERROR(pszPath);
    m_pFile.open(pszPath, std::ios::out);
    ASSERT_ERROR(m_pFile);

    bResult = true;
Exit0:
    if (!bResult)
    {
        UnInit();
    }
    return bResult;
}

void CallInfoBuffer::UnInit()
{
    if (m_pFile.is_open())
    {
        if (m_pszBuffer && m_nSize > 0)
        {
            m_pFile << m_pszBuffer;
        }
        m_pFile.close();
    }

    if (m_pszBuffer)
    {
        free(m_pszBuffer);
    }

    m_LastLineIndex = -1;
    m_pszBuffer = NULL;
    m_nBufferSize = 0;
    m_nSize = 0;
}

BOOL CallInfoBuffer::Add(const char* pszSourse, int nSize, bool bNewLine)
{
    BOOL bResult = false;
    BOOL bRetCode = false;
    int nIndex = 0;

    ASSERT_ERROR(pszSourse);
    ASSERT_ERROR(nSize > 0);
    ASSERT_ERROR(m_pszBuffer);

    if (m_nSize + nSize + 1 >= m_nBufferSize)
    {
        bRetCode = WriteFile();
        ASSERT_ERROR(bRetCode);
    }
    ASSERT_ERROR(m_nSize + nSize + 1 < m_nBufferSize);

    if (m_nSize > 0 && m_pszBuffer[m_nSize - 1] == '\n')
    {
        m_LastLineIndex = m_nSize;
    }

    strncpy(m_pszBuffer + m_nSize, pszSourse, nSize);
    m_nSize += nSize;

    if (bNewLine)
    {
        m_pszBuffer[m_nSize++] = '\n';
    }
    else
    {
        m_pszBuffer[m_nSize++] = ':';
    }

    m_pszBuffer[m_nSize] = '\0';

    bResult = true;
Exit0:
    return bResult;
}

BOOL CallInfoBuffer::WriteFile()
{
    BOOL bResult = false;
    int nIndex = m_nSize;
    ASSERT_ERROR(m_pFile.is_open());
    ASSERT_ERROR(m_pszBuffer);

    while (nIndex > 0 && m_pszBuffer[--nIndex] != '\n');

    ASSERT_ERROR(m_pszBuffer[nIndex] == '\n');
    m_pszBuffer[nIndex] = '\0';
    m_pFile << m_pszBuffer << std::endl;

    strncpy(m_pszBuffer, m_pszBuffer + nIndex + 1, m_nSize - nIndex);

    m_LastLineIndex = 0;
    m_nSize = m_nSize - nIndex - 1;
    bResult = true;
Exit0:
    return bResult;
}

void CallInfoBuffer::PopLastLine()
{
    if (m_LastLineIndex >= 0)
    {
        m_nSize = m_LastLineIndex;
        m_pszBuffer[m_LastLineIndex] = '\0';
        m_LastLineIndex = -1;
    }
}

const char* CallInfoBuffer::GetBuffer()
{
    return m_pszBuffer;
}

void CallInfoBuffer::GetTopText(const char*& pszOut)
{
    ASSERT_ERROR(m_pszBuffer);
    pszOut = &m_pszBuffer[m_nSize];

    while (pszOut > m_pszBuffer && *pszOut != '\n')
    {
        pszOut--;
    }

    if (*pszOut == '\n')
    {
        pszOut++;
    }
Exit0:
    return;
}
