#include <stdio.h>
#include <string.h>
#include <windows.h>

int wmain(int argc, wchar_t** argv) {
  const wchar_t *heart_hands = L"🫶\n";
  if (argc > 1) {
    for (wchar_t const* it = heart_hands; *it != L'\0'; ++it) {
      wchar_t buf = *it;
      WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), &buf, 1, 0, 0);
    }
  } else {
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), heart_hands, wcslen(heart_hands), 0, 0);
  }
  return 0;
}
