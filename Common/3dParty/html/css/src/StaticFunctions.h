#ifndef STATICFUNCTIONS_H
#define STATICFUNCTIONS_H

#include "../../../../../UnicodeConverter/UnicodeConverter.h"
#include "../../../../../DesktopEditor/common/File.h"
#include "CCssCalculator.h"
#include "CssCalculator_global.h"
#include "ConstValues.h"
#include <cwctype>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

namespace NSCSS
{
    namespace NS_STATIC_FUNCTIONS
    {
        inline static std::wstring stringToWstring(const std::string& sString)
        {
            return UTF8_TO_U(sString);
        }

        inline static std::string wstringToString(const std::wstring& sWstring)
        {
            return U_TO_UTF8(sWstring);
        }

        inline static std::string GetContentAsUTF8(const std::string &sString, const std::wstring &sEncoding)
        {
            const std::string& sEnc = U_TO_UTF8(sEncoding);

            NSUnicodeConverter::CUnicodeConverter oConverter;
            const std::wstring& sUnicodeContent = oConverter.toUnicode(sString, sEnc.c_str());
            return U_TO_UTF8(sUnicodeContent);
        }

        inline static std::string GetContentAsUTF8(const std::wstring& sFileName)
        {
            std::string sFileContent;

            // читаем файл как есть. utf-8 тут просто название.
            if(!NSFile::CFileBinary::ReadAllTextUtf8A(sFileName, sFileContent))
                return sFileContent;

            std::string sEncoding;
            if (true)
            {
                // определяем кодировку
                const std::string::size_type& posCharset = sFileContent.find("@charset");

                if (std::string::npos != posCharset)
                {
                    std::string::size_type pos1 = sFileContent.find_first_of("\"';", posCharset);
                    if (std::string::npos != pos1)
                    {
                        pos1 += 1;
                        std::string::size_type pos2 = sFileContent.find_first_of("\"';", pos1);
                        if (std::string::npos != pos2)
                        {
                            sEncoding = sFileContent.substr(pos1, pos2 - pos1);
                        }

                        // check valid
                        if (std::string::npos != sEncoding.find_first_of(" \n\r\t\f\v"))
                            sEncoding = "";
                    }
                }
            }

            if (sEncoding.empty())
                sEncoding = "utf-8";


            if (!sEncoding.empty() && sEncoding != "utf-8" && sEncoding != "UTF-8")
            {
                NSUnicodeConverter::CUnicodeConverter oConverter;
                sFileContent = U_TO_UTF8(oConverter.toUnicode(sFileContent, sEncoding.c_str()));
            }

            return sFileContent;
        }

        inline static bool ThereIsNumber(const std::wstring& sString)
        {
            if (sString.empty())
                return false;

            size_t posDigit = sString.find_first_of(L"0123456789");
            size_t posNoDigit = sString.find_first_not_of(L" \n\r\t\f\v123456789.");

            if (posDigit == std::wstring::npos)
                return false;

            if (posNoDigit == std::wstring::npos)
                return true;

            return (posDigit < posNoDigit) || (posDigit == 0  && posNoDigit == 0);
        }

        inline std::wstring DeleteSpace(const std::wstring& sValue)
        {
            if (sValue.empty())
                return std::wstring();

            size_t start = sValue.find_first_not_of(L" \n\r\t\f\v");
            if (std::wstring::npos == start)
                return sValue;

            size_t end = sValue.find_last_not_of(L" \n\r\t\f\v"); // точно >=0
            return sValue.substr(start, end - start + 1);
        }

        inline std::vector<std::string> GetWords(const std::wstring& sLine)
        {
            if (sLine.empty())
                return {};

            std::vector<std::string> arWords;
            arWords.reserve(16);
            const std::string sTemp = wstringToString(sLine);
            size_t posFirstNotSpace = sTemp.find_first_not_of(" \n\r\t\f\v:;,");
            while (posFirstNotSpace != std::wstring::npos)
            {
                const size_t& posLastNotSpace = sTemp.find_first_of(" \n\r\t\f\v:;,", posFirstNotSpace);
                arWords.push_back(sTemp.substr(posFirstNotSpace, posLastNotSpace - posFirstNotSpace));
                posFirstNotSpace = sTemp.find_first_not_of(" \n\r\t\f\v:;,", posLastNotSpace);
            }
            return arWords;
        }

