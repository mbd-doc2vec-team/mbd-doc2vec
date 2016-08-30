#include "file.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stddef.h>
#include <stdint.h>
#include <string>

// XML tags for each segment of the page.
constexpr uint8_t kPageBegin[] =  {
  '<', 'p', 'a', 'g', 'e', '>'
};
constexpr uint8_t kPageEnd[] =  {
  '<', '/', 'p', 'a', 'g', 'e', '>'
};
constexpr uint8_t kTitleBegin[] = {
  '<', 't', 'i', 't', 'l', 'e', '>'
};
constexpr uint8_t kTitleEnd[] = {
  '<', '/', 't', 'i', 't', 'l', 'e', '>'
};
constexpr uint8_t kNoText[] = {
  '<', 't', 'e', 'x', 't', ' ',
  'x', 'm', 'l', ':', 's', 'p',
  'a', 'c', 'e', '=', '"', 'p',
  'r', 'e', 's', 'e', 'r', 'v',
  'e', '"', ' ', '/', '>'
};
constexpr uint8_t kTextBegin[] = {
  '<', 't', 'e', 'x', 't', ' ',
  'x', 'm', 'l', ':', 's', 'p',
  'a', 'c', 'e', '=', '"', 'p',
  'r', 'e', 's', 'e', 'r', 'v',
  'e', '"', '>'
};
constexpr uint8_t kIdBegin[] = {
  '<', 'i', 'd', '>'
};
constexpr uint8_t kIdEnd[] = {
  '<', '/', 'i', 'd', '>'
};
constexpr uint8_t kTextEnd[] = {
  '<', '/', 't', 'e', 'x', 't', '>'
};
constexpr uint8_t kRedirect[] = {
  '#', 'r', 'e', 'd', 'i', 'r', 'e', 'c', 't'
};

