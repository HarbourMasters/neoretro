#include "games/bk64/Bk64Conventions.h"
#include "games/pm64/Pm64Font.h"
#include "games/pm64/Pm64Images.h"
#include "games/pm64/Pm64Text.h"
#include "games/sm64/Sm64Text.h"
#include "image/Image.h"
#include "otr/Archive.h"
#include "otr/Exporters.h"
#include "otr/Resource.h"
#include "otr/Texture.h"
#include "services/FontGenerator.h"
#include "services/OtrGenerator.h"
#include "services/TextureBuilder.h"
#include "services/TextureReplacement.h"
#include "services/UpdateFeed.h"
#include "utils/Hash.h"
#include "utils/Paths.h"
#include "utils/ThreadPool.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace {
int sFailures = 0;
int sChecks = 0;

#define CHECK(cond)                                                                \
    do {                                                                           \
        sChecks++;                                                                 \
        if (!(cond)) {                                                             \
            sFailures++;                                                           \
            std::fprintf(stderr, "  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        }                                                                          \
    } while (0)

struct TestCase {
    const char* name;
    std::function<void()> fn;
};

uint32_t U32(const std::vector<char>& data, size_t at) {
    uint32_t v;
    std::memcpy(&v, data.data() + at, 4);
    return v;
}

std::vector<uint8_t> Bytes(const std::vector<char>& data) {
    return std::vector<uint8_t>(data.begin(), data.end());
}

fs::path TempDir(const std::string& name) {
    const fs::path dir = fs::temp_directory_path() / ("neoretro-tests-" + name);
    fs::remove_all(dir);
    fs::create_directories(dir);
    return dir;
}

Image Gradient(int w, int h) {
    Image image(w, h, 4);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint8_t* p = image.Pixel(x, y);
            p[0] = (uint8_t)(x * 255 / std::max(1, w - 1));
            p[1] = (uint8_t)(y * 255 / std::max(1, h - 1));
            p[2] = (uint8_t)((x + y) * 7);
            p[3] = (x + y) % 3 == 0 ? 0 : 255;
        }
    }
    return image;
}

Image Palettized(int w, int h, int colors) {
    Image image(w, h, 4);
    image.hasPalette = true;
    image.indices.resize((size_t)w * h);
    for (int i = 0; i < colors; i++) {
        image.SetPaletteColor(i, (uint8_t)(i * 16), (uint8_t)(i * 16), (uint8_t)(i * 16), 255);
    }
    for (size_t i = 0; i < image.indices.size(); i++) {
        image.indices[i] = (uint8_t)(i % colors);
    }
    image.ResolvePalette();
    return image;
}

Otr::Texture MakeTexture(Otr::TextureType type, const Image& image) {
    Otr::Texture texture;
    texture.type = type;
    Otr::TextureCodec::FromImage(texture, image);
    return texture;
}

void TestHeaderLayout() {
    Otr::Texture texture = MakeTexture(Otr::TextureType::RGBA32bpp, Gradient(4, 2));
    const auto bytes = Otr::ExportTexture(texture, "test");
    CHECK(bytes.size() == 0x40 + 16 + 4 * 2 * 4);
    CHECK(bytes[0] == 0);
    CHECK(U32(bytes, 0x04) == (uint32_t)Otr::ResourceType::Texture);
    CHECK(U32(bytes, 0x08) == 0);
    CHECK(U32(bytes, 0x40) == (uint32_t)Otr::TextureType::RGBA32bpp);
    CHECK(U32(bytes, 0x44) == 4);
    CHECK(U32(bytes, 0x48) == 2);
    CHECK(U32(bytes, 0x4C) == 32);

    texture.SetFlags(Otr::kTextureFlagLoadAsRaw);
    texture.SetScale(2.0f, 3.0f);
    const auto roy = Otr::ExportTexture(texture, "test");
    CHECK(U32(roy, 0x08) == 1);
    CHECK(U32(roy, 0x4C) == Otr::kTextureFlagLoadAsRaw);
    float h;
    float v;
    std::memcpy(&h, roy.data() + 0x50, 4);
    std::memcpy(&v, roy.data() + 0x54, 4);
    CHECK(h == 2.0f && v == 3.0f);

    const auto read = Otr::Texture::Read(Bytes(roy));
    CHECK(read.has_value());
    CHECK(read && read->version == Otr::Version::Roy && read->flags == 1 && read->hByteScale == 2.0f);
    CHECK(read && read->data == texture.data);
}

void TestCodecRoundTrips() {
    {
        const Image source = Gradient(8, 4);
        const auto image = Otr::TextureCodec::ToImage(MakeTexture(Otr::TextureType::RGBA32bpp, source));
        CHECK(image && image->rgba == source.rgba);
    }
    {
        const Image source = Palettized(8, 4, 16);
        const auto texture = MakeTexture(Otr::TextureType::Palette4bpp, source);
        CHECK(texture.data.size() == 16);
        const auto image = Otr::TextureCodec::ToImage(texture);
        CHECK(image && image->hasPalette && image->indices == source.indices);
    }
    {
        const Image source = Palettized(8, 4, 200);
        const auto image = Otr::TextureCodec::ToImage(MakeTexture(Otr::TextureType::Palette8bpp, source));
        CHECK(image && image->indices == source.indices);
    }
    for (const auto type :
         { Otr::TextureType::RGBA16bpp, Otr::TextureType::GrayscaleAlpha16bpp, Otr::TextureType::GrayscaleAlpha8bpp,
           Otr::TextureType::GrayscaleAlpha4bpp, Otr::TextureType::Grayscale8bpp, Otr::TextureType::Grayscale4bpp }) {
        Image source = Gradient(8, 4);
        if (type != Otr::TextureType::RGBA16bpp) {
            for (size_t i = 0; i < source.rgba.size(); i += 4) {
                source.rgba[i + 1] = source.rgba[i + 2] = source.rgba[i];
            }
        }
        const auto texture = MakeTexture(type, source);
        CHECK(texture.data.size() == Otr::BufferSize(type, 8, 4));
        const auto image = Otr::TextureCodec::ToImage(texture);
        CHECK(image.has_value());
        if (!image) {
            continue;
        }
        const int tolerance = 256 / (1 << std::min(Otr::BitSize(type) == 32 ? 8 : 5, 8)) + 36;
        int worst = 0;
        for (size_t i = 0; i < source.rgba.size(); i += 4) {
            worst = std::max(worst, std::abs(image->rgba[i] - source.rgba[i]));
        }
        CHECK(worst <= tolerance);
    }
    {
        Image lut(16, 1, 4);
        for (int i = 0; i < 16; i++) {
            uint8_t* p = lut.Pixel(i, 0);
            p[0] = 255;
            p[1] = (uint8_t)i;
            p[2] = 0;
            p[3] = 255;
        }
        const auto tlut = MakeTexture(Otr::TextureType::RGBA32bpp, lut);
        const auto image =
            Otr::TextureCodec::ToImage(MakeTexture(Otr::TextureType::Palette4bpp, Palettized(4, 4, 16)), &tlut);
        CHECK(image && image->Pixel(3, 0)[0] == 255 && image->Pixel(3, 0)[1] == 3);
    }
}

void TestPngPalettePreserved() {
    const Image source = Palettized(6, 3, 12);
    const auto decoded = ImageIO::DecodePng(ImageIO::EncodePng(source));
    CHECK(decoded && decoded->hasPalette && decoded->indices == source.indices);
    CHECK(decoded && decoded->channels == 3);

    Image rgb(2, 2, 3);
    const auto rgbDecoded = ImageIO::DecodePng(ImageIO::EncodePng(rgb));
    CHECK(rgbDecoded && !rgbDecoded->hasPalette && rgbDecoded->channels == 3);
}

void TestSequence() {
    const std::vector<uint8_t> seq = { 1, 2, 3, 4, 5 };
    const auto sequence = Otr::Sequence::FromSeqFile(seq, "Title/Theme\r\n0x2A\r\nfanfare\r\n");
    CHECK(sequence.has_value());
    if (!sequence) {
        return;
    }
    CHECK(sequence->path == "Title|Theme_fanfare");
    CHECK(sequence->cachePolicy == 1 && sequence->medium == 2 && sequence->fontIndices == std::vector<uint8_t>{ 0x2A });

    const auto bytes = Otr::ExportSequence(*sequence);
    CHECK(U32(bytes, 0x04) == (uint32_t)Otr::ResourceType::OoTAudioSequence);
    CHECK(U32(bytes, 0x08) == 2);
    CHECK(U32(bytes, 0x40) == 5);
    CHECK(bytes.size() == 0x40 + 4 + 5 + 3 + 4 + 1);
    CHECK((uint8_t)bytes[0x49] == 0 && (uint8_t)bytes[0x4A] == 2 && (uint8_t)bytes[0x4B] == 1);
    CHECK(U32(bytes, 0x4C) == 1 && (uint8_t)bytes[0x50] == 0x2A);

    const auto bgm = Otr::Sequence::FromSeqFile(seq, "Field\n1f");
    CHECK(bgm && bgm->path == "Field_bgm" && bgm->cachePolicy == 2);
}

void TestArchives() {
    const fs::path dir = TempDir("archives");
    for (const char* ext : { ".o2r", ".otr" }) {
        const std::string path = (dir / (std::string("test") + ext)).string();
        {
            Otr::ArchiveWriter writer(path);
            CHECK(writer.IsOpen());
            CHECK(writer.Add("textures/a", { 'a', 'b', 'c' }));
            std::vector<char> big(100000, 'x');
            CHECK(writer.Add("dir/b", big));
            CHECK(writer.Close());
        }
        auto reader = Otr::ArchiveReader::Open(path);
        CHECK(reader != nullptr);
        if (!reader) {
            continue;
        }
        auto names = reader->List();
        std::sort(names.begin(), names.end());
        CHECK((names == std::vector<std::string>{ "dir/b", "textures/a" }));
        const auto a = reader->Read("textures/a");
        CHECK(a && *a == std::vector<uint8_t>({ 'a', 'b', 'c' }));
        const auto b = reader->Read("dir/b");
        CHECK(b && b->size() == 100000 && (*b)[99999] == 'x');
    }
}

TextureManifestEntry Entry(Otr::TextureType type, int w, int h) {
    TextureManifestEntry entry;
    entry.textureType = type;
    entry.textureWidth = w;
    entry.textureHeight = h;
    return entry;
}

std::optional<Otr::Texture> BuildTexture(const Image& image, const TextureManifestEntry& entry) {
    const auto bytes = TextureBuilder::BuildFromImage(image, entry, "test");
    return bytes ? Otr::Texture::Read(Bytes(*bytes)) : std::nullopt;
}

void TestTextureBuilderRules() {
    {
        const auto texture = BuildTexture(Palettized(8, 8, 16), Entry(Otr::TextureType::Palette4bpp, 8, 8));
        CHECK(texture && texture->type == Otr::TextureType::Palette4bpp && texture->version == Otr::Version::Deckard);
    }
    {
        const auto texture = BuildTexture(Gradient(8, 8), Entry(Otr::TextureType::Palette8bpp, 8, 8));
        CHECK(texture && texture->type == Otr::TextureType::RGBA32bpp && texture->flags == Otr::kTextureFlagLoadAsRaw);
    }
    {
        const auto texture = BuildTexture(Gradient(16, 16), Entry(Otr::TextureType::RGBA16bpp, 8, 8));
        CHECK(texture && texture->type == Otr::TextureType::RGBA32bpp);
        CHECK(texture && texture->hByteScale == 4.0f && texture->vPixelScale == 2.0f);
        CHECK(texture && texture->width == 16 && texture->height == 16);
    }
    {
        const auto texture = BuildTexture(Gradient(17, 15), Entry(Otr::TextureType::RGBA32bpp, 8, 8));
        CHECK(texture && texture->width == 16 && texture->height == 16);
    }
    {
        const auto texture = BuildTexture(Gradient(8, 8), Entry(Otr::TextureType::GrayscaleAlpha8bpp, 8, 8));
        CHECK(texture && texture->type == Otr::TextureType::RGBA32bpp && texture->flags == Otr::kTextureFlagLoadAsRaw);
    }
    {
        Image grey = Gradient(8, 8);
        for (size_t i = 0; i < grey.rgba.size(); i += 4) {
            grey.rgba[i + 1] = grey.rgba[i + 2] = grey.rgba[i];
        }
        grey = ImageOps::Convert(grey, 3);
        const auto texture = BuildTexture(grey, Entry(Otr::TextureType::Grayscale8bpp, 8, 8));
        CHECK(texture && texture->type == Otr::TextureType::Grayscale8bpp && texture->flags == 0);
    }
    {
        auto entry = Entry(Otr::TextureType::Grayscale4bpp, 6, 8);
        entry.kind = TextureEntryKind::AdditiveFontGlyph;
        const auto texture = BuildTexture(Gradient(12, 16), entry);
        CHECK(texture && texture->width == 16 && texture->height == 16 && texture->hByteScale == 2.0f);
    }
}

void TestBk64Conventions() {
    const nlohmann::json manifest = {
        { "sprite/foo_1_2", { { "hash", "x" }, { "textureType", 2 }, { "textureWidth", 16 }, { "textureHeight", 8 } } },
    };
    Bk64TextureConventions bk;
    const auto color = bk.ResolveAdditiveEntry(manifest, "sprite/foo_1_2_BLUE");
    CHECK(color && color->kind == TextureEntryKind::Additive && color->textureWidth == 16);
    const auto bold = bk.ResolveAdditiveEntry(manifest, "boldfont/foo_1_2_A0");
    CHECK(bold && bold->kind == TextureEntryKind::AdditiveFontGlyph);
    CHECK(!bk.ResolveAdditiveEntry(manifest, "sprite/unknown"));
    const nlohmann::json frames = {
        { "anim/door_tex_2",
          { { "hash", "x" }, { "textureType", 1 }, { "textureWidth", 32 }, { "textureHeight", 32 } } },
    };
    const auto frame = bk.ResolveAdditiveEntry(frames, "anim/door_tex_2_frame_5");
    CHECK(frame && frame->kind == TextureEntryKind::Additive && frame->textureWidth == 32);
    CHECK(!bk.ResolveAdditiveEntry(frames, "anim/door_tex_2_frame_"));
    CHECK(TextureReplacement::BandIndexOf("bg/sky#2") == 2);
    CHECK(!TextureReplacement::BandIndexOf("bg/sky#"));
    CHECK(!TextureReplacement::BandIndexOf("bg/sky"));
}

void TestReplaceTexturesFlow() {
    const fs::path dir = TempDir("replace");
    const std::string gameArchive = (dir / "game.o2r").string();
    {
        Otr::ArchiveWriter writer(gameArchive);
        writer.Add("textures/a/ci",
                   Otr::ExportTexture(MakeTexture(Otr::TextureType::Palette4bpp, Palettized(8, 8, 16)), "ci"));
        writer.Add("textures/a/rgba",
                   Otr::ExportTexture(MakeTexture(Otr::TextureType::RGBA16bpp, Gradient(8, 8)), "rgba"));
        writer.Add("misc/blob", { 'n', 'o', 't', 'r', 'e', 's' });
        writer.Close();
    }

    const auto extracted = TextureReplacement::ProcessOtr({ gameArchive }, dir / "out");
    CHECK(extracted && extracted->size() == 2);
    const fs::path folder = dir / "out" / "game";
    CHECK(fs::exists(folder / "textures/a/ci.png") && fs::exists(folder / "textures/a/rgba.png"));
    if (!extracted) {
        return;
    }
    auto entries = *extracted;
    const auto font = TextureReplacement::DumpFont(folder, NEORETRO_ASSETS_DIR);
    CHECK(font && font->textureWidth == 64 && font->textureHeight == 256);
    if (font) {
        entries[TextureReplacement::kFontTextureName] = *font;
    }
    CHECK(TextureReplacement::WriteManifest(folder / "manifest.json", entries));

    auto staged = TextureReplacement::ProcessFolder(folder);
    CHECK(staged && staged->empty());

    auto ci = ImageIO::DecodePng(*Paths::ReadFile(folder / "textures/a/ci.png"));
    CHECK(ci && ci->hasPalette);
    ci->indices[0] = 15;
    ci->ResolvePalette();
    Paths::WriteFile(folder / "textures/a/ci.png", ImageIO::EncodePng(*ci));
    staged = TextureReplacement::ProcessFolder(folder);
    CHECK(staged && staged->size() == 1 && staged->count("textures/a") && staged->at("textures/a").size() == 1);
    if (!staged || staged->empty()) {
        return;
    }

    std::map<std::string, StageEntry> stage;
    for (const auto& [key, pairs] : *staged) {
        stage[key] = CustomTexturesEntry{ pairs };
    }
    const std::string output = (dir / "mod.o2r").string();
    std::atomic<int> processed = 0;
    std::vector<std::string> errors;
    CHECK(OtrGenerator::Generate(stage, output, true, processed, [&](const std::string& e) { errors.push_back(e); }));
    CHECK(errors.empty() && processed == 1);

    auto reader = Otr::ArchiveReader::Open(output);
    CHECK(reader && reader->List() == std::vector<std::string>{ "alt/textures/a/ci" });
    const auto data = reader ? reader->Read("alt/textures/a/ci") : std::nullopt;
    const auto texture = data ? Otr::Texture::Read(*data) : std::nullopt;
    CHECK(texture && texture->type == Otr::TextureType::Palette4bpp && texture->width == 8);
    CHECK(texture && (texture->data[0] >> 4) == 15);
}

void TestGenerateCustomAndSequences() {
    const fs::path dir = TempDir("generate");
    Paths::WriteText(dir / "custom/root.txt", "root");
    Paths::WriteText(dir / "custom/sub/file.bin", "data");
    Paths::WriteFile(dir / "music/song.seq", { 9, 9, 9 });
    Paths::WriteText(dir / "music/song.meta", "Song\n3\n");

    std::map<std::string, StageEntry> stage;
    stage["."] = CustomStageEntry{ { dir / "custom/root.txt" } };
    stage["sub"] = CustomStageEntry{ { dir / "custom/sub/file.bin" } };
    stage["custom/music"] = CustomSequencesEntry{ { { dir / "music/song.seq", dir / "music/song.meta" } } };

    const std::string output = (dir / "out.otr").string();
    std::atomic<int> processed = 0;
    CHECK(OtrGenerator::Generate(stage, output, false, processed, [](const std::string&) {}));
    CHECK(processed == 3);
    auto reader = Otr::ArchiveReader::Open(output);
    auto names = reader ? reader->List() : std::vector<std::string>{};
    std::sort(names.begin(), names.end());
    CHECK((names == std::vector<std::string>{ "custom/music/Song_bgm", "root.txt", "sub/file.bin" }));
    const auto seq = reader ? reader->Read("custom/music/Song_bgm") : std::nullopt;
    const auto header = seq ? Otr::ReadHeader(*seq) : std::nullopt;
    CHECK(header && header->type == (uint32_t)Otr::ResourceType::OoTAudioSequence);
}

void TestSm64Text() {
    const std::vector<uint8_t> hello = { 0x11, 0x28, 0x2F, 0x2F, 0x32, 0x9E, 0xD2, 0xF2, 0xFE, 0x54, 0xFA, 0x7B, 0xFF };
    const std::string decoded = Sm64::DecodeText(hello);
    CHECK(decoded == "Hello {you}!\n[A]{star}{7B}");
    CHECK(Sm64::EncodeText(decoded) == hello);
    CHECK(Sm64::EncodeText("\xe2\x98\x85{mario}") == (std::vector<uint8_t>{ 0xFA, 0x40, 0x41, 0xFF }));
    CHECK(Sm64::EncodeText("a\r\nb") == (std::vector<uint8_t>{ 0x24, 0xFE, 0x25, 0xFF }));
    std::string error;
    CHECK(!Sm64::EncodeText("bad #", &error));
    CHECK(error.find('#') != std::string::npos);

    Sm64::Dialog dialog;
    dialog.linesPerBox = 6;
    dialog.leftOffset = 95;
    dialog.width = 150;
    dialog.text = hello;
    const auto dialogData = Sm64::ExportDialog(dialog, "texts/dialogs/dialog_00");
    const auto parsedDialog = Sm64::ParseDialog(std::vector<uint8_t>(dialogData.begin(), dialogData.end()));
    CHECK(parsedDialog && parsedDialog->linesPerBox == 6 && parsedDialog->leftOffset == 95 &&
          parsedDialog->width == 150 && parsedDialog->text == hello);

    const auto textData = Sm64::ExportText(hello, "texts/courses/course_00");
    CHECK(Sm64::ParseText(std::vector<uint8_t>(textData.begin(), textData.end())) == hello);

    const Sm64::Dictionary dictionary = { { "TEXT_A", { 0x0A, 0xFF } }, { "TEXT_HELLO", hello } };
    const auto dictionaryData = Sm64::ExportDictionary(dictionary, "texts/strings/global");
    CHECK(Sm64::ParseDictionary(std::vector<uint8_t>(dictionaryData.begin(), dictionaryData.end())) == dictionary);
    CHECK(!Sm64::ParseDialog(std::vector<uint8_t>(textData.begin(), textData.end())));

    const fs::path dir = TempDir("sm64");
    std::map<std::string, StageEntry> stage;
    stage["texts/dialogs"] = ResourceDataEntry{ { { "dialog_00", dialogData } } };
    stage["texts/strings"] = ResourceDataEntry{ { { "global", dictionaryData } } };
    const std::string output = (dir / "text.o2r").string();
    std::atomic<int> processed = 0;
    CHECK(OtrGenerator::Generate(stage, output, true, processed, [](const std::string&) {}));
    CHECK(processed == 2);
    auto reader = Otr::ArchiveReader::Open(output);
    const auto assets = reader ? Sm64::LoadTextAssets(*reader) : Sm64::TextAssets{};
    CHECK(assets.dialogs.size() == 1 && assets.dialogs.count("texts/dialogs/dialog_00"));
    CHECK(assets.strings == dictionary);
}

void TestRealSm64Archive() {
    const char* path = std::getenv("NEORETRO_TEST_SM64_ARCHIVE");
    if (path == nullptr) {
        std::printf("  (skipped: set NEORETRO_TEST_SM64_ARCHIVE)\n");
        return;
    }
    auto reader = Otr::ArchiveReader::Open(path);
    CHECK(reader != nullptr);
    if (!reader) {
        return;
    }
    const auto start = std::chrono::steady_clock::now();
    const auto assets = Sm64::LoadTextAssets(*reader);
    const auto seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    std::printf("  %zu dialogs, %zu texts, %zu strings in %.2fs\n", assets.dialogs.size(), assets.texts.size(),
                assets.strings ? assets.strings->size() : 0, seconds);
    CHECK(!assets.dialogs.empty() && !assets.texts.empty() && assets.strings);
    size_t mismatches = 0;
    const auto roundTrip = [&](const std::string& name, const std::vector<uint8_t>& bytes) {
        const auto end = std::find(bytes.begin(), bytes.end(), Sm64::kTerminator);
        const std::vector<uint8_t> expected(bytes.begin(), end == bytes.end() ? end : end + 1);
        if (Sm64::EncodeText(Sm64::DecodeText(bytes)) != expected) {
            mismatches++;
            std::printf("    mismatch: %s\n", name.c_str());
        }
    };
    for (const auto& [name, dialog] : assets.dialogs) {
        roundTrip(name, dialog.text);
    }
    for (const auto& [name, text] : assets.texts) {
        roundTrip(name, text);
    }
    if (assets.strings) {
        for (const auto& [key, value] : *assets.strings) {
            roundTrip(key, value);
        }
    }
    CHECK(mismatches == 0);
}

void TestUpdateFeed() {
    CHECK(UpdateFeed::IsNewer("1.10.0", "1.9.9"));
    CHECK(UpdateFeed::IsNewer("0.2", "0.1.9"));
    CHECK(!UpdateFeed::IsNewer("1.0", "1.0.0"));
    CHECK(!UpdateFeed::IsNewer("0.9.9", "1.0.0"));
    CHECK(UpdateFeed::IsValidVersion("1.2.3"));
    CHECK(!UpdateFeed::IsValidVersion("1..2"));
    CHECK(!UpdateFeed::IsValidVersion("v1.2"));
    CHECK(!UpdateFeed::IsValidVersion(""));

    const std::string sha(64, 'A');
    const std::string feed = R"({"version": "0.2.0", "notes": "Fixes",
        "assets": {"linux-x64": {"url": "https://example.com/neoretro.AppImage", "sha256": ")" +
                             sha + R"("},
                   "windows-x64": {"url": "http://example.com/neoretro.zip", "sha256": ")" +
                             sha + R"("}}})";
    const auto release = UpdateFeed::Parse(feed, "linux-x64");
    CHECK(release && release->version == "0.2.0" && release->notes == "Fixes" &&
          release->sha256 == std::string(64, 'a'));
    std::string error;
    CHECK(!UpdateFeed::Parse(feed, "windows-x64", &error));
    CHECK(!error.empty());
    CHECK(!UpdateFeed::Parse(feed, "mac-universal"));
    CHECK(!UpdateFeed::Parse("not json", "linux-x64"));
    CHECK(!UpdateFeed::Parse(R"({"version": 2, "assets": {}})", "linux-x64"));
    CHECK(std::string(UpdateFeed::PlatformKey()).find('-') != std::string::npos);
}

