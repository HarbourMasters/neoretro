#pragma once

#include "image/Image.h"
#include "ui/PreviewTexture.h"

#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

struct ImageEntry {
    std::string path;
    std::string group;
    std::string name;
    std::string info;
    Image original;
    std::optional<Image> edited;
    std::string note;

    bool Modified() const {
        return edited.has_value();
    }
    const Image& Current() const {
        return edited ? *edited : original;
    }
};

struct ImageEntryList {
    std::vector<ImageEntry> entries;
    std::vector<int> filtered;
    int selected = -1;
    std::string query;
    std::set<std::string> expanded;

    void Filter();
    bool HasChanges() const;
};

struct ImageEntryEditorState {
    PreviewTexture texture;
    int shownIndex = -1;
    bool shownEdited = false;
    bool showOriginal = false;
    std::string error;
};

struct ImageEntryEditorOptions {
    std::string_view id;
    std::function<void(int index)> exportPng;
    std::function<void(int index)> importPng;
    std::function<void(int index)> revert;
};

void ImageEntryEditor(ImageEntryList& list, ImageEntryEditorState& state, const ImageEntryEditorOptions& options,
                      float width, float height);
