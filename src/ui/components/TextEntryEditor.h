#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>

struct TextEntry {
    std::string path;
    std::string name;
    std::string original;
    std::string text;
    std::string error;
    bool fieldsModified = false;

    bool Modified() const {
        return fieldsModified || text != original;
    }
};

struct TextEntryList {
    std::vector<TextEntry> entries;
    std::vector<int> filtered;
    int selected = -1;
    std::string query;

    void Filter();
    bool HasChanges() const;
};

struct TextEntryEditorOptions {
    std::string_view id;
    const char* hint = "";
    std::function<void(TextEntry& entry, int index)> validate;
    std::function<void(TextEntry& entry, int index)> revert;
    std::function<void(TextEntry& entry, int index)> drawFields;
    std::function<std::string(const std::string& text)> preview;
};

std::string TextEntryPreview(const std::string& text);

void TextEntryEditor(TextEntryList& list, const TextEntryEditorOptions& options, float width, float height);