void TestPm64Text() {
    const std::vector<uint8_t> hello = { 0xFC, 0x01, 0x28, 0x49, 0x0C, 0xF7, 0xFF, 0x05, 0x0A, 0x41, 0x98,
                                         0xF7, 0xFF, 0x24, 0xFF, 0x05, 0x10, 0x98, 0xFF, 0x25, 0x01, 0xF0,
                                         0x3B, 0xFB, 0xF2, 0x1E, 0xFF, 0x00, 0x01, 0xFE, 0xFD };
    const std::string decoded = Pm64::DecodeText(hello);
    CHECK(decoded == "[Style right]\nHi, [Color 0x0A]a[~A] [A]!\n\\[[Next]\n[Pause 30][Font menu]\n[Raw 0xFE]");
    CHECK(Pm64::EncodeText(decoded) == hello);
    CHECK(Pm64::EncodeText("[style choice pos=1,2 size=0x10,4]x\r\ny") ==
          (std::vector<uint8_t>{ 0xFC, 0x05, 1, 2, 0x10, 4, 0x58, 0xF0, 0x59, 0xFD }));
    CHECK(Pm64::EncodeText("[ItemIcon 0x0123][SetPosX 300]") ==
          (std::vector<uint8_t>{ 0xFF, 0x17, 0x01, 0x23, 0xFF, 0x10, 0x01, 0x2C, 0xFD }));
    CHECK(Pm64::DecodeText({ 0xFB, 0xF0, 0xFD }) == "[Next]\n\n");
    CHECK(Pm64::EncodeText("[Next]\n\n") == (std::vector<uint8_t>{ 0xFB, 0xF0, 0xFD }));
    CHECK(Pm64::DecodeText({ 0xFF, 0x05 }) == "[Func 0x05]");
    CHECK(Pm64::DecodeText({ 0xFF }) == "[Raw 0xFF]");
    std::string error;
    CHECK(!Pm64::EncodeText("bad [", Pm64::Charset::Standard, &error) && error.find("Unclosed") != std::string::npos);
    CHECK(!Pm64::EncodeText("[Bogus 1]", Pm64::Charset::Standard, &error) && error.find("Bogus") != std::string::npos);
    CHECK(!Pm64::EncodeText("[Pause 256]", Pm64::Charset::Standard, &error));
    CHECK(Pm64::EncodeText("[Pause 010][Pause 08]") == (std::vector<uint8_t>{ 0xF2, 10, 0xF2, 8, 0xFD }));
    CHECK(!Pm64::EncodeText("[Pause 0x]") && !Pm64::EncodeText("[Pause +5]") && !Pm64::EncodeText("[Pause -1]"));
    CHECK(!Pm64::EncodeText("\xe2\x98\x85", Pm64::Charset::Standard, &error));

    CHECK(Pm64::CharsetFor("messages/MSG_Credits_0001") == Pm64::Charset::Credits);
    CHECK(Pm64::CharsetFor("messages/MSG_Intro_0001") == Pm64::Charset::Standard);
    CHECK(Pm64::EncodeText("MARIO \xc2\xa9 1", Pm64::Charset::Credits) ==
          (std::vector<uint8_t>{ 0x0C, 0x00, 0x11, 0x08, 0x0E, 0xF7, 0x27, 0xF7, 0x1E, 0xFD }));
    CHECK(!Pm64::EncodeText("mario", Pm64::Charset::Credits));

    const auto data = Pm64::ExportMessage(hello, "messages/MSG_Test_0001");
    const std::vector<uint8_t> resource(data.begin(), data.end());
    const auto header = Otr::ReadHeader(resource);
    CHECK(header && header->type == (uint32_t)Otr::ResourceType::Blob);
    CHECK(resource.size() == 64 + 4 + 32);
    CHECK(Pm64::ParseMessage(resource) == hello);
    const std::vector<uint8_t> terminatorArg = { 0xF2, 0xFD, 0x28, 0x49, 0xFD };
    CHECK(Pm64::DecodeText(terminatorArg) == "[Pause 253]Hi");
    const auto terminatorData = Pm64::ExportMessage(terminatorArg, "messages/MSG_Test_0002");
    CHECK(Pm64::ParseMessage(std::vector<uint8_t>(terminatorData.begin(), terminatorData.end())) == terminatorArg);

    const fs::path dir = TempDir("pm64");
    std::map<std::string, StageEntry> stage;
    stage["messages"] = ResourceDataEntry{ { { "MSG_Test_0001", data } } };
    const std::string output = (dir / "text.o2r").string();
    std::atomic<int> processed = 0;
    CHECK(OtrGenerator::Generate(stage, output, true, processed, [](const std::string&) {}));
    CHECK(processed == 1);
    auto reader = Otr::ArchiveReader::Open(output);
    const auto messages = reader ? Pm64::LoadMessages(*reader) : Pm64::Messages{};
    CHECK(messages.size() == 1 && messages.count("messages/MSG_Test_0001") &&
          messages.at("messages/MSG_Test_0001") == hello);
}

