#include "FileSystem.hpp"

alarm_clock::memory::FileSystem::FileSystem() : sdmmcCard{nullptr}
{
    esp_vfs_fat_sdmmc_mount_config_t mountConfiguration = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = false,
    };

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = 40000;

    sdmmc_slot_config_t slotConfiguration = SDMMC_SLOT_CONFIG_DEFAULT();
    slotConfiguration.width = 4;
    slotConfiguration.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    auto result = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slotConfiguration, &mountConfiguration, &sdmmcCard);
    ESP_ERROR_CHECK(result);

    sdmmc_card_print_info(stdout, sdmmcCard);
}

alarm_clock::memory::FileSystem::~FileSystem()
{
    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, sdmmcCard);
    sdmmcCard = nullptr;

    ESP_LOGI(TAG, "Card unmounted");
}

std::unique_ptr<FILE, void (*)(FILE *)> alarm_clock::memory::FileSystem::OpenFile(const char *path)
{
    ESP_LOGI(TAG, "Reading file '%s'.", path);

    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return std::unique_ptr<FILE, void (*)(FILE *)>(nullptr, &CloseFile);
    }

    return std::unique_ptr<FILE, void (*)(FILE *)>(file, &CloseFile);
}

size_t alarm_clock::memory::FileSystem::Read(FILE *file, uint8_t *buffer, size_t count)
{
    return fread(buffer, 1, count, file);
}

void alarm_clock::memory::FileSystem::CloseFile(FILE *file)
{
    if (file)
    {
        ESP_LOGI(TAG, "File handle closed.");
        fclose(file);
    }
}
