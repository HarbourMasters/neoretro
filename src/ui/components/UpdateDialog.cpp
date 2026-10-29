#include "UpdateDialog.h"

#include "l10n/Strings.h"
#include "services/Updater.h"
#include "ui/clay/ClayUi.h"
#include "ui/components/Widgets.h"
#include "ui/theme/Colors.h"
#include "ui/theme/Theme.h"

#include <algorithm>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>

namespace UpdateDialog {
namespace {
constexpr size_t kMaxNotes = 600;

std::string Notes(const std::string& notes) {
    if (notes.size() <= kMaxNotes) {
        return notes;
    }
    return notes.substr(0, kMaxNotes) + "...";
}

void Title(ClayUi& ui, const std::string& text) {
    Widgets::Text(ui, text, TextSize::TitleLarge, RetroColors::White, FONT_MEDIUM);
}

void Body(ClayUi& ui, const std::string& text) {
    if (!text.empty()) {
        Widgets::Text(ui, text, TextSize::BodyMedium, RetroColors::WithOpacity(RetroColors::White, 0.8f));
    }
}

bool DialogButton(ClayUi& ui, const char* id, const char* label, bool primary = false) {
    const Clay_Sizing sizing = { CLAY_SIZING_FIT(90), CLAY_SIZING_FIXED(40) };
    return Widgets::Button(ui, id, label, sizing, true, primary ? Widgets::ElevatedStyle() : Widgets::OutlinedStyle());
}

void ProgressBar(ClayUi& ui, float progress) {
    CLAY({ .id = CLAY_ID("UpdateDialog_Progress"),
           .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(8) } },
           .backgroundColor = RetroColors::White12,
           .cornerRadius = CLAY_CORNER_RADIUS(4) }) {
        CLAY({ .layout = { .sizing = { CLAY_SIZING_PERCENT(std::clamp(progress, 0.0f, 1.0f)), CLAY_SIZING_GROW(0) } },
               .backgroundColor = RetroColors::TelegramBlue,
               .cornerRadius = CLAY_CORNER_RADIUS(4) }) {
        }
    }
}

void AvailableContent(ClayUi& ui) {
    const auto& release = Updater::Pending();
    Title(ui, fmt::format(fmt::runtime(Tr("updater_available")), release.version));
    Body(ui, Notes(release.notes));
    Body(ui, fmt::format(fmt::runtime(Tr("updater_youHave")), Updater::CurrentVersion()));
    const bool canInstall = Updater::CanInstall();
    if (!canInstall) {
        Body(ui, Tr("updater_manualInstall"));
    }

    const Clay_ElementId autoCheckId = CLAY_ID("UpdateDialog_AutoCheck");
    if (ui.Hovered(autoCheckId)) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    CLAY({ .id = autoCheckId, .layout = { .childGap = 8, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
        const bool autoCheck = Updater::AutoCheck();
        Widgets::Text(ui, autoCheck ? Icons::CheckBox : Icons::CheckBoxOutlineBlank, 22,
                      autoCheck ? RetroColors::TelegramBlue : RetroColors::Grey, FONT_ICONS);
        Widgets::Text(ui, Tr("updater_autoCheck"), TextSize::BodyMedium, RetroColors::White);
    }
    if (ui.Clicked(autoCheckId)) {
        Updater::SetAutoCheck(!Updater::AutoCheck());
    }

    CLAY({ .id = CLAY_ID("UpdateDialog_Buttons"), .layout = { .sizing = { CLAY_SIZING_GROW(0) }, .childGap = 10 } }) {
        if (DialogButton(ui, "UpdateDialog_Skip", Tr("updater_skip"))) {
            Updater::Skip();
        }
        Widgets::Spacer();
        if (DialogButton(ui, "UpdateDialog_Later", Tr("updater_later"))) {
            Updater::Later();
        }
        if (DialogButton(ui, "UpdateDialog_Install", canInstall ? Tr("updater_install") : Tr("updater_download"),
                         true)) {
            Updater::Install();
        }
    }
}

void MessageContent(ClayUi& ui, const char* title, bool offerDownload) {
    Title(ui, title);
    Body(ui, Updater::Message());
    CLAY({ .id = CLAY_ID("UpdateDialog_Buttons"),
           .layout = {
               .sizing = { CLAY_SIZING_GROW(0) }, .childGap = 10, .childAlignment = { .x = CLAY_ALIGN_X_RIGHT } } }) {
        if (offerDownload && DialogButton(ui, "UpdateDialog_Manual", Tr("updater_downloadManually"))) {
            Updater::OpenReleasesPage();
        }
        if (DialogButton(ui, "UpdateDialog_Close", Tr("updater_close"), true)) {
            Updater::Close();
        }
    }
}
} // namespace

void Layout(ClayUi& ui) {
    if (!Updater::DialogVisible()) {
        return;
    }
    const ImVec2 size = ImGui::GetMainViewport()->WorkSize;
    CLAY({ .id = CLAY_ID("UpdateDialog_Scrim"),
           .layout = { .sizing = { CLAY_SIZING_FIXED(size.x), CLAY_SIZING_FIXED(size.y) },
                       .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } },
           .backgroundColor = RetroColors::WithOpacity(RetroColors::Black, 0.6f),
           .floating = { .zIndex = 100, .attachTo = CLAY_ATTACH_TO_ROOT } }) {
        CLAY({ .id = CLAY_ID("UpdateDialog"),
               .layout = { .sizing = { CLAY_SIZING_FIXED(480) },
                           .padding = CLAY_PADDING_ALL(24),
                           .childGap = 14,
                           .layoutDirection = CLAY_TOP_TO_BOTTOM },
               .backgroundColor = RetroColors::Elephant,
               .cornerRadius = CLAY_CORNER_RADIUS(8) }) {
            switch (Updater::GetStatus()) {
                case Updater::Status::Checking:
                    CLAY({ .id = CLAY_ID("UpdateDialog_Checking"),
                           .layout = { .childGap = 12, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER } } }) {
                        Widgets::Spinner(ui, "UpdateDialog_Spinner", 20, RetroColors::TelegramBlue, 3);
                        Title(ui, Tr("updater_checking"));
                    }
                    break;
                case Updater::Status::Available:
                    AvailableContent(ui);
                    break;
                case Updater::Status::Downloading:
                    Title(ui, fmt::format(fmt::runtime(Tr("updater_downloading")), Updater::Pending().version));
                    ProgressBar(ui, Updater::Progress());
                    break;
                case Updater::Status::Installing:
                    Title(ui, Tr("updater_installing"));
                    break;
                case Updater::Status::UpToDate:
                    MessageContent(ui, Tr("updater_upToDate"), false);
                    break;
                case Updater::Status::Failed:
                    MessageContent(ui, Tr("updater_failed"), !Updater::Pending().version.empty());
                    break;
                default:
                    break;
            }
        }
    }
}
} // namespace UpdateDialog
