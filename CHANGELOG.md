# Changelog

## v0.4.0

* New features
  * Key reports for special characters use atoms like `:up`, `:backspace`, etc.
    rather than numbers. This is a backwards incompatible change so please be
    sure to review calls to getch/0 and receive blocks for key presses.

## v0.3.1

* Bug fixes
  * Fixed crash on stop_listening/0
  * Added missing files to hex.pm release

## v0.1.0

* Initial hex.pm release
