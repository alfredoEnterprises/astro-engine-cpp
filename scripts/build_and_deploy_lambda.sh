#!/bin/bash
set -e

echo "=== Locating project root ==="
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( dirname "$SCRIPT_DIR" )"

echo "Script directory: $SCRIPT_DIR"
echo "Project root:     $PROJECT_ROOT"

cd "$PROJECT_ROOT"

echo "=== 1. Clean build directory ==="
rm -rf build
mkdir build
cd build

echo "=== 2. Configure + build C++ engine ==="
cmake ..
make -j$(nproc)

echo "=== 3. Create minimal ephe folder ==="
mkdir ephe

echo "=== 4. Copy required Swiss Ephemeris files ==="
cp "$PROJECT_ROOT/external/swisseph/ephe/sepl_18.se1" ephe/
cp "$PROJECT_ROOT/external/swisseph/ephe/semo_18.se1" ephe/
cp "$PROJECT_ROOT/external/swisseph/ephe/seas_18.se1" ephe/

echo "=== 5. Copy bootstrap ==="
cp "$PROJECT_ROOT/bootstrap/bootstrap" .

echo "=== 6. Create Lambda ZIP ==="
zip -r9 chart_handler.zip bootstrap chart_handler ephe

echo "=== 7. Deploying ZIP to Lambda ==="
aws lambda update-function-code \
  --function-name astro-symbolic-engine4 \
  --zip-file fileb://chart_handler.zip


echo "=== DONE: Lambda updated successfully ==="