void TestRealPm64Archive() {
    const char* path = std::getenv("NEORETRO_TEST_PM64_ARCHIVE");
    if (path == nullptr) {
        std::printf("  (skipped: set NEORETRO_TEST_PM64_ARCHIVE)\n");
        return;
    }
    auto reader = Otr::ArchiveReader::Open(path);
    CHECK(reader != nullptr);
    if (!reader) {
        return;
    }
    const auto start = std::chrono::steady_clock::now();
    const auto messages = Pm64::LoadMessages(*reader);
    const auto seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    size_t mismatches = 0;
    size_t raw = 0;
    for (const auto& [name, bytes] : messages) {
        const auto charset = Pm64::CharsetFor(name);
        const std::string text = Pm64::DecodeText(bytes, charset);
        raw += text.find("[Raw ") != std::string::npos;
        if (Pm64::EncodeText(text, charset) != bytes) {
            if (mismatches++ < 10) {
                std::printf("    mismatch: %s\n", name.c_str());
            }
        }
    }
    std::printf("  %zu messages in %.2fs, %zu with raw bytes\n", messages.size(), seconds, raw);
    CHECK(!messages.empty());
    CHECK(mismatches == 0);

    const auto font = Pm64::LoadFont(*reader);
    CHECK(font.glyphs.size() == 166 && !font.palettes.empty());
    if (const auto it = messages.find("messages/MSG_CH0_0001"); it != messages.end() && !font.Empty()) {
        const Image text = Pm64::RenderText(font, it->second);
        CHECK(text.width > 100 && text.height >= 3 * Pm64::kGlyphSize);
    }

    for (const auto kind : { Pm64::ImageKind::Sprites, Pm64::ImageKind::MapTextures, Pm64::ImageKind::Pictures }) {
        const auto assets = Pm64::LoadImages(*reader, kind);
        size_t failures = 0;
        for (const auto& asset : assets) {
            const auto image = Pm64::DecodeImage(asset);
            const auto encoded = image ? Pm64::EncodeImage(asset, ImageOps::Convert(*image, 4)) : std::nullopt;
            if (!encoded || encoded->preview.rgba != ImageOps::Convert(*image, 4).rgba) {
                if (failures++ < 5) {
                    std::printf("    image round trip failed: %s\n", asset.path.c_str());
                }
            }
        }
        std::printf("  %zu images of kind %d\n", assets.size(), (int)kind);
        CHECK(!assets.empty());
        CHECK(failures == 0);
    }
}

