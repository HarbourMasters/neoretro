#pragma once

#include "services/UpdateFeed.h"

#include <string>

namespace Updater {
enum class Status { Idle, Checking, Available, Downloading, Installing, UpToDate, Failed };

void Start();
void Tick();

void Check(bool silent);
void Install();
void Later();
void Skip();
void Close();
void ShowDialog();
void OpenReleasesPage();

Status GetStatus();
bool DialogVisible();
bool UpdateAvailable();
bool CanInstall();
bool QuitRequested();
float Progress();

const UpdateFeed::Release& Pending();
const std::string& Message();
std::string CurrentVersion();

bool AutoCheck();
void SetAutoCheck(bool enabled);
} // namespace Updater
