#ifndef TEST_TEMPORARY_DIRECTORY_H_
#define TEST_TEMPORARY_DIRECTORY_H_

#include <chrono>
#include <filesystem>
#include <string>

class TestTemporaryDirectory
{
public:
  TestTemporaryDirectory()
  {
    const auto unique = std::chrono::steady_clock::now().time_since_epoch().count();
    path_ = std::filesystem::temp_directory_path() /
            ("mpm-geomechanics-test-" + std::to_string(unique));
    std::filesystem::create_directories(path_);
  }

  ~TestTemporaryDirectory()
  {
    std::error_code error;
    std::filesystem::remove_all(path_, error);
  }

  const std::filesystem::path& path() const { return path_; }

private:
  std::filesystem::path path_;
};

#endif