Image EdgeGradient(int width, int height) {
    Image image(width, height, 4);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t* p = image.Pixel(x, y);
            p[0] = (uint8_t)(x * 255 / std::max(width - 1, 1));
            p[1] = (uint8_t)(y * 255 / std::max(height - 1, 1));
            p[2] = 0x80;
            p[3] = x == 0 ? 0 : 0xFF;
        }
    }
    return image;
}

void TestPm64ImageEncoding() {
    Pm64::ImageAsset asset;
    asset.path = "backgrounds/test_bg";
    asset.texture.type = Otr::TextureType::Palette4bpp;
    asset.texture.width = 32;
    asset.texture.height = 16;
    asset.texture.data.assign(Otr::BufferSize(asset.texture.type, 32, 16), 0);
    asset.tlutPath = "backgrounds/test_bg_pal0";
    asset.tlut = Otr::Texture{};
    asset.tlut->type = Otr::TextureType::RGBA16bpp;
    asset.tlut->width = 16;
    asset.tlut->height = 1;
    asset.tlut->data.assign(32, 0);

    std::string error;
    CHECK(!Pm64::EncodeImage(asset, EdgeGradient(16, 16), &error) && !error.empty());

    const auto reduced = Pm64::EncodeImage(asset, EdgeGradient(32, 16));
    CHECK(reduced && reduced->reducedColors && !reduced->mappedToPalette);
    CHECK(reduced && reduced->textures.size() == 2 && reduced->textures[0].first == asset.path &&
          reduced->textures[1].first == asset.tlutPath);
    CHECK(reduced && ImageOps::CountColors(reduced->preview, 64) <= 16);
    CHECK(reduced && reduced->preview.Pixel(0, 3)[3] == 0 && reduced->preview.Pixel(5, 3)[3] == 0xFF);

    if (reduced) {
        const auto written = Otr::ExportTexture(reduced->textures[0].second, "test_bg");
        const auto read = Otr::Texture::Read(std::vector<uint8_t>(written.begin(), written.end()));
        const auto tlut = Otr::ExportTexture(reduced->textures[1].second, "test_bg_pal0");
        const auto readTlut = Otr::Texture::Read(std::vector<uint8_t>(tlut.begin(), tlut.end()));
        CHECK(read && readTlut && read->width == 32 && read->type == Otr::TextureType::Palette4bpp);
        const auto decoded = read && readTlut ? Otr::TextureCodec::ToImage(*read, &*readTlut) : std::nullopt;
        CHECK(decoded && ImageOps::Convert(*decoded, 4).rgba == reduced->preview.rgba);

        asset.texture = reduced->textures[0].second;
        asset.tlut = reduced->textures[1].second;
        asset.sharedPalette = true;
        const auto mapped = Pm64::EncodeImage(asset, EdgeGradient(32, 16));
        CHECK(mapped && mapped->textures.size() == 1 && mapped->mappedToPalette);
        CHECK(mapped && ImageOps::CountColors(mapped->preview, 64) <= 16);
    }

    Image few(32, 16, 4);
    for (size_t i = 0; i < few.rgba.size(); i += 4) {
        few.rgba[i] = (i / 4) % 3 == 0 ? 0xFF : 0x00;
        few.rgba[i + 3] = 0xFF;
    }
    asset.sharedPalette = false;
    const auto exact = Pm64::EncodeImage(asset, few);
    CHECK(exact && !exact->reducedColors && exact->preview.rgba == few.rgba);
}

