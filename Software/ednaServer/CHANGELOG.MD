# Changes from Master V4

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)

## Unreleased

### Added

- Sample now with button support
- Depressure state before sampling to improve sampling
- Support for analog flow sensor
- System cutoff pressure
- Moving median algorithm for pressure sensor

### Changed

- Modified formula for pressure sensor based on data sheet
- Flow sensor reads in milliliters instead of liters
- Order of states is now different

### Deprecated

### Removed

### Fixed

- Flush valve no longer opens during hyperflush
- Airflush state now behave properly