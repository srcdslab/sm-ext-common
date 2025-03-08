# sm-ext-common

AlliedModders extension helper to:
- factorize ambuild scripts
- common helpers

## Compile an extension using this project

```bash
python3 ../sm-ext-common/configure.py --enable-optimize --enable-auto-versioning --ext-root="../" --sdks="$SDKS" --mms-path="$CACHE_PATH/metamod-source" --hl2sdk-manifest-path="$CACHE_PATH/hl2sdk-manifests" --hl2sdk-root="$CACHE_PATH" --sm-path="$CACHE_PATH/sourcemod"
```