constexpr uint8_t kToLower[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
  0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
  0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
  0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
  0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
  0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
  0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
  0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

constexpr uint8_t ToLower(uint8_t c)
{
  return kToLower[c];
}

constexpr bool IsSpace(uint8_t c)
{
  return c == ' ' || c == '\n';
}

constexpr bool IsAlpha(uint8_t c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

constexpr bool IsDigit(uint8_t c)
{
  return c >= '0' &&  c <= '9';
}

// Case insensitive comparison operator.
constexpr bool comp(uint8_t a, uint8_t b)
{
  return ToLower(a) == ToLower(b);
};

// Compare if two strings are equivalent.
bool eq(const void * __restrict__ aa,
        const void * __restrict__ bb,
        size_t c)
{
  auto a = (const uint8_t*) aa;
  auto b = (const uint8_t*) bb;
  for (size_t i = 0; i < c; i++) {
    if (!comp(a[i], b[i])) return false;
  }
  return true;
};

// Processes the given title, removing spaces and invalid file path contents.
std::string ProcessTitle(const uint8_t * __restrict__ title_start,
                         const uint8_t * __restrict__ title_end)
{
  std::string file_path;
  while (title_start < title_end) {
    if (*title_start == '&') {
      while (*title_start != ';') title_start++;
    } else if (IsSpace(*title_start)) {
      file_path += '_';
    } else if (IsAlpha(*title_start)) {
      file_path += ToLower(*title_start);
    } else if (IsDigit(*title_start)) {
      file_path += *title_start;
    } else if (*title_start == '/') {
      file_path += "_slash_";
    } else if (*title_start == '.') {
      file_path += "_dot_";
    } else {
      file_path += *title_start;
    }
    title_start++;
  }
  return file_path;
}

// Returns if the given string is a good title or not.
bool GoodTitle(const std::string &file_path)
{
  if (file_path.find(":") == -1) return true;
  if (file_path.find("archives:") != -1) return false;
  if (file_path.find("book:") != -1) return false;
  if (file_path.find("category:") != -1) return false;
  if (file_path.find("draft:") != -1) return false;
  if (file_path.find("file:") != -1) return false;
  if (file_path.find("help:") != -1) return false;
  if (file_path.find("mediawiki:") != -1) return false;
  if (file_path.find("module:") != -1) return false;
  if (file_path.find("portal:") != -1) return false;
  if (file_path.find("template:") != -1) return false;
  if (file_path.find("timedtext:") != -1) return false;
  if (file_path.find("topic:") != -1) return false;
  if (file_path.find("wikipedia:") != -1) return false;
  return true;
}

// Transforms the given text region into a string of space
// separated lowercase words.
std::string ProcessText(const uint8_t * __restrict__ text_start,
                        const uint8_t * __restrict__ text_end)
{
  std::string text;

  // First go through and fix basic html stuff.
  while (text_start < text_end) {
    if (*text_start == '<') {
      while (*text_start != '>') text_start++;
    } else if (*text_start == '&') {
      text_start++;
      if (eq(text_start, "lt", 2)) {
        text += "<";
        text_start += 2;
      } else if (eq(text_start, "gt", 2)) {
        text += ">";
        text_start += 2;
      } else if (eq(text_start, "amp;nbsp", 8)) {
        text += " ";
        text_start += 8;
      } else if (eq(text_start, "amp", 3)) {
        text += "&";
        text_start += 3;
      } else if (eq(text_start, "nbsp", 4)) {
        text += " ";
        text_start += 4;
      } else if (eq(text_start, "quot", 4)) {
        text += "'";
        text_start += 4;
      }
    } else if (eq(text_start, "[http:", 6)) {
      text_start += 6;
      while (*text_start != ']' && *text_start != ' ') text_start++;
      text += '[';
    } else {
      text += *text_start;
    }
    text_start++;
  }

  // Now rip off any new tags
  std::string cleaned_text;
  auto it = text.c_str();
  auto end = it + text.size();
  while (it < end) {
    if (*it == '<') {
      it++;
      if (eq(it, "ref>", 4)) {
        it += 4;
        while (*it != '<') it++;
      }
      while (*it != '>') it++;
    } else if (*it == '[') {
      if (eq(it, "[[image:", 8)) {
        auto pipe_it = it+8;
        auto last_pipe = pipe_it;
        while (*pipe_it != '[' && *pipe_it != ']') {
          if (*pipe_it == '|') last_pipe = pipe_it;
          pipe_it++;
        }
        it = last_pipe;
      } else if (eq(it, "[[category:", 11)) {
        auto cat_it = it+11;
        while (*cat_it != '|' && *cat_it != ']') cat_it++;
        std::string interior_text(it+11, cat_it);
        while (*cat_it != ']') cat_it++;
        it = cat_it+1;
        cleaned_text += "[[" + interior_text + "]]";
      } else if (eq(it, "[[:category:", 11)) {
        auto cat_it = it+12;
        while (*cat_it != '|' && *cat_it != ']') cat_it++;
        std::string interior_text(it+12, cat_it);
        while (*cat_it != ']') cat_it++;
        it = cat_it+1;
        cleaned_text += "[[" + interior_text + "]]";
      } else if (eq(it, "[[", 2)) {
        auto fit = it+2;
        while (IsAlpha(*fit) || *fit == '-') fit++;
        if (*fit == ':') {
          while (*fit != ']') fit++;
          fit++;
        } else {
          fit = it+2;
          while (*fit != '|' && *fit != ']') fit++;
          cleaned_text += "[[";
          if (*fit == '|') {
            it = fit;
          }
        }
      }
    } else if (*it == '|') {
      it++;
      if (eq(it, "thumb", 5)) {
        it += 5;
      } else if (eq(it, "left", 4)) {
        it += 4;
      } else if (eq(it, "right", 5)) {
        it += 5;
      } else if (IsDigit(*it)) {
        auto number_it = it;
        while (IsDigit(*number_it)) number_it++;
        if (eq(number_it, "px", 2)) {
          it = number_it+2;
        }
      }
    } else if (*it == '{') {
      it++;
      if (*it == '{') {
        while (*it != '}') it++;
        it++;
      } else {
        while (*it != '}') it++;
      }
    } else {
      cleaned_text += *it;
    }
    it++;
  }

  // Finally, clean up extraneous characters.
  std::string final_text;
  it = cleaned_text.c_str();
  end = it + cleaned_text.size();
  while (it < end) {
    if (*it == '&') {
      while (*it != ';' && std::isalnum(*it)) it++;
    } else if (IsDigit(*it)) {
      // TODO(ZacharyForman): Turn these into proper text?
      switch (*it) {
        case '0': final_text += " zero "; break;
        case '1': final_text += " one "; break;
        case '2': final_text += " two "; break;
        case '3': final_text += " three "; break;
        case '4': final_text += " four "; break;
        case '5': final_text += " five "; break;
        case '6': final_text += " six "; break;
        case '7': final_text += " seven "; break;
        case '8': final_text += " eight "; break;
        case '9': final_text += " nine "; break;
      }
    } else if (IsAlpha(*it)) {
      final_text += ToLower(*it);
    } else if (IsSpace(*it)) {
      final_text += *it;
    } else {
      final_text += ' ';
    }
    it++;
  }

  // Trim off excess space
  size_t r = 0, w = 0;
  for (;;) {
    while (r < final_text.size() && IsSpace(final_text[r])) r++;
    while (r < final_text.size() && !IsSpace(final_text[r])) {
      final_text[w++] = final_text[r++];
    }
    if (r == final_text.size()) break;
    final_text[w++] = ' ';
  }
  final_text.resize(w);

  return final_text;
}

#define FIND(str) \
  std::search(buf, buf_end, std::begin(str), std::end(str), comp)

void ReadAndWriteArticles(const File &xml)
{
  const uint8_t *buf = xml.buffer();
  const uint8_t *buf_end = buf + xml.size();

  while (buf < buf_end) {
    const uint8_t *page_start = FIND(kPageBegin) + sizeof(kPageBegin);
    buf = page_start;

    const uint8_t *title_start = FIND(kTitleBegin) + sizeof(kTitleBegin);
    buf = title_start;
    const uint8_t *title_end = FIND(kTitleEnd);
    buf = title_end + sizeof(kTitleEnd);

    const uint8_t *id_start = FIND(kIdBegin) + sizeof(kIdBegin);
    buf = id_start;
    const uint8_t *id_end = FIND(kIdEnd);
    buf = id_end + sizeof(kIdEnd);

    if (std::search(buf, std::min(buf+500, buf_end), std::begin(kNoText),
                    std::end(kNoText), comp) != buf+500) {
      buf = FIND(kPageEnd) + sizeof(kPageEnd);
      continue;
    }

    const uint8_t *text_start = FIND(kTextBegin) + sizeof(kTextBegin);
    buf = text_start;
    const uint8_t *text_end = FIND(kTextEnd);
    buf = text_end + sizeof(kTextEnd);

    if (std::search(text_start, text_end, std::begin(kRedirect),
                    std::end(kRedirect), comp) != text_end) {
      buf = FIND(kPageEnd) + sizeof(kPageEnd);
      continue;
    }

    const uint8_t *page_end = FIND(kPageEnd);
    buf = page_end + sizeof(kPageEnd);

    std::string title = ProcessTitle(title_start, title_end);
    if (!GoodTitle(title)) continue;
    std::string file_path(id_start, id_end);

    std::string article_text = ProcessText(text_start, text_end);

    std::cout << title << '.' << file_path << ' ' << article_text << '\n';
  }
}

#undef FIND

int main(int argc, char **argv)
{
  std::ios_base::sync_with_stdio(false);
  ReadAndWriteArticles(File(argv[1]));
}
