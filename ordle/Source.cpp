#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <map>
#include <random>
#include <algorithm>

struct Dictionary
{
public:
  Dictionary()
  {
    std::wifstream f;
    f.open("ordlista.txt");
    if (f)
    {
      std::wstring line;
      while (std::getline(f, line))
      {
        if (line.empty())
          continue;
        ToUpper(line);
        if (IsStringOk(line))
          myWords.push_back(line);
      }
      f.close();
    }
  }

  std::vector<std::wstring> FindWordsExt(const std::vector<std::wstring>& words, const std::wstring& mustContain, const std::wstring& mustNotContain = L"", unsigned int numChar = (unsigned int)-1)
  {
    std::wstring _mustContain = mustContain;
    std::wstring _mustNotContain = mustNotContain;
    ToUpper(_mustContain);
    ToUpper(_mustNotContain);
    std::vector<std::wstring> result;
    for (auto& word : words)
    {
      if (word.length() == numChar || numChar == (unsigned int)-1)
      {
        bool isOk = true;
        for (auto& mc : _mustContain)
        {
          if (!Find(word, mc))
          {
            isOk = false;
            break;
          }
        }
        if (!isOk) continue;

        for (auto& mnc : _mustNotContain)
        {
          if (Find(word, mnc))
          {
            isOk = false;
            break;
          }
        }
        if (!isOk) continue;

        result.push_back(word);
      }
    }
    return result;
  }

  std::vector<std::wstring> FindWords(const std::wstring& mustContain, const std::wstring& mustNotContain = L"", unsigned int numChar = (unsigned int)-1)
  {
    return FindWordsExt(myWords, mustContain, mustNotContain, numChar);
  }
  /*Shoudl be like L"..D.E"*/
  std::vector<std::wstring> FindWordsSpecificExt(const std::vector<std::wstring>& words, const std::wstring& wordDef)
  {
    std::wstring _wordDef = wordDef;
    ToUpper(_wordDef);
    std::vector<std::wstring> result;
    for (auto& word : words)
    {
      if (word.length() == _wordDef.length())
      {
        bool isOk = true;
        for (unsigned int i = 0; i < _wordDef.length(); i++)
        {
          if (_wordDef[i] == L'.')
          {
            continue;
          }

          if (word.find(_wordDef[i], i) != i)
          {
            isOk = false;
            break;
          }
        }
        if (isOk)
          result.push_back(word);
      }
    }
    return result;
  }

  /*Shoudl be like L"..D.E"*/
  std::vector<std::wstring> FindWordsSpecific(const std::wstring& wordDef)
  {
    return FindWordsSpecificExt(myWords, wordDef);
  }

  const std::vector<std::wstring>& GetWords() const
  {
    return myWords;
  }

  static void ToUpper(std::wstring& str)
  {
    for (auto& c : str)
    {
      if (c == L'å') c = L'Å';
      else if (c == L'ä') c = L'Ä';
      else if (c == L'ö') c = L'Ö';
      else c = std::toupper(c);
    }
  }
private:
  bool Find(const std::wstring& str, const std::wstring& subStr)
  {
    return str.find(subStr) < str.size();
  }
  bool Find(const std::wstring& str, wchar_t c)
  {
    return str.find(c) < str.size();
  }
  bool IsStringOk(const std::wstring& str)
  {
    for (auto& c : str)
    {
      if ((c < L'A' || c > L'Z') && !(c == L'Å' || c == L'Ä' || c == L'Ö'))
      {
        return false;
      }
    }

    return true;
  }
  std::vector<std::wstring> myWords;

};


int menu()
{
  int val = 0;
  std::wcout << L"\n[MENY]\n";
  std::wcout << L"1. Ny sökning där bokstavsplatser inte spelar roll\n";
  std::wcout << L"2. Ny sökning där bokstavsplatser spelar roll\n";
  std::wcout << L"3. Fortsatt sökning där bokstavsplatser inte spelar roll\n";
  std::wcout << L"4. Fortsatt sökning där bokstavsplatser spelar roll\n";
  std::wcout << L"5. Spara resultat\n";
  std::wcout << L"6. Ge mig ord att starta med\n";
  std::wcout << L"0. Avsluta\n";
  std::wcout << "Val: ";
  std::wcin >> val;
  std::wcout << "\n";
  return val;
}