        inline std::vector<std::wstring> GetWordsW(const std::wstring& sLine, const std::wstring& sSigns = L" \n\r\t\f\v:;,")
        {
            if (sLine.empty())
                return {};

            if (sLine.find_first_of(sSigns) == std::wstring::npos)
                return std::vector<std::wstring>({sLine});

            std::vector<std::wstring> arWords;
            arWords.reserve(16);
            size_t posFirstNotSpace = sLine.find_first_not_of(sSigns);
            while (posFirstNotSpace != std::wstring::npos)
            {
                const size_t& posLastNotSpace = sLine.find_first_of(sSigns, posFirstNotSpace);
                arWords.push_back(sLine.substr(posFirstNotSpace, posLastNotSpace - posFirstNotSpace));
                posFirstNotSpace = sLine.find_first_not_of(sSigns, posLastNotSpace);
            }
            return arWords;
        }

        inline bool ConvertAbsoluteValue(std::wstring& sAbsoluteValue)
        {
            if (sAbsoluteValue.empty())
                return false;

            bool bIsConvert = false;

            std::map<std::wstring, std::wstring> arAbsoluteValues = {{L"xx-small", L"9px"},  {L"x-small", L"10px"},
                                                                     {L"small",    L"13px"}, {L"medium",  L"16px"},
                                                                     {L"large",    L"18px"}, {L"x-large", L"24px"},
                                                                     {L"xx-large", L"32px"}};

            if (sAbsoluteValue.find(L' ') == std::wstring::npos && arAbsoluteValues.find(sAbsoluteValue) != arAbsoluteValues.end())
            {
                sAbsoluteValue = arAbsoluteValues[sAbsoluteValue];
                return true;
            }

            std::wstring sValue;
            sValue.reserve(sAbsoluteValue.length() + 1);

            for (const std::wstring& sWord : GetWordsW(sAbsoluteValue))
            {
                std::map<std::wstring, std::wstring>::iterator oFind = arAbsoluteValues.find(sWord);
                if (oFind != arAbsoluteValues.end())
                {
                    bIsConvert = true;
                    sValue += oFind->second + L' ';
                }
                else
                    sValue += sWord + L' ';
            }
            sValue.pop_back();

            if (bIsConvert)
                sAbsoluteValue = sValue;

            return bIsConvert;
        }

        inline std::vector<std::string> GetSelectorsList(const std::wstring& sSelectors)
        {
            if (sSelectors.empty())
                return std::vector<std::string>();

            std::wstring sNames = sSelectors;
            std::wstring sClasses;
            std::wstring sIds;

            size_t posLattice = sNames.find(L"#");
            if (posLattice != std::wstring::npos)
            {
                sNames = sSelectors.substr(0, posLattice);
                sIds   = sSelectors.substr(posLattice);
            }

            size_t posPoint = sNames.find(L'.');
            if (posPoint != std::wstring::npos)
            {
                sNames   = sSelectors.substr(0, posPoint);
                sClasses = sSelectors.substr(posPoint);
                posLattice = sClasses.find(L'#');
                if (posLattice != std::wstring::npos)
                    sClasses = sClasses.substr(0, posLattice);
            }

            std::vector<std::string> arNames   = GetWords(sNames);
            std::vector<std::string> arClasses = GetWords(sClasses);
            std::vector<std::string> arIds     = GetWords(sIds);
            std::vector<std::string> arSelectors(arNames);
            arSelectors.reserve(arNames.size() * arClasses.size() + arNames.size() * arIds.size() + arClasses.size() * arIds.size());

            for (std::string& sClass : arClasses)
            {
                if (sClass.find('.') == std::string::npos)
                    sClass = '.' + sClass;
                arSelectors.push_back(sClass);
                for (const std::string& sName : arNames)
                    arSelectors.push_back(sName + sClass);
                for (const std::string& sId : arIds)
                    arSelectors.push_back(sClass + sId);
            }
            for (std::string& sId : arIds)
            {
                if (sId.find('#') == std::string::npos)
                    sId = '#' + sId;
                arSelectors.push_back(sId);
                for (const std::string& sName : arNames)
                    arSelectors.push_back(sName + sId);
            }

            return arSelectors;
        }

        inline std::vector<std::string> GetSelectorsListWithParents(const std::vector<CNode> &arParents)
        {
            if (arParents.size() < 2)
                return std::vector<std::string>();

            if (arParents.back().m_sName.empty())
                return std::vector<std::string>();

            std::vector<std::string> arSelectors;

            short int nIndex = 0;

            while (nIndex < static_cast<short int>(arParents.size() - 1))
            {
                std::wstring sSelector;

                for (unsigned short int i = nIndex; i < arParents.size() - 1; ++i)
                {
                    if (arParents[i].m_sName.empty())
                        continue;

                    std::wstring sParentSelector = arParents[i].m_sName;
                    if (!arParents[i].m_sClass.empty())
                        sParentSelector += L'.' + arParents[i].m_sClass;
                    if (!arParents[i].m_sId.empty())
                        sParentSelector += L'#' + arParents[i].m_sId;

                    sSelector += sParentSelector + L' ';

                }

                std::wstring sChildrenSelector = arParents.back().m_sName;
                if (!arParents.back().m_sClass.empty())
                    sChildrenSelector += L'.' + arParents.back().m_sClass;
                if (!arParents.back().m_sId.empty())
                    sChildrenSelector += L'#' + arParents.back().m_sId;

                sSelector += sChildrenSelector;
                ++nIndex;
                arSelectors.push_back(wstringToString(sSelector));
            }

            return arSelectors;
        }

