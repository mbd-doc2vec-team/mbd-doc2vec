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

// Case insensitive comparison operator.
auto comp = [](uint8_t a, uint8_t b) {
  return std::tolower(a) == std::tolower(b);
};

auto eq = [](const std::string& a, const std::string &b) {
  return std::equal(a.begin(), a.end(), b.begin(), comp);
};

// Processes the given title, removing spaces and invalid file path contents.
std::string ProcessTitle(const uint8_t *title_start, const uint8_t *title_end)
{
  std::string file_path;
  while (title_start < title_end) {
    if (*title_start == '&') {
      while (*title_start != ';') title_start++;
    } else if (std::isspace(*title_start)) {
      file_path += '_';
    } else if (std::isalnum(*title_start)) {
      file_path += std::tolower(*title_start);
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
  if (file_path.find(':') == -1) return true;
  if (file_path.find("category:") != -1) return false;
  if (file_path.find("template:") != -1) return false;
  if (file_path.find("archives:") != -1) return false;
  if (file_path.find("file:") != -1) return false;
  if (file_path.find("portal:") != -1) return false;
  if (file_path.find("wikipedia:") != -1) return false;
  if (file_path.find("draft:") != -1) return false;
  if (file_path.find("module:") != -1) return false;
  if (file_path.find("book:") != -1) return false;
  if (file_path.find("mediawiki:") != -1) return false;
  if (file_path.find("help:") != -1) return false;
  if (file_path.find("timedtext:") != -1) return false;
  if (file_path.find("topic:") != -1) return false;
  return true;
}

// Transforms the given text region into a string of space
// separated lowercase words.
std::string ProcessText(const uint8_t *text_start, const uint8_t *text_end)
{
  std::string text;

  // First go through and fix basic html stuff.
  while (text_start < text_end) {
    if (*text_start == '<') {
      while (*text_start != '>') text_start++;
    } else if (*text_start == '&') {
      text_start++;
      if (eq(std::string(text_start, text_start+2), "lt")) {
        text += "<";
        text_start += 2;
      } else if (eq(std::string(text_start, text_start+2), "gt")) {
        text += ">";
        text_start += 2;
      } else if (eq(std::string(text_start, text_start+8), "amp;nbsp")) {
        text += " ";
        text_start += 8;
      } else if (eq(std::string(text_start, text_start+3), "amp")) {
        text += "&";
        text_start += 3;
      } else if (eq(std::string(text_start, text_start+4), "nbsp")) {
        text += " ";
        text_start += 4;
      } else if (eq(std::string(text_start, text_start+4), "quot")) {
        text += "'";
        text_start += 4;
      }
    } else if (eq(std::string(text_start, text_start+6), "[http:")) {
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
  auto it = text.begin();
  while (it < text.end()) {
    if (*it == '<') {
      it++;
      if (eq(std::string(it, it+4), "ref>")) {
        it += 4;
        while (*it != '<') it++;
      }
      while (*it != '>') it++;
    } else if (*it == '[') {
      if (eq(std::string(it, it+8), "[[image:")) {
        auto pipe_it = it+8;
        auto last_pipe = pipe_it;
        while (*pipe_it != '[' && *pipe_it != ']') {
          if (*pipe_it == '|') last_pipe = pipe_it;
          pipe_it++;
        }
        it = last_pipe;
      } else if (eq(std::string(it, it+11), "[[category:")) {
        auto cat_it = it+11;
        while (*cat_it != '|' && *cat_it != ']') cat_it++;
        std::string interior_text(it+11, cat_it);
        while (*cat_it != ']') cat_it++;
        it = cat_it+1;
        cleaned_text += "[[" + interior_text + "]]";
      } else if (eq(std::string(it, it+11), "[[:category:")) {
        auto cat_it = it+12;
        while (*cat_it != '|' && *cat_it != ']') cat_it++;
        std::string interior_text(it+12, cat_it);
        while (*cat_it != ']') cat_it++;
        it = cat_it+1;
        cleaned_text += "[[" + interior_text + "]]";
      } else if (eq(std::string(it, it+2), "[[")) {
        auto fit = it+2;
        while (std::isalpha(*fit) || *fit == '-') fit++;
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
      if (eq(std::string(it, it+5), "thumb")) {
        it += 5;
      } else if (eq(std::string(it, it+4), "left")) {
        it += 4;
      } else if (eq(std::string(it, it+5), "right")) {
        it += 5;
      } else if (std::isdigit(*it)) {
        auto number_it = it;
        while (std::isdigit(*number_it)) number_it++;
        if (eq(std::string(number_it, number_it+2), "px")) {
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
  it = cleaned_text.begin();

  while (it < cleaned_text.end()) {
    if (*it == '&') {
      while (*it != ';' && std::isalnum(*it)) it++;
    } else if (std::isdigit(*it)) {
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
    } else if (std::isalpha(*it)) {
      final_text += std::tolower(*it);
    } else if (std::isspace(*it)) {
      final_text += *it;
    } else {
      final_text += ' ';
    }
    it++;
  }

  // Trim off excess space
  int r = 0, w = 0;
  while (r < final_text.size()) {
    while (r < final_text.size() && std::isspace(final_text[r])) r++;
    while (r < final_text.size() && !std::isspace(final_text[r])) {
      final_text[w++] = final_text[r++];
    }
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
