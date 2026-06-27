// Copyright (c) 2026 VillageSQL Contributors
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <https://www.gnu.org/licenses/>.

#include <villagesql/vsql.h>

#include <cctype>
#include <string>
#include <string_view>

using namespace vsql;

// Strips whitespace and hyphens, returns empty string on non-digit input.
static std::string strip_separators(std::string_view input) {
  std::string digits;
  digits.reserve(input.size());
  for (char c : input) {
    if (c == ' ' || c == '-' || c == '\t') continue;
    if (!std::isdigit(static_cast<unsigned char>(c))) return {};
    digits += c;
  }
  return digits;
}

// Computes the Luhn checksum for check digit generation.
// https://en.wikipedia.org/wiki/Luhn_algorithm
static int luhn_checksum(const std::string &digits) {
  int sum = 0;
  bool double_digit = true;
  for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
    int d = *it - '0';
    if (double_digit) {
      d *= 2;
      if (d > 9) d -= 9;
    }
    sum += d;
    double_digit = !double_digit;
  }
  return sum % 10;
}

// luhn_valid(value TEXT) -> INT
// Returns 1 if the string passes the Luhn checksum, 0 otherwise.
// Returns 0 (not an error) on non-numeric input.
void vdf_luhn_valid(StringArg input, IntResult out) {
  if (input.is_null()) {
    out.set_null();
    return;
  }

  std::string digits = strip_separators(input.value());
  if (digits.empty() || digits.size() < 2) {
    out.set(0);
    return;
  }

  int sum = 0;
  bool double_digit = false;
  for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
    int d = *it - '0';
    if (double_digit) {
      d *= 2;
      if (d > 9) d -= 9;
    }
    sum += d;
    double_digit = !double_digit;
  }

  out.set(sum % 10 == 0 ? 1 : 0);
}

// luhn_checkdigit(partial TEXT) -> TEXT
// Appends the correct check digit to a partial identifier.
void vdf_luhn_checkdigit(StringArg input, StringResult out) {
  if (input.is_null()) {
    out.set_null();
    return;
  }

  std::string digits = strip_separators(input.value());
  if (digits.empty()) {
    out.error("input must contain at least one digit");
    return;
  }

  int check = luhn_checksum(digits);
  int check_digit = (check == 0) ? 0 : (10 - check);
  digits += static_cast<char>('0' + check_digit);

  out.set(std::string_view(digits));
}

VEF_GENERATE_ENTRY_POINTS(
    make_extension()
        .func(make_func<&vdf_luhn_valid>("luhn_valid")
                  .returns(INT)
                  .param(STRING)
                  .deterministic()
                  .build())
        .func(make_func<&vdf_luhn_checkdigit>("luhn_checkdigit")
                  .returns(STRING)
                  .param(STRING)
                  .buffer_size(256)
                  .deterministic()
                  .build()))
