# VillageSQL Luhn Extension

[Luhn algorithm](https://en.wikipedia.org/wiki/Luhn_algorithm) validator and check-digit generator for VillageSQL. Catches single-digit and most transposition errors in credit-card numbers, IMEI codes, GTIN/EAN barcodes, Canadian SIN numbers, and many other industry identifiers.

## Functions

### luhn_valid

```sql
vsql_luhn.luhn_valid(value TEXT) -> INT
```

Returns `1` if the numeric string passes the Luhn checksum, `0` otherwise. Embedded whitespace and hyphens are ignored. Returns `0` (not an error) on non-numeric input, so it composes cleanly in `WHERE` clauses on dirty data.

```sql
SELECT vsql_luhn.luhn_valid('4539148803436467');          -- 1 (valid)
SELECT vsql_luhn.luhn_valid('4539148803436460');          -- 0 (invalid)
SELECT vsql_luhn.luhn_valid('4539 1488 0343 6467');       -- 1 (separators ignored)
SELECT vsql_luhn.luhn_valid('abcdef');                    -- 0 (non-numeric)
SELECT vsql_luhn.luhn_valid(NULL);                        -- NULL
```

### luhn_checkdigit

```sql
vsql_luhn.luhn_checkdigit(partial TEXT) -> TEXT
```

Generates the correct check digit for a partial identifier and returns the full string with the check digit appended. Embedded whitespace and hyphens are stripped from the output.

```sql
SELECT vsql_luhn.luhn_checkdigit('453914880343646');      -- '4539148803436467'
SELECT vsql_luhn.luhn_checkdigit('7992739871');            -- '79927398713'
```

## Installation

```sql
INSTALL EXTENSION vsql_luhn;
```

## Building from source

```bash
export VillageSQL_BUILD_DIR=/path/to/villagesql/build
./build.sh
```

## License

GPLv2 — see [LICENSE](LICENSE) for details.
