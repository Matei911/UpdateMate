#include "drawPage3.h"
#include <SD.h>
#include <vector>

// selection state
volatile bool selectBooks = true;
volatile int selectedBookIndex = 0;
volatile bool listNeedsFullRefresh = false;
int maxBookIdx = 0;

// scrolling state
int scrollLine = 0;
int scrollStep = 50;  // Default, will be updated by drawBookContent

static const int MAX_W = 460;
static const int LEFT_X = 10;
static const int LINE_H = 16;
static const int RESERVED_ROWS_FOR_FOOTER = 3;
static const int RESERVED_ROWS_FOR_HEADER = 3;


static std::vector<String> fileList;


static void loadTxtFiles(const char* path = "/") {
  fileList.clear();
  File dir = SD.open(path);
  if (!dir) return;
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    if (!entry.isDirectory()) {
      String name = entry.name();
      if (name.endsWith(".txt")) fileList.push_back(name);
    }
    entry.close();
  }
  dir.close();
  maxBookIdx = fileList.size();
}

static String formatFileName(const String& filename) {
  String formattedName = filename;

  // 1. Remove the .txt extension
  if (formattedName.endsWith(".txt")) {
    formattedName = formattedName.substring(0, formattedName.length() - 4);
  }

  // 2. Replace underscores with spaces
  formattedName.replace('_', ' ');

  // 3. Capitalize the first letter of each word
  if (formattedName.length() > 0) {
    // Capitalize the very first letter
    formattedName[0] = toupper(formattedName[0]);

    // Loop through the rest of the string to find spaces
    for (int i = 1; i < formattedName.length(); i++) {
      if (formattedName[i - 1] == ' ') {
        formattedName[i] = toupper(formattedName[i]);
      }
    }
  }

  return formattedName;
}

// This is for the "Books" list view (landscape)

static void drawHeader() {
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(65, 90);
  display.setTextColor(GxEPD_BLACK);
  display.print("Books");
  display.fillRect(50, 100, 130, 3, GxEPD_BLACK);
}