void TestFontGenerator() {
    FontGenerator generator;
    CHECK(generator.Load(fs::path(NEORETRO_ASSETS_DIR) / "fonts/GoogleSans-Regular.ttf"));
    FontGenerator::Settings settings;
    const Image table = generator.RenderTable(settings);
    CHECK(table.width == 256 && table.height == 1024);
    int lit = 0;
    for (size_t i = 3; i < table.rgba.size(); i += 4) {
        lit += table.rgba[i] > 0;
    }
    CHECK(lit > 1000);
}

void TestGlidenOrderAndAliases() {
    const fs::path dir = TempDir("gliden");
    for (const char* file : { "b/z.png", "a.png", "b/Y.png", ".hidden/h.png", "~!~skip/s.png", "C.png" }) {
        Paths::WriteFile(dir / file, { 1 });
    }
    std::vector<std::string> order;
    for (const auto& file : TextureReplacement::ListInGlidenOrder(dir)) {
        order.push_back(Paths::Relative(file, dir));
    }
    CHECK((order == std::vector<std::string>{ "a.png", "b/Y.png", "b/z.png", "C.png" }));

    const fs::path pack = TempDir("aliases");
    const Image image = Gradient(8, 8);
    const auto png = ImageIO::EncodePng(image);
    nlohmann::json manifest;
    for (const char* name : { "tex/one", "tex/two", "sprite/foo_tex_3" }) {
        auto entry = Entry(Otr::TextureType::RGBA32bpp, 8, 8);
        entry.hash = "stale";
        manifest[name] = entry.ToJson();
    }
    Paths::WriteText(pack / "manifest.json", manifest.dump());
    Paths::WriteText(pack / "aliases.json", R"({ "one.png": "tex/one", "old/moved.png": "tex/two" })");
    Paths::WriteFile(pack / "b/one.png", png);
    Paths::WriteFile(pack / "a/one.png", png);
    Paths::WriteFile(pack / "new/moved.png", png);
    Paths::WriteFile(pack / "sprite/foo_tex_3_frame_2.png", png);

    const auto staged = TextureReplacement::ProcessFolder(pack);
    CHECK(staged.has_value());
    if (!staged) {
        return;
    }
    const auto find = [&](const std::string& target) -> const TexturePair* {
        for (const auto& [key, pairs] : *staged) {
            for (const auto& pair : pairs) {
                if (pair.second.targetName == target) {
                    return &pair;
                }
            }
        }
        return nullptr;
    };
    const auto* one = find("tex/one");
    const auto* two = find("tex/two");
    const auto* frame = find("sprite/foo_tex_3_frame_2");
    CHECK(one && Paths::Relative(one->first, pack) == "a/one.png");
    CHECK(two && Paths::Relative(two->first, pack) == "new/moved.png");
    CHECK(frame && frame->second.kind == TextureEntryKind::Additive);
    size_t total = 0;
    for (const auto& [key, pairs] : *staged) {
        total += pairs.size();
    }
    CHECK(total == 3);
}

