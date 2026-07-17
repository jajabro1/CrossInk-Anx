# CrossPoint Reader — Durable Context

Keep this file focused on repo-specific gotchas that are worth reusing in future sessions.

## Simulator

- Simulator patches belong in the adjacent `crosspoint-simulator` repo.
- The valid local simulator env in this repo is `simulator`, and `pio run -e simulator` currently builds cleanly.
- The simulator `PNGdec` stub in `crosspoint-simulator/src/PNGdec.h` needs to mirror the real API shape used by app code, including `hasAlpha()` and `getTransparentColor()`, even though decode still fails intentionally.
- Known simulator limits:
  - No image rendering: `platformio.ini` ignores `hal`, `PNGdec`, and `JPEGDEC`, so image decoders are intentionally absent.
  - JPEGDEC stub always fails; `JPEGDEC fallback: open failed (err=-1)` is expected in simulator.
  - `esp_deep_sleep_start()` is a no-op in simulator.
  - `HalStorage` uses POSIX file access under `./fs_` and allows multiple readers, unlike real hardware.

## Real Hardware / Storage

- SdFat on hardware allows only one open reader per file path at a time. If a fallback needs to reopen the same file, close the first handle before reopening.

## Rendering / Reader Pipeline

- `lib/Epub/Epub/Page.cpp`: images must render only in `GfxRenderer::BW`; grayscale passes are text anti-aliasing passes only.
- Kindle EPUBs may contain paired high-res and old-Kindle fallback images. `ChapterHtmlSlimParser` should skip `<img>` nodes with `data-AmznRemoved-M8` to avoid duplicate stacked images.
- After image/layout pipeline changes that affect cached EPUB output, clear the affected `.crosspoint/epub_<hash>/` cache if behavior looks stale.

## Misc Repo Gotchas

- POSIX TZ signs are inverted from ISO 8601 in `TimeStore::applyTimezone()`: `"UTC-1"` means UTC+1.
- `LyraTheme::drawHeader()` does not call `BaseTheme::drawHeader()`, so header changes in the base theme must be duplicated in Lyra if needed.

## Calibre Integration & Sync Hashing

- **EPUB Metadata Modding:** Calibre's "Send to Device" modifies EPUB files on-the-fly (covers, series, IDs), causing size/MD5 mismatches with raw filesystem copies. Always pull from the Calibre OPDS server or copy directly from the destination WebDAV folder to ensure identical hashes.
- **Cross-Subnet Discovery:** The wireless device plugin uses UDP/mDNS on startup. If Calibre is on a different subnet, set the Host IP manually in *Customize plugin* and **restart Calibre** to trigger connection.

## Rebranding & Build Environments
- **CrossAnx Branding:** The project is rebranded from `CrossInk` to `CrossAnx`. Source macros use `CROSSANX_VERSION` instead of `CROSSINK_VERSION`. i18n translation key `STR_CROSSINK` translates to `"CrossAnx"`.
- **Required Environments:** Always compile both the `tiny` and `xlarge` environments: `pio run -e tiny -e xlarge`.