std::vector<std::wstring> newSearch(Dictionary* dic_p)
{
  std::wstring mustContain;
  std::wstring mustNotContain;
  unsigned int wordLength = 0;
  std::wcout << "Skriv \'0\' för att lämna tom...\n";
  std::wcout << "Ordet måste innehålla (ex: hjs): ";
  std::wcin >> mustContain;
  if (mustContain == L"0")
    mustContain = L"";
  std::wcout << "\nSkriv \'0\' för att lämna tom...\n";
  std::wcout << "Ordet får inte innehålla (ex: pöl): ";
  std::wcin >> mustNotContain;
  if (mustNotContain == L"0")
    mustNotContain = L"";
  std::wcout << "\nOrdets längd: ";
  std::wcin >> wordLength;
  std::wcout << "\n";

  return dic_p->FindWords(mustContain, mustNotContain, wordLength);
}
std::vector<std::wstring> newSearchSpec(Dictionary* dic_p)
{
  std::wstring mustContain;
  std::wcout << "Skriv ordet som du söker. \'.\' betyder godtycklig bokstav...\n";
  std::wcout << "Ordet måste exakt innehålla (ex: .a.t): ";
  std::wcin >> mustContain;
  std::wcout << std::endl;

  return dic_p->FindWordsSpecific(mustContain);
}
std::vector<std::wstring> newSearch(Dictionary* dic_p, const std::vector<std::wstring>& vec)
{
  std::wstring mustContain;
  std::wstring mustNotContain;
  unsigned int wordLength = 0;
  std::wcout << "Skriv \'0\' för att lämna tom...\n";
  std::wcout << "Ordet måste innehålla (ex: hjs): ";
  std::wcin >> mustContain;
  if (mustContain == L"0")
    mustContain = L"";
  std::wcout << "\nSkriv \'0\' för att lämna tom...\n";
  std::wcout << "Ordet får inte innehålla (ex: pöl): ";
  std::wcin >> mustNotContain;
  if (mustNotContain == L"0")
    mustNotContain = L"";
  std::wcout << "\nOrdets längd: ";
  std::wcin >> wordLength;
  std::wcout << "\n";

  return dic_p->FindWordsExt(vec, mustContain, mustNotContain, wordLength);
}
std::vector<std::wstring> newSearchSpec(Dictionary* dic_p, const std::vector<std::wstring>& vec)
{
  std::wstring mustContain;
  std::wcout << "Skriv ordet som du söker. \'.\' betyder godtycklig bokstav...\n";
  std::wcout << "Ordet måste exakt innehålla (ex: .a.t): ";
  std::wcin >> mustContain;
  std::wcout << std::endl;

  return dic_p->FindWordsSpecificExt(vec, mustContain);
}

#include <unordered_map>

bool IsUniqueChars(const std::wstring& str)
{
  std::unordered_map<wchar_t, bool> map;
  map[L'X'] = true;
  map[L'Z'] = true;

  for (auto& c : str)
  {
    if (map[c])
      return false;
    map[c] = true;
  }

  return true;
}

uint16_t GetNumVocals(const std::vector<std::wstring>& words)
{
  constexpr uint16_t NUM_VOCALS = 9;
  static const wchar_t VOCALS_LIST[NUM_VOCALS] = { L'A', L'O', L'U' , L'Å' , L'E' , L'I' , L'Y', L'Ä' , L'Ö' };
  std::map<wchar_t, bool> vocals_map;
  for (uint16_t i = 0; i < NUM_VOCALS; i++)
  {
    vocals_map[VOCALS_LIST[i]] = true;
  }

  uint16_t numVocals = 0;

  for (auto& w : words)
  {
    for (auto& c : w)
    {
      if (vocals_map[c]) numVocals++;
    }
  }

  return numVocals;
}