void TestThreadPool() {
    ThreadPool pool(4);
    std::vector<int> values(10000, 0);
    pool.ParallelFor(values.size(), [&](size_t i) { values[i] = (int)i * 2; });
    bool ordered = true;
    for (size_t i = 0; i < values.size(); i++) {
        ordered &= values[i] == (int)i * 2;
    }
    CHECK(ordered);

    std::atomic<int> total = 0;
    std::vector<std::future<void>> outer;
    for (int i = 0; i < 8; i++) {
        outer.push_back(pool.Submit([&] { pool.ParallelFor(100, [&](size_t) { total++; }); }));
    }
    for (auto& future : outer) {
        future.wait();
    }
    CHECK(total == 800);

    bool threw = false;
    try {
        pool.ParallelFor(50, [](size_t i) {
            if (i == 17) {
                throw std::runtime_error("boom");
            }
        });
    } catch (const std::runtime_error&) { threw = true; }
    CHECK(threw);
    CHECK(pool.Submit([] { return 42; }).get() == 42);
}

void TestRealArchive() {
    const char* path = std::getenv("NEORETRO_TEST_ARCHIVE");
    if (path == nullptr) {
        std::printf("  (skipped: set NEORETRO_TEST_ARCHIVE)\n");
        return;
    }
    auto reader = Otr::ArchiveReader::Open(path);
    CHECK(reader != nullptr);
    if (!reader) {
        return;
    }
    size_t files = 0;
    size_t textures = 0;
    size_t decoded = 0;
    std::map<Otr::TextureType, int> types;
    reader->ForEach([&](const std::string& name, const std::vector<uint8_t>& data) {
        files++;
        if (const auto texture = Otr::Texture::Read(data)) {
            textures++;
            types[texture->type]++;
            decoded += Otr::TextureCodec::ToImage(*texture).has_value();
        }
    });
    std::printf("  %zu files, %zu textures, %zu decoded\n", files, textures, decoded);
    for (const auto& [type, count] : types) {
        std::printf("    %s: %d\n", std::string(Otr::TextureTypeName(type)).c_str(), count);
    }
    CHECK(files > 0);
    CHECK(decoded == textures);

    const fs::path dir = TempDir("real");
    const auto start = std::chrono::steady_clock::now();
    const auto extracted = TextureReplacement::ProcessOtr({ path }, dir);
    const auto seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    CHECK(extracted.has_value());
    if (!extracted) {
        return;
    }
    const fs::path folder = dir / TextureReplacement::OutputFolderName(path);
    TextureReplacement::WriteManifest(folder / "manifest.json", *extracted);
    const auto staged = TextureReplacement::ProcessFolder(folder);
    std::printf("  extracted %zu textures in %.1fs, %zu folders restaged\n", extracted->size(), seconds,
                staged ? staged->size() : 0);
    CHECK(staged && staged->empty());
}
} // namespace

