#include "settings.hpp"
#include <filesystem>
#include <fstream>
#include <glibmm/miscutils.h>

#ifdef WITH_KEYCHAIN
#include <keychain/keychain.h>
#endif

const std::string KeychainPackage = "com.github.uowuo.abaddon";
const std::string KeychainService = "abaddon-client-token";
const std::string KeychainUser = "";

SettingsManager::SettingsManager(const std::string &filename)
    : m_filename(filename) {
    if (!std::filesystem::exists(filename)) {
        std::fstream fs;
        fs.open(filename, std::ios::out);
        fs.close();
    }

    try {
        m_ok = m_file.load_from_file(m_filename, Glib::KEY_FILE_KEEP_COMMENTS);
    } catch (const Glib::Error &e) {
        fprintf(stderr, "error opening settings KeyFile: %s\n", e.what().c_str());
        m_ok = false;
    }

    DefineSettings();
    if (m_ok) ReadSettings();
}

<<<<<<< HEAD
void SettingsManager::ReadSettings() {
#define SMBOOL(section, key, var)                          \
    try {                                                  \
        m_settings.var = m_file.get_boolean(section, key); \
    } catch (...) {}
#define SMSTR(section, key, var)                          \
    try {                                                 \
        m_settings.var = m_file.get_string(section, key); \
    } catch (...) {}
#define SMINT(section, key, var)                           \
    try {                                                  \
        m_settings.var = m_file.get_integer(section, key); \
    } catch (...) {}
#define SMFLT(section, key, var)                          \
    try {                                                 \
        m_settings.var = m_file.get_double(section, key); \
    } catch (...) {}

    SMSTR("discord", "api_base", APIBaseURL);
    SMSTR("discord", "gateway", GatewayURL);
    SMBOOL("discord", "memory_db", UseMemoryDB);
    SMBOOL("discord", "prefetch", Prefetch);
    SMBOOL("discord", "autoconnect", Autoconnect);
    SMBOOL("gui", "auto_complete_emojis", AutoCompleteEmojis);
    SMSTR("gui", "css", MainCSS);
    SMBOOL("gui", "animated_guild_hover_only", AnimatedGuildHoverOnly);
    SMBOOL("gui", "animations", ShowAnimations);
    SMBOOL("gui", "custom_emojis", ShowCustomEmojis);
    SMBOOL("gui", "member_list_discriminator", ShowMemberListDiscriminators);
    SMBOOL("gui", "owner_crown", ShowOwnerCrown);
    SMBOOL("gui", "save_state", SaveState);
    SMBOOL("gui", "stock_emojis", ShowStockEmojis);
    SMBOOL("gui", "unreads", Unreads);
    SMBOOL("gui", "alt_menu", AltMenu);
    SMBOOL("gui", "hide_to_tray", HideToTray);
    SMBOOL("gui", "show_deleted_indicator", ShowDeletedIndicator);
    SMFLT("gui", "font_scale", FontScale);
    SMINT("http", "concurrent", CacheHTTPConcurrency);
    SMSTR("http", "user_agent", UserAgent);
    SMSTR("style", "expandercolor", ChannelsExpanderColor);
    SMSTR("style", "nsfwchannelcolor", NSFWChannelColor);
    SMSTR("style", "mentionbadgecolor", MentionBadgeColor);
    SMSTR("style", "mentionbadgetextcolor", MentionBadgeTextColor);
    SMSTR("style", "unreadcolor", UnreadIndicatorColor);
    SMBOOL("notifications", "enabled", NotificationsEnabled);
    SMBOOL("notifications", "playsound", NotificationsPlaySound);
    SMSTR("voice", "vad", VAD);
    SMBOOL("windows", "hideconsole", HideConsole);

=======
void SettingsManager::HandleReadToken() {
>>>>>>> bc9bb3d (refactor settings)
#ifdef WITH_KEYCHAIN
    // Move to keychain if present

    std::string token;
    try {
        token = m_file.get_string("discord", "token");
    } catch (...) {}

    if (!token.empty()) {
        keychain::Error error {};
        keychain::setPassword(KeychainPackage, KeychainService, KeychainUser, token, error);
        if (error) {
            spdlog::get("ui")->error("Keychain error setting token: {}", error.message);
        } else {
            m_file.remove_key("discord", "token");
        }
    }

    keychain::Error error {};
    m_settings.DiscordToken = keychain::getPassword(KeychainPackage, KeychainService, KeychainUser, error);
    if (error && error.type != keychain::ErrorType::NotFound) {
        spdlog::get("ui")->error("Keychain error reading token: {} ({})", error.message, error.code);
    }

#else
    AddSetting("discord", "token", "", &Settings::DiscordToken);
#endif
}

void SettingsManager::HandleWriteToken() {
#ifdef WITH_KEYCHAIN
    keychain::Error error {};

    keychain::setPassword(KeychainPackage, KeychainService, KeychainUser, m_settings.DiscordToken, error);
    if (error) {
        spdlog::get("ui")->error("Keychain error setting token: {}", error.message);
    }
#endif
    // else it will get enumerated over as part of definitions
}

void SettingsManager::DefineSettings() {
    using namespace std::string_literals;

    AddSetting("discord", "api_base", "https://discord.com/api/v9"s, &Settings::APIBaseURL);
    AddSetting("discord", "gateway", "wss://gateway.discord.gg/?v=9&encoding=json&compress=zlib-stream"s, &Settings::GatewayURL);
    AddSetting("discord", "memory_db", false, &Settings::UseMemoryDB);
    AddSetting("discord", "prefetch", false, &Settings::Prefetch);
    AddSetting("discord", "autoconnect", false, &Settings::Autoconnect);

    AddSetting("gui", "css", "main.css"s, &Settings::MainCSS);
    AddSetting("gui", "animated_guild_hover_only", true, &Settings::AnimatedGuildHoverOnly);
    AddSetting("gui", "animations", true, &Settings::ShowAnimations);
    AddSetting("gui", "custom_emojis", true, &Settings::ShowCustomEmojis);
    AddSetting("gui", "owner_crown", true, &Settings::ShowOwnerCrown);
    AddSetting("gui", "save_state", true, &Settings::SaveState);
    AddSetting("gui", "stock_emojis", false, &Settings::ShowStockEmojis);
    AddSetting("gui", "unreads", true, &Settings::Unreads);
    AddSetting("gui", "alt_menu", false, &Settings::AltMenu);
    AddSetting("gui", "hide_to_try", false, &Settings::HideToTray);
    AddSetting("gui", "show_deleted_indicator", true, &Settings::ShowDeletedIndicator);
    AddSetting("gui", "font_scale", -1.0, &Settings::FontScale);

    AddSetting("http", "concurrent", 20, &Settings::CacheHTTPConcurrency);
    AddSetting("http", "user_agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.87 Safari/537.36"s, &Settings::UserAgent);

    AddSetting("style", "expandercolor", "rgba(255, 83, 112, 0)"s, &Settings::ChannelsExpanderColor);
    AddSetting("style", "nsfwchannelcolor", "#970d0d"s, &Settings::NSFWChannelColor);
    AddSetting("style", "mentionbadgecolor", "rgba(184, 37, 37, 0)"s, &Settings::MentionBadgeColor);
    AddSetting("style", "mentionbadgetextcolor", "rgba(251, 251, 251, 0)"s, &Settings::MentionBadgeTextColor);
    AddSetting("style", "unreadcolor", "rgba(255, 255, 255, 0)"s, &Settings::UnreadIndicatorColor);

#ifdef _WIN32
    AddSetting("notifications", "enabled", false, &Settings::NotificationsEnabled);
    AddSetting("notifications", "playsound", false, &Settings::NotificationsPlaySound);

    AddSetting("windows", "hideconsole", false, &Settings::HideConsole);
#else
    AddSetting("notifications", "enabled", true, &Settings::NotificationsEnabled);
    AddSetting("notifications", "playsound", true, &Settings::NotificationsPlaySound);
#endif

#ifdef WITH_RNNOISE
    AddSetting("voice", "vad", "rnnoise"s, &Settings::VAD);
#else
    AddSetting("voice", "vad", "gate", &Settings::VAD);
#endif

    HandleReadToken();
}

void SettingsManager::ReadSettings() {
    for (auto &[k, setting] : m_definitions) {
        switch (setting.Type) {
            case SettingDefinition::TypeString:
                try {
                    m_settings.*(setting.Ptr.String) = m_file.get_string(setting.Section, setting.Name);
                } catch (...) {}
                break;
            case SettingDefinition::TypeBool:
                try {
                    m_settings.*(setting.Ptr.Bool) = m_file.get_boolean(setting.Section, setting.Name);
                } catch (...) {}
                break;
            case SettingDefinition::TypeDouble:
                try {
                    m_settings.*(setting.Ptr.Double) = m_file.get_double(setting.Section, setting.Name);
                } catch (...) {}
                break;
            case SettingDefinition::TypeInt:
                try {
                    m_settings.*(setting.Ptr.Int) = m_file.get_integer(setting.Section, setting.Name);
                } catch (...) {}
                break;
        }
    }

    m_read_settings = m_settings;
}

bool SettingsManager::IsValid() const {
    return m_ok;
}

SettingsManager::Settings &SettingsManager::GetSettings() {
    return m_settings;
}

void SettingsManager::Close() {
    if (m_ok) {
        for (auto &[k, setting] : m_definitions) {
            switch (setting.Type) {
                case SettingDefinition::TypeString:
                    if (m_settings.*(setting.Ptr.String) != m_read_settings.*(setting.Ptr.String)) {
                        m_file.set_string(setting.Section, setting.Name, m_settings.*(setting.Ptr.String));
                    }
                    break;
                case SettingDefinition::TypeBool:
                    if (m_settings.*(setting.Ptr.Bool) != m_read_settings.*(setting.Ptr.Bool)) {
                        m_file.set_boolean(setting.Section, setting.Name, m_settings.*(setting.Ptr.Bool));
                    }
                    break;
                case SettingDefinition::TypeDouble:
                    if (m_settings.*(setting.Ptr.Double) != m_read_settings.*(setting.Ptr.Double)) {
                        m_file.set_double(setting.Section, setting.Name, m_settings.*(setting.Ptr.Double));
                    }
                    break;
                case SettingDefinition::TypeInt:
                    if (m_settings.*(setting.Ptr.Int) != m_read_settings.*(setting.Ptr.Int)) {
                        m_file.set_integer(setting.Section, setting.Name, m_settings.*(setting.Ptr.Int));
                    }
                    break;
            }
        }

        HandleWriteToken();

        try {
            if (!m_file.save_to_file(m_filename)) {
                spdlog::get("ui")->error("Failed to save settings KeyFile");
            }
        } catch (const Glib::Error &e) {
            spdlog::get("ui")->error("Failed to save settings Keyfile: {}", e.what().c_str());
        }
    }
}