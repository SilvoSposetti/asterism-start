#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <cstring>

void VK_CHECK(VkResult result);

void VK_CHECK(VkResult result, const char *message);


void print(const char *message);

void print(std::string message);


void log(const char *message);

void log(std::string message);


void logTitle(const char *message);

void logTitle(std::string message);


std::string padText(std::string text, int maxLineLength);
