#!/bin/bash
# Regenerate the A3-only Codec2 prebuilts from the accompanying sources.
set -e
media_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
android_root=$(cd -- "$media_dir/../../../.." && pwd)
cd "$android_root"
export OUT_DIR=${OUT_DIR:-out-codec-opt}
source build/envsetup.sh
lunch lineage_a3lte-userdebug
m -j"${JOBS:-8}" liba3_codec2_av1_dav1d liba3_codec2_vp9_rowmt
for library in libcodec2_soft_av1dec_gav1.so libcodec2_soft_vp9dec.so; do
    cp "$ANDROID_PRODUCT_OUT/system/lib/a3codec-build/$library" "$media_dir/prebuilt/$library"
done
(cd "$media_dir/prebuilt" && sha256sum *.so > SHA256SUMS)
# Validate ELF dependencies and the preferred-prebuilt association as well.
m -j"${JOBS:-8}" libcodec2_soft_av1dec_gav1 libcodec2_soft_vp9dec
