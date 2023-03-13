#pragma once

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "driver/sdmmc_host.h"
#include <memory>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

namespace alarm_clock
{
    namespace memory
    {
        constexpr static const char *MOUNT_POINT = "/sdcard0";
        constexpr static const char *TAG = "fs:sdcard";

        typedef std::unique_ptr<FILE, void (*)(FILE *)> FileHandle;

        class FileSystem
        {
            sdmmc_card_t *sdmmcCard;

        public:
            FileSystem();
            ~FileSystem();

            FileHandle OpenFile(const char *path);
            size_t Read(FILE *file, uint8_t *buffer, size_t count);

        private:
            static void CloseFile(FILE *);
        };
    }
}