// Landscape for book list, here the user will select the books
static void drawListMode() {
  display.setRotation(0);

  if (listNeedsFullRefresh) {
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    listNeedsFullRefresh = false;
  }

  display.setPartialWindow(0, 0, 800, 480);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawHeader();  // Draws the "Books" title for the list
    drawSensorAndClockFinal();
    display.setFont(&FreeMonoBold12pt7b);
    const int startX = 20, startY = 140, lineH_local = 40;

    for (int i = 0; i <= maxBookIdx; i++) {
      int y = startY + i * lineH_local;
      String label;
      if (i < maxBookIdx) {
        // Format the filename before displaying it
        label = formatFileName(fileList[i]);
      } else {
        label = "Exit";
      }
      // if book is selected create a rectangle around it
      if (selectBooks && i == selectedBookIndex) {
        int16_t x0, y0;
        uint16_t w, h;
        display.getTextBounds(label, 0, 0, &x0, &y0, &w, &h);
        display.fillRect(startX - 4, y - (h - 2), w + 8, h + 6, GxEPD_BLACK);
        display.setTextColor(GxEPD_WHITE);
      } else {
        display.setTextColor(GxEPD_BLACK);
      }

      display.setCursor(startX, y);
      display.print(label);
    }
    display.setTextColor(GxEPD_BLACK);
  } while (display.nextPage());
}
/*
      This is a helper function whose job is to determine how many characters 
  of a very long word can fit onto a single line without exceeding the screen's width
*/
static int calculateFittingLength(const String& s, int maxWidth) {
  if (s.isEmpty()) return 0;

  int16_t x0_bounds, y0_bounds;
  uint16_t w_bounds, h_bounds;

  for (int k = 1; k <= s.length(); ++k) {
    String prefix = s.substring(0, k);
    display.getTextBounds(prefix, 0, 0, &x0_bounds, &y0_bounds, &w_bounds, &h_bounds);
    if (x0_bounds + w_bounds > maxWidth) {
      return (k > 1) ? (k - 1) : 1;
    }
  }
  return s.length();
}
/*
      This function advances the position within the text file by a specified number of 
  visual lines, not just file lines
*/
static void skipWrappedLines(File& f, int n_to_skip) {
  if (n_to_skip <= 0) return;

  int lines_actually_skipped = 0;

  while (lines_actually_skipped < n_to_skip && f.available()) {
    uint32_t physical_line_start_fpos = f.position();
    String raw_physical_line = f.readStringUntil('\n');

    if (raw_physical_line.isEmpty() && !f.available() && f.position() == physical_line_start_fpos) {
      break;
    }

    raw_physical_line.replace("\r", "");
    raw_physical_line.replace("“", "\"");
    raw_physical_line.replace("”", "\"");

    String current_visual_line_text = "";
    int current_char_idx_in_raw = 0;
    int raw_len = raw_physical_line.length();

    if (raw_len == 0) {
      continue;
    }

    while (current_char_idx_in_raw < raw_len) {
      int next_space_idx = raw_physical_line.indexOf(' ', current_char_idx_in_raw);
      if (next_space_idx == -1) next_space_idx = raw_len;

      String word = raw_physical_line.substring(current_char_idx_in_raw, next_space_idx);
      String trial_text = current_visual_line_text.length() ? current_visual_line_text + ' ' + word : word;

      int16_t x0_trial, y0_trial;
      uint16_t w_trial, h_trial;
      display.getTextBounds(trial_text, 0, 0, &x0_trial, &y0_trial, &w_trial, &h_trial);

      if (x0_trial + w_trial > MAX_W) {
        lines_actually_skipped++;
        if (lines_actually_skipped == n_to_skip) {
          f.seek(physical_line_start_fpos + current_char_idx_in_raw);
          return;
        }
        current_visual_line_text = "";

        String word_to_process = word;
        int word_segment_start_idx_in_raw = current_char_idx_in_raw;

        while (!word_to_process.isEmpty()) {
          int16_t x0_word, y0_word;
          uint16_t w_word, h_word;
          display.getTextBounds(word_to_process, 0, 0, &x0_word, &y0_word, &w_word, &h_word);

          if (x0_word + w_word > MAX_W) {
            int fitting_len = calculateFittingLength(word_to_process, MAX_W);

            lines_actually_skipped++;
            if (lines_actually_skipped == n_to_skip) {
              f.seek(physical_line_start_fpos + word_segment_start_idx_in_raw + fitting_len);
              return;
            }
            word_to_process = word_to_process.substring(fitting_len);
            word_segment_start_idx_in_raw += fitting_len;
            if (word_to_process.isEmpty()) break;
          } else {
            current_visual_line_text = word_to_process;
            break;
          }
        }
      } else {
        current_visual_line_text = trial_text;
      }
      current_char_idx_in_raw = next_space_idx + 1;
    }

    if (!current_visual_line_text.isEmpty()) {
      lines_actually_skipped++;
      if (lines_actually_skipped == n_to_skip) {
        return;
      }
    }
  }
}