        inline std::vector<std::wstring> GetWordsWithSigns(const std::wstring& sLine, const std::wstring& sSigns = L" \n\r\t\f\v:;,")
        {
            if (sLine.empty())
                return {};

            if (sLine.find_first_of(sSigns) == std::wstring::npos)
                return std::vector<std::wstring>({sLine});

            std::vector<std::wstring> arWords;
            arWords.reserve(16);
            size_t posFirstNotSpace = sLine.find_first_not_of(sSigns);
            while (posFirstNotSpace != std::wstring::npos)
            {
                const size_t posLastNotSpace = sLine.find_first_of(sSigns, posFirstNotSpace);
                arWords.push_back(sLine.substr(posFirstNotSpace, (posLastNotSpace != std::wstring::npos) ? posLastNotSpace - posFirstNotSpace + 1 : posLastNotSpace - posFirstNotSpace ));
                posFirstNotSpace = sLine.find_first_not_of(sSigns, posLastNotSpace);
            }
            return arWords;
        }

        inline bool IsDigit(const std::wstring& sValue)
        {
            return sValue.empty() ? false : std::all_of(sValue.begin(), sValue.end(), [] (const wchar_t& cChar) { return iswdigit(cChar); });
        }

        inline std::wstring ConvertRgbToHex(const std::wstring& sRgbValue)
        {
            size_t posFirst = sRgbValue.find_first_of(L"01234567890");
            if (posFirst == std::wstring::npos)
                return std::wstring();

            const wchar_t cHex[17]{L"0123456789ABCDEF"};

            std::wstring sValue = L"#";

            while (posFirst != std::wstring::npos)
            {
                std::wstring sHex;

                const size_t posSecond = sRgbValue.find_first_of(L", )", posFirst);

                short int nValue = std::stoi(sRgbValue.substr(posFirst, posSecond - posFirst));

                if (nValue < 0)
                    sHex += L"00";
                else if (nValue > 255)
                    sHex += L"FF";
                else
                {
                    do
                    {
                        sHex = cHex[nValue % 16] + sHex;
                        nValue /= 16;
                    }
                    while (nValue != 0);
                }

                if (sHex.length() < 2)
                    sHex = L"0" + sHex;

                sValue += sHex;

                posFirst = sRgbValue.find_first_of(L"01234567890", posSecond);
            }
            return sValue;
        }

        inline std::vector<unsigned short int> GetWeightSelector(const std::string& sSelector)
        {
            if (sSelector.empty())
                return std::vector<unsigned short int>{0, 0, 0, 0};

            std::vector<unsigned short int> arWeight = {0, 0, 0, 0};

            const std::vector<std::string> arSel = NS_STATIC_FUNCTIONS::GetWords(NS_STATIC_FUNCTIONS::stringToWstring(sSelector));

            for (const std::string& sSel : arSel)
            {
                if (sSel == "*")
                    ++arWeight[3];
                else if (sSel.find('#') != std::string::npos)
                    ++arWeight[0];
                else if (sSel.find(':') != std::string::npos)
                {
                    std::string sTemp(sSel);
                    sTemp.erase(std::remove_if(sTemp.begin(), sTemp.end(), [] (wchar_t ch) { return !std::iswalpha(ch); }));
                    std::find(NS_CONST_VALUES::arPseudoClasses.begin(), NS_CONST_VALUES::arPseudoClasses.end(), sTemp) != NS_CONST_VALUES::arPseudoClasses.end() ? ++arWeight[1] : ++arWeight[2];
                }
                else if (sSel.find_first_of(".[]") != std::string::npos)
                    ++arWeight[1];
                else
                    ++arWeight[2];
            }

            return arWeight;
        }

        inline std::vector<unsigned short int> GetWeightSelector(const std::wstring& sSelector)
        {
            return GetWeightSelector(NS_STATIC_FUNCTIONS::wstringToString(sSelector));
        }

    }
}

#endif // STATICFUNCTIONS_H
