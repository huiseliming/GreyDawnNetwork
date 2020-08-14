#include <gtest/gtest.h>
#include <filesystem>
#include <condition_variable>
#include <functional>
#include <fstream>
#include <gtest/gtest.h>
#include <mutex>
#include <utility/Utility.h>

using namespace GreyDawn;

struct ChangedCallbackHelper {
    bool change_detected_ = false;
    std::condition_variable change_condition_;
    std::mutex change_mutex_;

    std::function< void() > detected_notify_function_ = [this] {
        std::lock_guard< std::mutex > lock(change_mutex_);
        change_detected_ = true;
        change_condition_.notify_all();
    };

    bool AwaitChanged() {
        std::unique_lock< decltype(change_mutex_) > lock(change_mutex_);
        const auto changed = change_condition_.wait_for(
            lock,
            std::chrono::milliseconds(50),
            [this] { return change_detected_; }
        );
        change_detected_ = false;
        return changed;
    }
};

struct DirectoryMonitorTests
    : public ::testing::Test
{
    DirectoryMonitor directory_monitor_;
    ChangedCallbackHelper changed_callback_helper_;
    std::string outer_path_;
    std::string inner_path_;
    
    virtual void SetUp() {
        outer_path_ =GetExecuteFileDirectoryAbsolutePath() + "/TestArea";
        inner_path_ = outer_path_ + "/MonitorThis";
        std::filesystem::create_directory(outer_path_);
        std::filesystem::create_directory(inner_path_);
    }

    virtual void TearDown() {
        directory_monitor_.Stop();
        std::filesystem::remove_all(outer_path_);
    }
};

TEST_F(DirectoryMonitorTests, NoCallbackJustAfterStartingMonitor) {
    ASSERT_TRUE(directory_monitor_.Start(changed_callback_helper_.detected_notify_function_, inner_path_));
    ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
}

TEST_F(DirectoryMonitorTests, DirectoryMonitoring) {
    ASSERT_TRUE(directory_monitor_.Start(changed_callback_helper_.detected_notify_function_, inner_path_));
    // create file
    std::string test_file_path = inner_path_ + "/fred.txt";
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }

    // edit file
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file << "Hello, World\r\n";
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }

    // delete file
    {
        std::filesystem::remove(test_file_path);
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }

    // create file outside the monitored area.
    test_file_path = outer_path_ + "/fred.txt";
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
    }

    // edit file outside the monitored area.
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file << "Hello, World\r\n";
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
    }

    // delete the file outside the monitored area.
    {
        std::filesystem::remove(test_file_path);
        ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
    }
}

TEST_F(DirectoryMonitorTests, MoveDirectoryMonitor) {
    ASSERT_TRUE(directory_monitor_.Start(changed_callback_helper_.detected_notify_function_, inner_path_));

    // Move 
    DirectoryMonitor new_directory_monitor(std::move(directory_monitor_));
    directory_monitor_.Stop();
    directory_monitor_ = std::move(new_directory_monitor);
    new_directory_monitor.Stop();

    // create file
    std::string test_file_path = inner_path_ + "/fds.txt";
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }

    // edit file 
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file << "Hello, World\r\n";
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }

    // delete file
    {
        std::filesystem::remove(test_file_path);
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }

    // create file outside the monitored area.
    test_file_path = outer_path_ + "/fred.txt";
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
    }

    // edi file outside the monitored area.
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file << "Hello, World\r\n";
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
    }

    // delete file outside the monitored area.
    {
        std::filesystem::remove(test_file_path);
        ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
    }
}

TEST_F(DirectoryMonitorTests, Stop) {
    ASSERT_TRUE(directory_monitor_.Start(changed_callback_helper_.detected_notify_function_, inner_path_));

    // create file
    std::string test_file_path = inner_path_ + "/fred.txt";
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }

    // Stop monitor
    directory_monitor_.Stop();

    // edit file
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file << "Hello, World\r\n";
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
    }

    // delete file
    {
        std::filesystem::remove(test_file_path);
        ASSERT_FALSE(changed_callback_helper_.AwaitChanged());
    }
}

TEST_F(DirectoryMonitorTests, ChangeFileThatExistedBeforeMonitoringBegan) {
    // create file
    std::string test_file_path = inner_path_ + "/fred.txt";
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file.close();
    }

    ASSERT_TRUE(directory_monitor_.Start(changed_callback_helper_.detected_notify_function_, inner_path_));

    // edit file
    {
        std::fstream file(test_file_path, std::ios_base::out | std::ios_base::ate);
        ASSERT_FALSE(file.fail());
        file << "Hello, World\r\n";
        ASSERT_FALSE(file.fail());
        file.close();
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }

    // delete file
    {
        std::filesystem::remove(test_file_path);
        ASSERT_TRUE(changed_callback_helper_.AwaitChanged());
    }
}