void findOptimalWordGuesses(Dictionary* dic_p, uint16_t numChars = 5, uint16_t numGuesses = 3, uint16_t numLoops = 10)
{
  srand(time(0));
  std::vector<std::wstring> blacklist;
  {
    std::wifstream blacklistInput("blacklist.txt");
    if (blacklistInput)
    {
      std::wstring line;
      while (std::getline(blacklistInput, line))
      {
        if (line.empty()) continue;
        Dictionary::ToUpper(line);
        blacklist.push_back(line);
      }

      blacklistInput.close();
    }
  }



  std::unordered_map<std::wstring, bool> blacklistMap;
  for (auto& bw : blacklist)
  {
    blacklistMap[bw] = true;
  }


  const std::vector<std::wstring>& dicWords = dic_p->GetWords();
  std::vector<std::wstring> words;

  for (auto word : dicWords)
  {
    if (word.size() == numChars)
      words.push_back(word);
  }
  size_t size = words.size();
  std::vector<size_t> indices(size);
  for (size_t i = 0; i < size; i++)
  {
    indices[i] = i;
  }

  std::wofstream f("optimal.txt");
  if (f)
  {
    for (uint16_t it = 0; it < numLoops; it++)
    {

      std::vector<std::wstring> returnVal;

      for (size_t i = 0; i < size; i++)
      {
        if (words[i].size() != numChars) continue;
        if (blacklistMap[words[i]]) continue;
        if (!IsUniqueChars(words[i])) continue;

        std::unordered_map<wchar_t, bool> usedChars;
        std::vector<std::wstring> guesses;

        for (const auto& c : words[i])
          usedChars[c] = true;

        guesses.push_back(words[i]);

        std::random_shuffle(indices.begin(), indices.end());

        for (size_t j = 0; j < size; j++)
        {
          const std::wstring& currentWord = words[indices[j]];

          if (currentWord.size() != numChars) continue;
          if (blacklistMap[currentWord]) continue;
          if (!IsUniqueChars(currentWord)) continue;

          bool wordIsOk = true;

          for (const auto& c : currentWord)
          {
            if (usedChars[c])
            {
              wordIsOk = false;
              break;
            }
          }

          if (wordIsOk)
          {
            guesses.push_back(currentWord);
            for (const auto& c : currentWord)
              usedChars[c] = true;
          }
          if (guesses.size() == numGuesses) break;

        }
        if (GetNumVocals(guesses) > GetNumVocals(returnVal))
        {
          returnVal = guesses;
        }
      }

      f << L"[Vokaler: " << GetNumVocals(returnVal) << L"]" << std::endl;
      std::wcout << L"[Vokaler: " << GetNumVocals(returnVal) << L"]" << std::endl;
      for (auto& str : returnVal)
      {
        f << str << std::endl;
        std::wcout << str << std::endl;
      }
      f << "---" << std::endl;
      std::wcout << "---" << std::endl;

    }

  }
  f.close();
}

void getGuess(Dictionary* dic_p)
{
  std::wcout << "Ordets längd: ";
  unsigned int wordLength = 0;
  std::wcin >> wordLength;
  std::wcout << "\nAntal unika ord: ";
  unsigned int numGuess = 0;
  std::wcin >> numGuess;
  std::wcout << "\nAntal iterationer: ";
  unsigned int numLoops = 0;
  std::wcin >> numLoops;

  findOptimalWordGuesses(dic_p, wordLength, numGuess, numLoops);
}

int main()
{
  _setmode(_fileno(stdout), _O_WTEXT);
  _setmode(_fileno(stdin), _O_WTEXT);
  setlocale(LC_ALL, "Swedish");

  // merge
  std::wifstream in1("ordlista1.txt");
  std::wifstream in2("ordlista2.txt");
  std::map<std::wstring, bool> map;
  if (in1 && in2)
  {
    std::wstring line;
    while (std::getline(in1, line))
    {
      if (line.empty()) continue;
      if (map[line] == false)
        map[line] = true;
    }
    while (std::getline(in2, line))
    {
      if (line.empty()) continue;
      if (map[line] == false)
        map[line] = true;
    }
    if (in1)
      in1.close();
    if (in2)
      in2.close();
    std::wofstream out1("ordlista.txt");
    if (out1)
    {
      for (auto& w : map)
        out1 << w.first << std::endl;
      out1.close();
    }
    return 0;
  }
  if (in1)
    in1.close();
  if (in2)
    in2.close();

  Dictionary dic;
  
  std::vector<std::wstring> result;
  int val = -1;
  while (val != 0)
  {
    val = menu();

    if (val == 1)
    {
      result = newSearch(&dic);
    }
    else if (val == 2)
    {
      result = newSearchSpec(&dic);
    }
    if (val == 3)
    {
      result = newSearch(&dic, result);
    }
    else if (val == 4)
    {
      result = newSearchSpec(&dic, result);
    }
    else if (val == 5)
    {
      std::wofstream of;
      of.open("resultatlista.txt");
      if (of)
      {
        for (auto& str : result)
          of << str << std::endl;
        of.close();
      }
    }
    else if (val == 6)
    {
      getGuess(&dic);
    }
    if (val > 0 && val < 5)
    {
      std::wcout << L"Resultat: \n";
      for (auto& str : result)
        std::wcout << str << std::endl;
    }
  }

  return 0;
}