int main() {
    spdlog::set_level(spdlog::level::warn);
    const std::vector<TestCase> tests = {
        { "header layout", TestHeaderLayout },
        { "codec round trips", TestCodecRoundTrips },
        { "png palette preserved", TestPngPalettePreserved },
        { "sequence", TestSequence },
        { "archives", TestArchives },
        { "texture builder rules", TestTextureBuilderRules },
        { "bk64 conventions", TestBk64Conventions },
        { "replace textures flow", TestReplaceTexturesFlow },
        { "generate custom and sequences", TestGenerateCustomAndSequences },
        { "sm64 text", TestSm64Text },
        { "real sm64 archive", TestRealSm64Archive },
        { "pm64 text", TestPm64Text },
        { "real pm64 archive", TestRealPm64Archive },
        { "pm64 image encoding", TestPm64ImageEncoding },
        { "update feed", TestUpdateFeed },
        { "font generator", TestFontGenerator },
        { "gliden order and aliases", TestGlidenOrderAndAliases },
        { "thread pool", TestThreadPool },
        { "real archive", TestRealArchive },
    };
    for (const auto& test : tests) {
        const int before = sFailures;
        std::printf("%s\n", test.name);
        test.fn();
        std::printf("  %s\n", sFailures == before ? "ok" : "FAILED");
    }
    std::printf("%d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
