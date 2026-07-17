#include "KOReaderDocumentId.h"

#include <HalStorage.h>
#include <Logging.h>
#include <MD5Builder.h>

namespace {
// Extract filename from path (everything after last '/')
std::string getFilename(const std::string& path) {
  const size_t pos = path.rfind('/');
  if (pos == std::string::npos) {
    return path;
  }
  return path.substr(pos + 1);
}
}  // namespace

std::string KOReaderDocumentId::calculateFromFilename(const std::string& filePath) {
  const std::string filename = getFilename(filePath);
  if (filename.empty()) {
    return "";
  }

  MD5Builder md5;
  md5.begin();
  md5.add(filename.c_str());
  md5.calculate();

  std::string result = md5.toString().c_str();
  LOG_DBG("KODoc", "Filename hash: %s (from '%s')", result.c_str(), filename.c_str());
  return result;
}

std::string KOReaderDocumentId::calculate(const std::string& filePath) {
  HalFile file;
  if (!Storage.openFileForRead("KODoc", filePath, file)) {
    LOG_DBG("KODoc", "Failed to open file: %s", filePath.c_str());
    return "";
  }

  const size_t fileSize = file.fileSize();
  LOG_DBG("KODoc", "Calculating full MD5 hash for file: %s (size: %zu)", filePath.c_str(), fileSize);

  // Initialize MD5 builder
  MD5Builder md5;
  md5.begin();

  // Buffer for reading chunks
  uint8_t buffer[CHUNK_SIZE];
  size_t totalBytesRead = 0;

  // Read sequentially in chunks to avoid high memory footprint
  while (true) {
    const size_t bytesRead = file.read(buffer, CHUNK_SIZE);
    if (bytesRead <= 0) {
      break;
    }
    md5.add(buffer, bytesRead);
    totalBytesRead += bytesRead;
  }

  // Calculate final hash
  md5.calculate();
  std::string result = md5.toString().c_str();

  LOG_DBG("KODoc", "Full hash calculated: %s (from %zu bytes)", result.c_str(), totalBytesRead);

  return result;
}
