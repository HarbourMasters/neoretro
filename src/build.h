#ifndef BUILD_H
#define BUILD_H

#include <cstdint>

// neoretro Version information
extern const char gBuildVersion[];
extern const uint16_t gBuildVersionMajor;
extern const uint16_t gBuildVersionMinor;
extern const uint16_t gBuildVersionPatch;

extern const char gGitBranch[];
extern const char gGitCommitHash[];
extern const char gGitCommitTag[];
extern const char gBuildTeam[];
extern const char gBuildDate[];
extern const char gBuildMakeOption[];

extern const uint32_t gVersionCode;
extern const char gRepositoryUrl[];
extern const char gReleaseJsonUrl[];
extern const char gReleasesUrl[];

#endif
