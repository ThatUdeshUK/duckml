#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

//https://unicode.org/reports/tr15/#Norm_Forms
//https://ssl.icu-project.org/apiref/icu4c/uchar_8h.html

using Vocab = std::unordered_map<std::wstring, size_t>;
using InvVocab = std::unordered_map<size_t, std::wstring>;

std::string convertFromUnicode(const std::wstring& wText);

class BasicTokenizer {
public:
	explicit BasicTokenizer(bool doLowerCase=true);
    std::vector<std::wstring> tokenize(const std::string& text) const;

private:
    std::wstring cleanText(const std::wstring& text) const;
    bool isControol(const wchar_t& ch) const;
    bool isWhitespace(const wchar_t& ch) const;
    bool isPunctuation(const wchar_t& ch) const;
    bool isChineseChar(const wchar_t& ch) const;
    std::wstring tokenizeChineseChars(const std::wstring& text) const;
    std::wstring join(const std::vector<std::wstring>& elements) const;
    std::wstring runStripAccents(const std::wstring& text) const;
    std::vector<std::wstring> runSplitOnPunc(const std::wstring& text) const;

    bool mDoLowerCase;
};

class WordpieceTokenizer {
public:
    explicit WordpieceTokenizer() : mMaxInputCharsPerWord(0) {}
    explicit WordpieceTokenizer(std::shared_ptr<Vocab> vocab, const std::wstring& unkToken = L"[UNK]", size_t maxInputCharsPerWord=200);
    std::vector<std::wstring> tokenize(const std::wstring& text) const;

private:
    std::shared_ptr<Vocab> mVocab;
    std::wstring mUnkToken;
    size_t mMaxInputCharsPerWord;
};

class FullTokenizer {
public:
    explicit FullTokenizer() : mDoLowerCase(false) {
	}
	explicit FullTokenizer(const std::string& vocabFile, bool doLowerCase = true);
    std::vector<std::wstring> tokenize(const std::string& text) const;
    std::vector<long> convertTokensToIds(const std::vector<std::wstring>& text) const;
    long convertTokenToId(const std::wstring& token) const;
    int tokenizeToIds(const std::string& text, int64_t* input_ids, int64_t* mask, int size, int start = 0) const;
    std::vector<long> tokenizeToIds(const std::string& text, int size) const;
private:
    std::shared_ptr<Vocab> mVocab;
    InvVocab mInvVocab;
    std::string mVocabFile;
    bool mDoLowerCase;
    BasicTokenizer mBasicTokenizer;
    WordpieceTokenizer mWordpieceTokenizer;
};