/*
      This is the main rendering function that reads a portion of the text file 
  and draws it to the screen, complete with word wrapping
*/
static void drawBookContent() {
  if (selectedBookIndex >= maxBookIdx || selectedBookIndex < 0) return;

  File f = SD.open("/" + fileList[selectedBookIndex]);
  if (!f) {
    // Error message will be drawn by drawBookViewMode
    return;
  }

  display.setFont(&FreeMonoBold9pt7b);  // Font for book content

  // Calculate maxRows available for content, leaving space for header and footer
  const int total_lines_on_screen = display.height() / LINE_H;  // display.height() is 800 in portrait
  int available_content_area_lines = total_lines_on_screen - RESERVED_ROWS_FOR_HEADER - RESERVED_ROWS_FOR_FOOTER;
  int maxRows_for_content = (available_content_area_lines > 0) ? available_content_area_lines : 0;

  int rowsLeft = maxRows_for_content;
  int rowsDrawn = 0;

  skipWrappedLines(f, scrollLine);

  // Current Y position for drawing text (top of text, cursor sets baseline)
  // Start drawing below the reserved header space.
  int y = RESERVED_ROWS_FOR_HEADER * LINE_H;

  while (rowsLeft > 0 && f.available()) {
    uint32_t lineStart_fpos = f.position();
    String raw = f.readStringUntil('\n');
    if (raw.isEmpty() && !f.available() && f.position() == lineStart_fpos) {
      break;
    }

    raw.replace("\r", "");
    raw.replace("“", "\"");
    raw.replace("”", "\"");

    String visualLine = "";
    int pos = 0, raw_line_len = raw.length();
    bool finishedPhysicalLine = true;

    if (raw_line_len == 0) {
      continue;
    }

    while (pos < raw_line_len && rowsLeft > 0) {
      int sp = raw.indexOf(' ', pos);
      if (sp == -1) sp = raw_line_len;
      String word = raw.substring(pos, sp);
      String trial = visualLine.length() ? visualLine + ' ' + word : word;

      int16_t x0_bounds, y0_bounds;
      uint16_t w_bounds, h_bounds;
      display.getTextBounds(trial, 0, 0, &x0_bounds, &y0_bounds, &w_bounds, &h_bounds);

      if (x0_bounds + w_bounds > MAX_W) {
        // y is the top-Y of the line box, LINE_H is baseline offset
        display.setCursor(LEFT_X, y + LINE_H);
        display.print(visualLine);
        y += LINE_H;
        --rowsLeft;
        ++rowsDrawn;

        if (rowsLeft == 0) {
          finishedPhysicalLine = false;
          break;
        }
        visualLine = "";

        String current_word_segment = word;
        while (!current_word_segment.isEmpty() && rowsLeft > 0) {
          int16_t x0_cws, y0_cws;
          uint16_t w_cws, h_cws;
          display.getTextBounds(current_word_segment, 0, 0, &x0_cws, &y0_cws, &w_cws, &h_cws);

          if (x0_cws + w_cws > MAX_W) {
            int fitting_len = calculateFittingLength(current_word_segment, MAX_W);
            String chunk = current_word_segment.substring(0, fitting_len);

            display.setCursor(LEFT_X, y + LINE_H);
            display.print(chunk);
            y += LINE_H;
            --rowsLeft;
            ++rowsDrawn;

            current_word_segment = current_word_segment.substring(fitting_len);
            if (rowsLeft == 0) {
              finishedPhysicalLine = false;
              break;
            }
            if (current_word_segment.isEmpty()) break;
          } else {
            visualLine = current_word_segment;
            break;
          }
        }
        if (!finishedPhysicalLine) break;

      } else {
        visualLine = trial;
      }
      pos = sp + 1;
    }

    if (finishedPhysicalLine && !visualLine.isEmpty() && rowsLeft > 0) {
      display.setCursor(LEFT_X, y + LINE_H);
      display.print(visualLine);
      y += LINE_H;
      --rowsLeft;
      ++rowsDrawn;
    }

    if (!finishedPhysicalLine) {
      f.seek(lineStart_fpos);
      break;
    }
  }
  f.close();

  scrollStep = rowsDrawn > 0 ? rowsDrawn : 1;
}

static void drawBookViewMode() {
  display.setRotation(1);  // portrait
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawSensorAndClockBook();

    bool fileError = false;
    if (selectedBookIndex < maxBookIdx && selectedBookIndex >= 0) {
      File testFile = SD.open("/" + fileList[selectedBookIndex]);
      if (!testFile) {
        fileError = true;
      } else {
        testFile.close();
        drawBookContent();  // Draws content between header and footer
      }
    } else {
      fileError = true;  // No valid book selected
    }

    // Set font and color for error messages or footer text
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);

    if (fileError) {
      String errorMsg = (selectedBookIndex >= maxBookIdx || selectedBookIndex < 0) ? "No book selected." : "Error: File not found.";
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(errorMsg, 0, 0, &x1, &y1, &w, &h);
      // Draw error message below the reserved header space
      int error_msg_y = (RESERVED_ROWS_FOR_HEADER * LINE_H) + LINE_H;
      display.setCursor((display.width() - w) / 2, error_msg_y);
      display.print(errorMsg);
    }

    // Page number display in the footer
    int currentPage = (scrollLine / (scrollStep > 0 ? scrollStep : 1)) + 1;
    String pageStr = "Page " + String(currentPage);

    int16_t x_page, y_page_bounds;
    uint16_t w_page, h_page;
    display.getTextBounds(pageStr, 0, 0, &x_page, &y_page_bounds, &w_page, &h_page);

    int page_count_x = (display.width() - w_page) / 2;
    int page_count_y = display.height() - (RESERVED_ROWS_FOR_FOOTER / 2 + 1) * LINE_H;
    page_count_y = display.height() - (2 * LINE_H) + (LINE_H - h_page) / 2 - y_page_bounds;
    page_count_y = display.height() - LINE_H;


    display.setCursor(page_count_x, page_count_y);
    display.print(pageStr);

  } while (display.nextPage());
}

void fullRefreshPage3() {
  if (listNeedsFullRefresh) {  // Coming back to list view
    loadTxtFiles("/");
  }

  if (selectBooks) {
    drawListMode();
  } else {
    drawBookViewMode();
  }
}